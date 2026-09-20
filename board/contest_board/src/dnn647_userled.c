/****************************************************************************
 * DNN647 user LED support
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#ifdef CONFIG_DNN647_USERLEDS

#include <stdbool.h>
#include <stdint.h>

#include <arch/board/board.h>

#include "stm32_gpio.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* 正点原子 LED0 is PG10 and LED1 is PE10.  The official LED example
 * configures both as push-pull, pull-down, low-speed outputs.  A high level
 * turns each LED off.
 */

#define GPIO_DNN647_LED0 \
  (GPIO_OUTPUT | GPIO_PULLDOWN | GPIO_SPEED_2MHZ | GPIO_PUSHPULL | \
   GPIO_OUTPUT_SET | GPIO_PORTG | GPIO_PIN10)

#define GPIO_DNN647_LED1 \
  (GPIO_OUTPUT | GPIO_PULLDOWN | GPIO_SPEED_2MHZ | GPIO_PUSHPULL | \
   GPIO_OUTPUT_SET | GPIO_PORTE | GPIO_PIN10)

/****************************************************************************
 * Private Data
 ****************************************************************************/

static const uint32_t g_dnn647_ledcfg[BOARD_NLEDS] =
{
  GPIO_DNN647_LED0,
  GPIO_DNN647_LED1,
};

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int dnn647_userled_initialize(void)
{
  int ret;
  unsigned int led;

  for (led = 0; led < BOARD_NLEDS; led++)
    {
      ret = stm32_configgpio(g_dnn647_ledcfg[led]);
      if (ret < 0)
        {
          return ret;
        }
    }

  return 0;
}

void dnn647_userled_set(int led, bool ledon)
{
  if ((unsigned int)led < BOARD_NLEDS)
    {
      /* Both board LEDs are active low. */

      stm32_gpiowrite(g_dnn647_ledcfg[led], !ledon);
    }
}

#endif /* CONFIG_DNN647_USERLEDS */
