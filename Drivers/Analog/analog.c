#include "analog.h"

#include "adc.h"
#include "main.h"
#include "opamp.h"

#define ANALOG_VDDA_MV        3300U
#define ANALOG_ADC_MAX_COUNT  4095U
#define ANALOG_SAMPLES        8U
#define ANALOG_TIMEOUT_MS     10U

/* Tiempo de muestreo largo: las salidas de los divisores y de los OPAMP
 * internos necesitan tiempo para cargar el condensador de muestreo. */
#define ANALOG_SAMPLING_TIME  ADC_SAMPLETIME_640CYCLES_5

/* ADC5 no está en CubeMX: se gestiona aquí (ver analog.h) */
static ADC_HandleTypeDef hadc5;

typedef struct {
  ADC_HandleTypeDef *hadc;
  uint32_t channel;
} Analog_Source_t;

static const Analog_Source_t sources[ANALOG_COUNT] = {
    [ANALOG_V_3V3] = {&hadc2, ADC_CHANNEL_15},
    [ANALOG_V_5V] = {&hadc1, ADC_CHANNEL_1},
    [ANALOG_V_11V] = {&hadc1, ADC_CHANNEL_3},
    [ANALOG_V_PYRO1] = {&hadc2, ADC_CHANNEL_13},
    [ANALOG_V_PYRO2] = {&hadc1, ADC_CHANNEL_11},
    [ANALOG_I_3V3] = {&hadc4, ADC_CHANNEL_4},
    [ANALOG_I_5V] = {&hadc1, ADC_CHANNEL_4},
    [ANALOG_I_11V] = {&hadc2, ADC_CHANNEL_VOPAMP3_ADC2},
    [ANALOG_I_PYRO1] = {&hadc2, ADC_CHANNEL_VOPAMP2},
    [ANALOG_I_PYRO2] = {&hadc5, ADC_CHANNEL_VOPAMP4},
};

static void ADC5_Init(void)
{
    /* Reloj ADC345: la fuente (SYSCLK) ya la fija HAL_ADC_MspInit() de ADC4.
     * HAL_ADC_MspInit() no tiene rama para ADC5, así que el reloj se activa
     * aquí antes de HAL_ADC_Init(). */
    __HAL_RCC_ADC345_CLK_ENABLE();

    /* Misma configuración que ADC1/2/4 en adc.c */
    hadc5.Instance = ADC5;
    hadc5.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
    hadc5.Init.Resolution = ADC_RESOLUTION_12B;
    hadc5.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc5.Init.GainCompensation = 0;
    hadc5.Init.ScanConvMode = ADC_SCAN_DISABLE;
    hadc5.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    hadc5.Init.LowPowerAutoWait = DISABLE;
    hadc5.Init.ContinuousConvMode = DISABLE;
    hadc5.Init.NbrOfConversion = 1;
    hadc5.Init.DiscontinuousConvMode = DISABLE;
    hadc5.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc5.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hadc5.Init.DMAContinuousRequests = DISABLE;
    hadc5.Init.Overrun = ADC_OVR_DATA_PRESERVED;
    hadc5.Init.OversamplingMode = DISABLE;
    if (HAL_ADC_Init(&hadc5) != HAL_OK)
    {
        Error_Handler();
    }
}

/* Una conversión: selecciona el canal, convierte y devuelve la cuenta */
static bool ConvertOnce(const Analog_Source_t *src, uint32_t *raw)
{
    if (HAL_ADC_Start(src->hadc) != HAL_OK)
    {
        return false;
    }

    bool ok = (HAL_ADC_PollForConversion(src->hadc, ANALOG_TIMEOUT_MS) == HAL_OK);
    if (ok)
    {
        *raw = HAL_ADC_GetValue(src->hadc);
    }

    (void)HAL_ADC_Stop(src->hadc);
    return ok;
}

void Analog_Init(void)
{
    /* +5V_VSensing (PA0) y +11V_VSensing (PA2) no están en el .ioc */
    GPIO_InitTypeDef gpio = {0};
    __HAL_RCC_GPIOA_CLK_ENABLE();
    gpio.Pin = GPIO_PIN_0 | GPIO_PIN_2;
    gpio.Mode = GPIO_MODE_ANALOG;
    gpio.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &gpio);

    ADC5_Init();

    /* ADC1 y ADC4 los calibra CurrentSense_Init() */
    if ((HAL_ADCEx_Calibration_Start(&hadc2, ADC_SINGLE_ENDED) != HAL_OK) ||
        (HAL_ADCEx_Calibration_Start(&hadc5, ADC_SINGLE_ENDED) != HAL_OK))
    {
        Error_Handler();
    }

    /* MX_OPAMPx_Init() los configura pero no los arranca: sin esto las
     * medidas de corriente de 11 V, Pyro1 y Pyro2 no llegan al ADC. */
    if ((HAL_OPAMP_Start(&hopamp2) != HAL_OK) ||
        (HAL_OPAMP_Start(&hopamp3) != HAL_OK) ||
        (HAL_OPAMP_Start(&hopamp4) != HAL_OK))
    {
        Error_Handler();
    }
}

bool Analog_ReadPinMillivolts(Analog_Channel_t channel, uint16_t *millivolts)
{
    if ((channel >= ANALOG_COUNT) || (millivolts == NULL))
    {
        return false;
    }

    const Analog_Source_t *src = &sources[channel];

    ADC_ChannelConfTypeDef config = {0};
    config.Channel = src->channel;
    config.Rank = ADC_REGULAR_RANK_1;
    config.SamplingTime = ANALOG_SAMPLING_TIME;
    config.SingleDiff = ADC_SINGLE_ENDED;
    config.OffsetNumber = ADC_OFFSET_NONE;
    config.Offset = 0;
    if (HAL_ADC_ConfigChannel(src->hadc, &config) != HAL_OK)
    {
        return false;
    }

    /* La primera conversión tras cambiar de canal se descarta: el
     * condensador de muestreo aún guarda la carga del canal anterior. */
    uint32_t raw = 0U;
    if (!ConvertOnce(src, &raw))
    {
        return false;
    }

    uint32_t sum = 0U;
    for (uint8_t i = 0U; i < ANALOG_SAMPLES; i++)
    {
        if (!ConvertOnce(src, &raw))
        {
            return false;
        }
        sum += raw;
    }

    uint32_t average = sum / ANALOG_SAMPLES;
    *millivolts = (uint16_t)((average * ANALOG_VDDA_MV) / ANALOG_ADC_MAX_COUNT);
    return true;
}
