# DNN647 Validation Record

## Hardware and scope

- Board: ALIENTEK DNN647, STM32N647X0H3Q
- Formal boot: FSBL + MX25UM25645G XSPI NOR
- Development boot: ST-LINK AP1 + SRAM at `0x34000400`
- Console: USART1 PE5/PE6, CH340, COM4, 115200 8N1

## Verified results

| Capability | Result | Evidence |
| --- | --- | --- |
| NOR formal boot | Passed | Full power-cycle output reached openvela and NSH. |
| UART TX and NSH | Passed | Startup text, `nsh>`, `help`, and `uname` were observed. |
| GPIO output | Passed | DS1 maps to PE10/LED1 and lights at a low output level. |
| ES8388 Level 0 | Passed | GPIO software I2C received ACK from address `0x10`. |

## ES8388 Level 0 reproducibility

Build the isolated SRAM configuration:

```bash
cd /home/dev/openvela-workspace
./build.sh vendor/openvela/boards/contest2026_370_board/configs/nsh_es8388 -j8
```

With `BOOT0=GND` and `BOOT1=3.3V`, load the SRAM ELF with ST-LINK AP1, then
set VTOR, SP, and PC from `0x34000400`. The expected console result is:

```text
ES8388_I2C_PROBE_START
ES8388_PRESENT addr=0x10
NuttShell (NSH)
nsh>
```

The validated candidate ELF SHA-256 is
`D4E953E27593B6809138E51E15C6CDE62733D517E70248E475737504283E92CC`.
The raw 95-byte serial capture SHA-256 is
`25D397392D7DE43C143F650C4FCBB7A722AC6019A9325C480651AA18A469E212`.

This is a GPIO software-I2C address ACK test only. It does not claim ES8388
register access, SAI, DMA, PCM capture, microphone level detection, or an
audio upper-half.

## GPIO LED scope

`nsh_gpio_led` configures `PG10/LED0` and `PE10/LED1` as active-low outputs.
The visible DS1 was confirmed as `PE10/LED1` through a GPIO BSRR HIL test
while the openvela SRAM image was running. RGB is deferred: the board uses a
10-pixel WS2812B chain on PF1, which requires a 1.25 us serial waveform and
is not part of this minimal GPIO BSP submission.
