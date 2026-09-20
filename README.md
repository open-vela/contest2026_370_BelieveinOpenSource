# 正点原子 DNN647 开发板 openvela 适配

本项目面向正点原子 DNN647 开发板（STM32N647），完成 openvela/NuttX 的板级适配，包含 UART/NSH、GPIO 以及 ES8388 音频 Codec 控制接口。

## 已完成适配

### UART 与 NSH

- 完成 STM32N647 USART1 板级适配；
- USART1_TX：PE5；
- USART1_RX：PE6；
- 支持 UART 控制台输出；
- 支持进入 NuttShell（NSH）。

### GPIO

- 完成 STM32N6 GPIO 初始化和板级配置；
- 完成 DNN647 用户 LED 控制；
- DS1/LED1 连接 PE10，低电平点亮；
- 提供独立 `nsh_gpio_led` 构建配置。

### ES8388 控制接口

- 完成 ES8388 控制接口的 GPIO 软件 I2C 实现；
- SCL：PE13；
- SDA：PE14；
- ES8388 7 位 I2C 地址：`0x10`；
- 支持地址 ACK 探测；
- 探测成功时输出 `ES8388_PRESENT addr=0x10`；
- 探测完成后继续进入 NSH。

## 代码结构

```text
board/contest_board/
├── configs/
│   ├── nsh/
│   ├── nsh_debug/
│   ├── nsh_es8388/
│   └── nsh_gpio_led/
├── include/                  # DNN647 引脚和板级定义
├── patches/                  # STM32N6 NuttX 芯片层补丁
├── scripts/                  # 构建和补丁脚本
├── src/                      # 板级初始化和构建集成
└── tools/                    # SRAM/GDB 加载工具
```

`patches/nuttx-stm32n6.patch` 用于接入 STM32N6 架构目录、启动代码、RCC、GPIO、UART、中断及相关 Kconfig 配置。

## 构建配置

在 openvela 工作区根目录执行：

```bash
./build.sh vendor/openvela/boards/contest2026_370_board/configs/nsh_debug --cmake -j2
./build.sh vendor/openvela/boards/contest2026_370_board/configs/nsh_es8388 --cmake -j2
./build.sh vendor/openvela/boards/contest2026_370_board/configs/nsh_gpio_led --cmake -j2
```

构建完成标志：

```text
#### build completed successfully
```

## 官方资料

- [硬件适配赛道指南](https://github.com/open-vela/docs/blob/dev-ai-contest-2026/zh-cn/contest_2026/hardware_porting/hardware_porting_track_guide.md)
- [代码提交指南](https://github.com/open-vela/docs/blob/dev-ai-contest-2026/zh-cn/contest_2026/code_submission_guide.md)
- [AI Coding 日志手册](https://github.com/open-vela/docs/blob/dev-ai-contest-2026/zh-cn/contest_2026/ai_coding_log_guide.md)

## 两套演示配置的复现命令

下面两套流程可以分别独立执行。Windows PowerShell 中每套流程都重新定义路径变量，不依赖上一套流程。

### ES8388 地址 ACK 与 NSH

先在 WSL 终端构建：

```bash
cd /home/dev/openvela-workspace
./build.sh vendor/openvela/boards/contest2026_370_board/configs/nsh_es8388 --cmake -j2
```

再打开一个 Windows PowerShell，执行以下完整命令：

```powershell
Push-Location '\\wsl.localhost\Ubuntu-22.04\home\dev\openvela-workspace'
Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass
$tool = '.\contest2026_370_BelieveinOpenSource\board\contest_board\tools\run_sram_image.ps1'
$elf = '.\cmake_out\contest2026_370_board_nsh_es8388\nuttx'
& $tool -ElfPath $elf
```

串口助手要先打开 COM4，参数为 `115200, 8N1`；BOOT0 置 GND、BOOT1 置 3.3V。预期看到 `ES8388_PRESENT addr=0x10`，随后出现 `nsh>`。

### GPIO 用户 LED

先在 WSL 终端构建：

```bash
cd /home/dev/openvela-workspace
./build.sh vendor/openvela/boards/contest2026_370_board/configs/nsh_gpio_led --cmake -j2
```

如果当前有其他 GDB 会话，在对应 GDB 窗口按 `Ctrl+C`，再输入 `quit`。然后打开一个新的 Windows PowerShell，执行以下完整命令：

```powershell
Push-Location '\\wsl.localhost\Ubuntu-22.04\home\dev\openvela-workspace'
Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass
$tool = '.\contest2026_370_BelieveinOpenSource\board\contest_board\tools\run_sram_image.ps1'
$elf = '.\cmake_out\contest2026_370_board_nsh_gpio_led\nuttx'
& $tool -ElfPath $elf
```

BOOT0 置 GND、BOOT1 置 3.3V，COM4 保持 `115200, 8N1`。预期 DS1（PE10/LED1，低电平有效）点亮，并在串口进入 `nsh>`。该配置证明板级 GPIO 输出，不等同于完整 NuttX LED 上半部驱动。
