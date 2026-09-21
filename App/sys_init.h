#ifndef SYS_INIT_H
#define SYS_INIT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Inicia todo lo que la placa necesita tras los MX_*_Init() de CubeMX:
 * ADCs de medida de corriente, CAN1 (canNode) y CAN2. */
void Sys_init(void);

#ifdef __cplusplus
}
#endif

#endif /* SYS_INIT_H */
