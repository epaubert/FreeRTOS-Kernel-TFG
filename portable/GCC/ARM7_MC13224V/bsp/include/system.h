/*
 * Sistemas operativos empotrados
 * Configuración del sistema
 */

#ifndef __SYSTEM_H_
#define __SYSTEM_H_

#include "excep.h"
#include "dev.h"

#include "itc.h"
#include "gpio.h"
#include "uart.h"
#include "timers.h"

/*
 * Configuración de la CPU
 */

/* Frecuencia de la CPU por defecto (24 MHz) */
#define CPU_FREQ               24000000u

/* Máximo número de dispositivos gestionables por el BSP */
#define BSP_MAX_DEV 8

/* Máximo número de ficheros (dispositivos) abiertos simultánemente */
#define BSP_MAX_FD 8

/*
 * Configuración del GPIO
 */
#define GPIO_BASE		((void *) 0x80000000)
#define GPIO_ID			(0)						/* Solo hay un GPIO */
#define GPIO_NAME 		"/dev/gpio"

/*
 * Configuración de las UART
 */
#define UART1_BASE 		((void *) 0x80005000)
#define UART1_ID		(uart_1)
#define UART1_BAUDRATE	(115200)
#define UART1_NAME 		"/dev/uart1"

#define UART2_BASE 		((void *) 0x8000b000)
#define UART2_ID		(uart_2)
#define UART2_BAUDRATE	(115200)
#define UART2_NAME 		"/dev/uart2"

/*
 * Configuración de E/S estándar
 */
#define BSP_STDOUT     UART1_NAME
#define BSP_STDIN      UART1_NAME
#define BSP_STDERR     UART1_NAME

/*
 * Configuración del ITC
 */
#define ITC_BASE		((void *) 0x80020000)


/*
 * Configuración de los timers
 */
#define TMR_BASE		((void *) 0x80007000)

#define TMR0_MASK		0x00
#define TMR0_NAME 		"/dev/tmr0"
#define TMR1_MASK 		0x20
#define TMR1_NAME 		"/dev/tmr1"
#define TMR2_MASK 		0x40
#define TMR2_NAME 		"/dev/tmr2"
#define TMR3_MASK 		0x60
#define TMR3_NAME 		"/dev/tmr3"

#endif /* __SYSTEM_H_ */
