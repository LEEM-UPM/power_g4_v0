#include "utils_gpio.h"
#include "adc.h"
#include "stm32g4xx_hal_gpio.h"

#define PYRO1_GPIO_PORT GPIOC
#define PYRO1_GPIO_PIN GPIO_PIN_13
#define PYRO2_GPIO_PORT GPIOA
#define PYRO2_GPIO_PIN GPIO_PIN_10
#define PYRO1A_GPIO_PORT GPIOC
#define PYRO1A_GPIO_PIN GPIO_PIN_15
#define PYRO1B_GPIO_PORT GPIOB
#define PYRO1B_GPIO_PIN GPIO_PIN_1
#define PYRO2A_GPIO_PORT GPIOB
#define PYRO2A_GPIO_PIN GPIO_PIN_2
#define PYRO2B_GPIO_PORT GPIOB
#define PYRO2B_GPIO_PIN GPIO_PIN_10

#define PYRO1A_CONTINUITY_GPIO_PORT GPIOA
#define PYRO1A_CONTINUITY_GPIO_PIN GPIO_PIN_6
#define PYRO1B_CONTINUITY_GPIO_PORT GPIOA
#define PYRO1B_CONTINUITY_GPIO_PIN GPIO_PIN_7
#define PYRO2A_CONTINUITY_GPIO_PORT GPIOA
#define PYRO2A_CONTINUITY_GPIO_PIN GPIO_PIN_9
#define PYRO2B_CONTINUITY_GPIO_PORT GPIOA
#define PYRO2B_CONTINUITY_GPIO_PIN GPIO_PIN_8

#define INA138_GAIN                   78.0f
#define SHUNT_RESISTANCE_OHMS         0.005f

#define THREEV_ADC_MAX_COUNT          4095.0f
#define THREEV_ADC_VREF_VOLTS         3.3f
#define THREEV_ADC_SAMPLES            8U
#define THREEV_ADC_TIMEOUT_MS         10U
#define THREEV_CURRENT_FILTER_ALPHA 1.0f

#define FIVEV_ADC_MAX_COUNT          4095.0f
#define FIVEV_ADC_VREF_VOLTS         3.3f
#define FIVEV_ADC_SAMPLES             8U
#define FIVEV_ADC_TIMEOUT_MS          10U
#define FIVEV_CURRENT_FILTER_ALPHA    1.0f
#define ADC_STARTUP_DELAY_MS          50U
#define ADC_WARMUP_DISCARD_SAMPLES    32U

static void DiscardAdcSamples(ADC_HandleTypeDef *hadc, uint32_t timeout_ms, uint8_t samples)
{
    for (uint8_t i = 0U; i < samples; i++)
    {
        if (HAL_ADC_Start(hadc) == HAL_OK)
        {
            if (HAL_ADC_PollForConversion(hadc, timeout_ms) == HAL_OK)
            {
                (void)HAL_ADC_GetValue(hadc);
            }
            (void)HAL_ADC_Stop(hadc);
        }
    }
}

static HAL_StatusTypeDef ReadAdcAverage(ADC_HandleTypeDef *hadc, uint8_t samples, uint32_t timeout_ms, uint32_t *raw_average)
{
    uint32_t acc = 0U;

    if ((hadc == NULL) || (raw_average == NULL) || (samples == 0U))
    {
        return HAL_ERROR;
    }

    /* Dummy conversion to settle the ADC sampling capacitor before averaging. */
    if (HAL_ADC_Start(hadc) != HAL_OK)
    {
        return HAL_ERROR;
    }

    if (HAL_ADC_PollForConversion(hadc, timeout_ms) != HAL_OK)
    {
        (void)HAL_ADC_Stop(hadc);
        return HAL_TIMEOUT;
    }

    (void)HAL_ADC_GetValue(hadc);

    if (HAL_ADC_Stop(hadc) != HAL_OK)
    {
        return HAL_ERROR;
    }

    for (uint8_t i = 0U; i < samples; i++)
    {
        if (HAL_ADC_Start(hadc) != HAL_OK)
        {
            return HAL_ERROR;
        }

        if (HAL_ADC_PollForConversion(hadc, timeout_ms) != HAL_OK)
        {
            (void)HAL_ADC_Stop(hadc);
            return HAL_TIMEOUT;
        }

        acc += HAL_ADC_GetValue(hadc);

        if (HAL_ADC_Stop(hadc) != HAL_OK)
        {
            return HAL_ERROR;
        }
    }

    *raw_average = acc / samples;
    return HAL_OK;
}


void Pyro1_ON(void)
{
    HAL_GPIO_WritePin(PYRO1_GPIO_PORT, PYRO1_GPIO_PIN, GPIO_PIN_SET);
}

void Pyro1_OFF(void)
{
    HAL_GPIO_WritePin(PYRO1_GPIO_PORT, PYRO1_GPIO_PIN, GPIO_PIN_RESET);
}

void Pyro2_ON(void)
{
    HAL_GPIO_WritePin(PYRO2_GPIO_PORT, PYRO2_GPIO_PIN, GPIO_PIN_SET);
}

void Pyro2_OFF(void)
{
    HAL_GPIO_WritePin(PYRO2_GPIO_PORT, PYRO2_GPIO_PIN, GPIO_PIN_RESET);
}

void Pyro1A_ON(void)
{
    HAL_GPIO_WritePin(PYRO1A_GPIO_PORT, PYRO1A_GPIO_PIN, GPIO_PIN_SET);
}

void Pyro1A_OFF(void)
{
    HAL_GPIO_WritePin(PYRO1A_GPIO_PORT, PYRO1A_GPIO_PIN, GPIO_PIN_RESET);
}

void Pyro1B_ON(void)
{
    HAL_GPIO_WritePin(PYRO1B_GPIO_PORT, PYRO1B_GPIO_PIN, GPIO_PIN_SET);
}

void Pyro1B_OFF(void)
{
    HAL_GPIO_WritePin(PYRO1B_GPIO_PORT, PYRO1B_GPIO_PIN, GPIO_PIN_RESET);
}

void Pyro2A_ON(void)
{
    HAL_GPIO_WritePin(PYRO2A_GPIO_PORT, PYRO2A_GPIO_PIN, GPIO_PIN_SET);
}

void Pyro2A_OFF(void)
{
    HAL_GPIO_WritePin(PYRO2A_GPIO_PORT, PYRO2A_GPIO_PIN, GPIO_PIN_RESET);
}

void Pyro2B_ON(void)
{
    HAL_GPIO_WritePin(PYRO2B_GPIO_PORT, PYRO2B_GPIO_PIN, GPIO_PIN_SET);
}

void Pyro2B_OFF(void)
{
    HAL_GPIO_WritePin(PYRO2B_GPIO_PORT, PYRO2B_GPIO_PIN, GPIO_PIN_RESET);
}

void AllPyroCheck(void)
{
    Pyro1_ON();
    Pyro2_ON();
    HAL_Delay(1000); // Delay for 1000 milliseconds (1 second)
    Pyro1A_ON();
    HAL_Delay(1000); // Delay for 1000 milliseconds (1 second)
    Pyro1B_ON();
    HAL_Delay(1000); // Delay for 1000 milliseconds (1 second)
    Pyro2A_ON();
    HAL_Delay(1000); // Delay for 1000 milliseconds (1 second)
    Pyro2B_ON();
    HAL_Delay(1000); // Delay for 1000 milliseconds (1 second)

    Pyro1_OFF();
    Pyro2_OFF();
    Pyro1A_OFF();
    Pyro1B_OFF();
    Pyro2A_OFF();
    Pyro2B_OFF();
}

bool Pyro1A_Continuity(void)
{
    return (HAL_GPIO_ReadPin(PYRO1A_CONTINUITY_GPIO_PORT, PYRO1A_CONTINUITY_GPIO_PIN) == GPIO_PIN_SET);
}

bool Pyro1B_Continuity(void)
{
    return (HAL_GPIO_ReadPin(PYRO1B_CONTINUITY_GPIO_PORT, PYRO1B_CONTINUITY_GPIO_PIN) == GPIO_PIN_SET);
}

bool Pyro2A_Continuity(void)
{
    return (HAL_GPIO_ReadPin(PYRO2A_CONTINUITY_GPIO_PORT, PYRO2A_CONTINUITY_GPIO_PIN) == GPIO_PIN_SET);
}

bool Pyro2B_Continuity(void)
{
    return (HAL_GPIO_ReadPin(PYRO2B_CONTINUITY_GPIO_PORT, PYRO2B_CONTINUITY_GPIO_PIN) == GPIO_PIN_SET);
}

void Sys_init(void) {
    ADC_ChannelConfTypeDef adc4_config = {0};
    ADC_ChannelConfTypeDef adc1_config = {0};

    /* Allow VDDA/reference path and upstream analog front-end to settle after power-up. */
    HAL_Delay(ADC_STARTUP_DELAY_MS);

    if (HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED) != HAL_OK)
    {
        Error_Handler();
    }

    adc1_config.Channel = ADC_CHANNEL_4;
    adc1_config.Rank = ADC_REGULAR_RANK_1;
    adc1_config.SamplingTime = ADC_SAMPLETIME_640CYCLES_5;
    adc1_config.SingleDiff = ADC_SINGLE_ENDED;
    adc1_config.OffsetNumber = ADC_OFFSET_NONE;
    adc1_config.Offset = 0;

    if (HAL_ADC_ConfigChannel(&hadc1, &adc1_config) != HAL_OK)
    {
        Error_Handler();
    }

    if (HAL_ADC_Start(&hadc1) == HAL_OK)
    {
        if (HAL_ADC_PollForConversion(&hadc1, FIVEV_ADC_TIMEOUT_MS) == HAL_OK)
        {
            (void)HAL_ADC_GetValue(&hadc1);
        }
        (void)HAL_ADC_Stop(&hadc1);
    }

    DiscardAdcSamples(&hadc1, FIVEV_ADC_TIMEOUT_MS, ADC_WARMUP_DISCARD_SAMPLES);

    if (HAL_ADCEx_Calibration_Start(&hadc4, ADC_SINGLE_ENDED) != HAL_OK)
    {
        Error_Handler();
    }

    adc4_config.Channel = ADC_CHANNEL_4;
    adc4_config.Rank = ADC_REGULAR_RANK_1;
    adc4_config.SamplingTime = ADC_SAMPLETIME_640CYCLES_5;
    adc4_config.SingleDiff = ADC_SINGLE_ENDED;
    adc4_config.OffsetNumber = ADC_OFFSET_NONE;
    adc4_config.Offset = 0;

    if (HAL_ADC_ConfigChannel(&hadc4, &adc4_config) != HAL_OK)
    {
        Error_Handler();
    }

    /* One dummy conversion helps discharge residual charge from the ADC sampling capacitor. */
    if (HAL_ADC_Start(&hadc4) == HAL_OK)
    {
        if (HAL_ADC_PollForConversion(&hadc4, THREEV_ADC_TIMEOUT_MS) == HAL_OK)
        {
            (void)HAL_ADC_GetValue(&hadc4);
        }
        (void)HAL_ADC_Stop(&hadc4);
    }

    DiscardAdcSamples(&hadc4, THREEV_ADC_TIMEOUT_MS,
                      ADC_WARMUP_DISCARD_SAMPLES);

    CAN_Setup();

}

float ThreeV_Current(void)
{
    static float filtered_current_a = 0.0f;
    static bool filter_initialized = false;
    uint32_t raw_avg = 0U;
    float adc_voltage;
    float shunt_voltage;
    float current_a;

    if (ReadAdcAverage(&hadc4, THREEV_ADC_SAMPLES, THREEV_ADC_TIMEOUT_MS, &raw_avg) != HAL_OK)
    {
        return filtered_current_a;
    }

    adc_voltage = ((float)raw_avg / THREEV_ADC_MAX_COUNT) * THREEV_ADC_VREF_VOLTS;
    shunt_voltage = adc_voltage / INA138_GAIN;
    current_a = shunt_voltage / SHUNT_RESISTANCE_OHMS;

    if (!filter_initialized)
    {
        filtered_current_a = current_a;
        filter_initialized = true;
    }
    else
    {
        filtered_current_a += THREEV_CURRENT_FILTER_ALPHA * (current_a - filtered_current_a);
    }

    return filtered_current_a * 0.30356f + 0.005; // Correction
}

float FiveV_Current(void)
{
    static float filtered_current_a = 0.0f;
    static bool filter_initialized = false;
    uint32_t raw_avg = 0U;
    float adc_voltage;
    float shunt_voltage;
    float current_a;

    ADC_ChannelConfTypeDef adc1_config = {0};

    adc1_config.Channel = ADC_CHANNEL_4;
    adc1_config.Rank = ADC_REGULAR_RANK_1;
    adc1_config.SamplingTime = ADC_SAMPLETIME_640CYCLES_5;
    adc1_config.SingleDiff = ADC_SINGLE_ENDED;
    adc1_config.OffsetNumber = ADC_OFFSET_NONE;
    adc1_config.Offset = 0;

    if (HAL_ADC_ConfigChannel(&hadc1, &adc1_config) != HAL_OK)
    {
        return filtered_current_a;
    }

    if (ReadAdcAverage(&hadc1, FIVEV_ADC_SAMPLES, FIVEV_ADC_TIMEOUT_MS, &raw_avg) != HAL_OK)
    {
        return filtered_current_a;
    }

    adc_voltage = ((float)raw_avg / FIVEV_ADC_MAX_COUNT) * FIVEV_ADC_VREF_VOLTS;
    shunt_voltage = adc_voltage / INA138_GAIN;
    current_a = shunt_voltage / SHUNT_RESISTANCE_OHMS;

    if (!filter_initialized)
    {
        filtered_current_a = current_a;
        filter_initialized = true;
    }
    else
    {
        filtered_current_a += FIVEV_CURRENT_FILTER_ALPHA * (current_a - filtered_current_a);
    }

    return filtered_current_a * 0.372; // Correction
}

