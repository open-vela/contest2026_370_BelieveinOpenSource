/****************************************************************************
 * STM32N647 DNN647 board initialization
 ****************************************************************************/

#include <stddef.h>
#include <stdint.h>

#include <nuttx/arch.h>
#include <nuttx/board.h>

#include <arch/board/board.h>

/* Both the SRAM debug image and the XIP image use the 2 MiB AXI SRAM window
 * at 0x34000000.  STM32N6 has no chip-specific heap allocator yet, so the
 * generic ARM fallback would use CONFIG_RAM_START/RAM_SIZE (both zero for
 * this custom board) and create an invalid, wrapping heap range.
 */

#define DNN647_AXISRAM_END 0x34200000u

extern const uintptr_t g_idle_topstack;

void up_allocate_heap(void **heap_start, size_t *heap_size)
{
  *heap_start = (void *)g_idle_topstack;
  *heap_size = DNN647_AXISRAM_END - g_idle_topstack;
}

void stm32_board_initialize(void)
{
  /* USART1 GPIO and clocks are initialized by the STM32N6 common layer. */
}

void board_late_initialize(void)
{
#ifdef CONFIG_DNN647_USERLEDS
  if (dnn647_userled_initialize() >= 0)
    {
      /* DS1 is the visible board LED: LED1 / PE10, active low. */

      dnn647_userled_set(BOARD_LED1, true);
    }
#endif

#ifdef CONFIG_DNN647_ES8388_PROBE
  dnn647_es8388_probe();
#endif
}

int board_app_initialize(uintptr_t arg)
{
  UNUSED(arg);
  return 0;
}
