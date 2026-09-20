# 正点原子 STM32N647 DNN647 板级适配

本目录通过 manifest 映射到 openvela `vendor/openvela/boards/contest2026_370_board`。
当前目标是通过正点原子提供的 FSBL 从外部 XSPI NOR 正式启动，并提供 USART1/NSH 最小系统。屏幕、传感器和音频暂不纳入 MVP。

## ES8388 Level 0

`nsh_es8388` 是独立的 SRAM 调试配置，保留正式 NOR 启动配置和 UART
配置不变。它在 `board_late_initialize()` 中通过 PE13 (SCL) 和 PE14
(SDA) GPIO 软件 I2C 发送 ES8388 的写地址 `0x20`，并采样第九个时钟的
ACK。构建命令：

```bash
cd /home/dev/openvela-workspace
./build.sh vendor/openvela/boards/contest2026_370_board/configs/nsh_es8388 -j8
```

实机 SRAM 验证输出为 `ES8388_PRESENT addr=0x10`，随后进入 `nsh>`。
该结果只证明地址 ACK；不代表 ES8388 寄存器读写、SAI、DMA、PCM 采集或
麦克风音量检测已经完成。完整构建、烧录和原始串口证据记录在
`C:\Users\Fu_sheng\Desktop\workspace\OPENVELA_DNN647_ES8388_LEVEL0_2026-09-20.md`。

## GPIO LED Level 0

`nsh_gpio_led` is a separate SRAM configuration for the board user LEDs. It
configures `PG10/LED0` and `PE10/LED1` as active-low push-pull outputs. The
visible DS1 is `PE10/LED1`; `board_late_initialize()` selects LED1 for the
candidate's board-owned output.

Build command:

```bash
cd /home/dev/openvela-workspace
./build.sh vendor/openvela/boards/contest2026_370_board/configs/nsh_gpio_led -j8
```

GPIO HIL confirmed that DS1 lights when PE10 is a low output. This is GPIO
BSP evidence, not a NuttX LED upper-half or a NOR boot claim. The WS2812B RGB
chain on PF1 is intentionally deferred because it requires sub-microsecond
waveform timing.
