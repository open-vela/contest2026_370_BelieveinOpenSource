/****************************************************************************
 * DNN647 board definitions for the STM32N647X0H3Q.
 *
 * Values are taken from the 正点原子 05_Serial CubeN6 project and still
 * require hardware validation on the actual DNN647 board.
 ****************************************************************************/

#ifndef __BOARDS_CONTEST_BOARD_INCLUDE_BOARD_H
#define __BOARDS_CONTEST_BOARD_INCLUDE_BOARD_H

#include <nuttx/config.h>

#include <stdbool.h>
#include <stdint.h>

#include <arch/stm32n6/chip.h>

#define STM32_HSI_FREQUENCY 64000000ul

/* CubeN6 configures PLL1 as HSI / 4 * 75, with IC1 divide by 2. */
#define STM32_PLL1_M       4
#define STM32_PLL1_N       75
#define STM32_PLL1_IC1_DIV 2

/* CPU clock used by the SysTick setup.  This is the CubeN6 PLL1 result:
 * 64 MHz / 4 * 75 / 2 = 600 MHz.  Keep this aligned with the RCC setup until
 * the value is confirmed on the board with a debugger or clock output.
 */
#define STM32_CPUCLK_FREQUENCY 600000000ul

/* PE5/PE6 are USART1 AF7 on the DNN647 serial header.  The STM32N6 serial
 * sources include the chip-private pin map before these macros are expanded.
 */
#define GPIO_USART1_TX GPIO_USART1_TX_1
#define GPIO_USART1_RX GPIO_USART1_RX_1

/* PE5/PE6 are in the 1.8 V VDDIO2 domain in the Cube project.  The STM32N6
 * start source includes the chip-private PWR definitions before expansion.
 */
#define BOARD_PWR_VDDIO (PWR_SVMCR3_VDDIO2SV | PWR_SVMCR3_VDDIO2VRSEL)

/* The user LEDs are active low. */

#define BOARD_LED0     0
#define BOARD_LED1     1
#define BOARD_NLEDS    2
#define BOARD_LED0_BIT (1 << BOARD_LED0)
#define BOARD_LED1_BIT (1 << BOARD_LED1)

#ifndef __ASSEMBLY__
void stm32_board_initialize(void);
#ifdef CONFIG_DNN647_USERLEDS
int dnn647_userled_initialize(void);
void dnn647_userled_set(int led, bool ledon);
#endif
#ifdef CONFIG_DNN647_ES8388_PROBE
void dnn647_es8388_probe(void);
#endif
#ifdef CONFIG_DNN647_XIP_ENTRY_PROBE
void dnn647_xip_entry_probe(void);
#endif
#endif

#endif /* __BOARDS_CONTEST_BOARD_INCLUDE_BOARD_H */
