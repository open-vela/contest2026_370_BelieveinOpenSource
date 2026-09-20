/****************************************************************************
 * DNN647 ES8388 Level 0 GPIO I2C probe
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#ifdef CONFIG_DNN647_ES8388_PROBE

#include <stdint.h>
#include <syslog.h>

#include <nuttx/arch.h>

#include "stm32_gpio.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define DNN647_ES8388_SCL \
  (GPIO_OUTPUT | GPIO_PULLUP | GPIO_SPEED_50MHZ | GPIO_PUSHPULL | \
   GPIO_PORTE | GPIO_PIN13)

#define DNN647_ES8388_SDA \
  (GPIO_OUTPUT | GPIO_PULLUP | GPIO_SPEED_50MHZ | GPIO_OPENDRAIN | \
   GPIO_PORTE | GPIO_PIN14)

#define DNN647_ES8388_ADDR_WRITE 0x20

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static void dnn647_es8388_delay(void)
{
  up_udelay(2);
}

static void dnn647_es8388_scl(bool high)
{
  stm32_gpiowrite(DNN647_ES8388_SCL, high);
  dnn647_es8388_delay();
}

static void dnn647_es8388_sda(bool high)
{
  stm32_gpiowrite(DNN647_ES8388_SDA, high);
  dnn647_es8388_delay();
}

static void dnn647_es8388_start(void)
{
  dnn647_es8388_sda(true);
  dnn647_es8388_scl(true);
  dnn647_es8388_sda(false);
  dnn647_es8388_scl(false);
}

static void dnn647_es8388_stop(void)
{
  dnn647_es8388_sda(false);
  dnn647_es8388_scl(true);
  dnn647_es8388_sda(true);
}

static void dnn647_es8388_sendbyte(uint8_t data)
{
  unsigned int bit;

  for (bit = 0; bit < 8; bit++)
    {
      dnn647_es8388_sda((data & 0x80) != 0);
      dnn647_es8388_scl(true);
      dnn647_es8388_scl(false);
      data <<= 1;
    }

  /* Release SDA for the slave ACK bit. */

  dnn647_es8388_sda(true);
}

static int dnn647_es8388_waitack(void)
{
  bool nack;

  dnn647_es8388_sda(true);
  dnn647_es8388_scl(true);
  nack = stm32_gpioread(DNN647_ES8388_SDA);
  dnn647_es8388_scl(false);

  return nack ? 1 : 0;
}

static int dnn647_es8388_gpio_init(void)
{
  int ret;

  ret = stm32_configgpio(DNN647_ES8388_SCL);
  if (ret < 0)
    {
      return ret;
    }

  ret = stm32_configgpio(DNN647_ES8388_SDA);
  if (ret < 0)
    {
      return ret;
    }

  dnn647_es8388_sda(true);
  dnn647_es8388_scl(true);

  return 0;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

void dnn647_es8388_probe(void)
{
  int ret;
  int ack;

  syslog(LOG_INFO, "ES8388_I2C_PROBE_START\n");

  ret = dnn647_es8388_gpio_init();
  if (ret < 0)
    {
      syslog(LOG_INFO, "ES8388_NOT_FOUND ack=2\n");
      return;
    }

  if (!stm32_gpioread(DNN647_ES8388_SCL) ||
      !stm32_gpioread(DNN647_ES8388_SDA))
    {
      dnn647_es8388_stop();
      syslog(LOG_INFO, "ES8388_NOT_FOUND ack=3\n");
      return;
    }

  dnn647_es8388_start();
  dnn647_es8388_sendbyte(DNN647_ES8388_ADDR_WRITE);
  ack = dnn647_es8388_waitack();
  dnn647_es8388_stop();

  if (ack == 0)
    {
      syslog(LOG_INFO, "ES8388_PRESENT addr=0x10\n");
    }
  else
    {
      syslog(LOG_INFO, "ES8388_NOT_FOUND ack=%d\n", ack);
    }
}

#endif /* CONFIG_DNN647_ES8388_PROBE */
