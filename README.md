# openvela on 正点原子 DNN647

2026 首届 openvela AI 硬件开发者大赛，新硬件适配赛道作品。目标板为正点原子 DNN647，MCU 为 STM32N647X0H3Q。本作品实现了 STM32N6 芯片层与 DNN647 板级构建配置，目标启动链路为：

```text
FSBL -> MX25UM25645G XSPI NOR -> openvela/NuttX XIP image -> USART1 -> CH340 -> PC
```

当前 MVP 聚焦 NOR 启动、UART/NSH、GPIO 与 ES8388 地址 ACK 基线；屏幕 FPC
损坏，不作为验收依赖。麦克风 PCM、音量算法和 NPU 音频异常检测不在本提交范围。

## 代码结构

```text
board/contest_board/
  configs/nsh/       # 正式 NOR XIP 配置
  configs/nsh_debug/ # SRAM/GDB 调试配置
  configs/nsh_es8388/ # ES8388 GPIO software-I2C Level 0 配置
  configs/nsh_gpio_led/ # GPIO user LED 配置
  include/           # DNN647 引脚和电源域定义
  patches/           # STM32N6 NuttX 芯片层补丁
  scripts/           # 链接脚本和补丁应用脚本
  src/               # 板级启动和 CMake 集成
logs/                # AI Coding 日志
```

`patches/nuttx-stm32n6.patch` 包含 32 个文件：openvela 当前 NuttX 基线中缺失的 STM32N6 架构目录、Kconfig 接入、RCC/GPIO/UART/中断/启动代码。该补丁只保存在比赛专属仓，不直接提交公共仓。

## 环境与构建

以下命令在 repo 工作区根目录执行，专属仓目录为 `contest2026_370_BelieveinOpenSource/`：

```bash
# 只需在新的同步工作区执行一次
bash contest2026_370_BelieveinOpenSource/board/contest_board/scripts/apply_nuttx_patch.sh

./build.sh vendor/openvela/boards/contest2026_370_board/configs/nsh --cmake -j2

arm-none-eabi-objcopy -I binary \
  cmake_out/contest2026_370_board_nsh/nuttx.bin \
  --change-addresses=0x70100400 -O ihex \
  openvela-dnn647-nsh-xip.hex
```

本次验证使用 Ubuntu 22.04 WSL2、openvela 工作区自带的 GNU Arm Embedded Toolchain。正式构建成功标志为：

```text
#### build completed successfully
```

## 烧录

板卡使用正点原子资料包提供的 FSBL 与 MX25UM25645G 外部加载器。FSBL 首次写入 `0x70000000`；应用镜像写入 `0x70100400`。应用更新不应覆盖 FSBL。

Windows PowerShell 示例：

```powershell
$cli = 'D:\embeddedTool\STM32Dev\STM32CubeProgrammer\bin\STM32_Programmer_CLI.exe'
$loader = 'D:\...\MX25UM25645G_ATK-CNN647B_ExtMemLoader.stldr'
& $cli -c port=SWD mode=HOTPLUG -el $loader -w .\openvela-dnn647-nsh-xip.hex -v
```

BOOT 跳线（以正点原子资料和本板实测为准）：

| 用途 | BOOT0 | BOOT1 |
| --- | --- | --- |
| Flash boot，外部 NOR 正式启动 | GND | GND |
| Development boot，ST-LINK 烧录和 GDB 调试 | GND | 3.3V |

串口：`USART1_TX=PE5`、`USART1_RX=PE6`，经 P12 跳线连接 CH340；PC 侧参数为 `COM4, 115200, 8N1`。

## 验证记录（2026-09-20）

| 项目 | 结果 | 证据 |
| --- | --- | --- |
| STM32N6/DNN647 正式 XIP 配置编译 | 通过 | `build completed successfully` |
| NOR 地址 `0x70100400` 烧录与读回校验 | 通过 | CubeProgrammer: `Download verified successfully` |
| FSBL -> NOR -> 早期代码 -> USART1 TX | 通过 | Flash boot 下连续收到 `DNN647 EARLY UART TX PROBE` |
| SRAM/GDB 下 openvela NSH | 通过 | 收到 `NuttShell (NSH)` 和 `nsh>` |
| NOR Flash boot 下 openvela/NSH | 通过 | 完整上电后观察到启动文本和 `nsh>` |
| USART1 RX/NSH 命令输入 | 通过 | 已执行 `help`、`uname` 等 NSH 命令 |
| GPIO user LED | 通过 | DS1 = PE10/LED1，低电平点亮 |
| ES8388 GPIO software-I2C address ACK | 通过 | `ES8388_PRESENT addr=0x10` 后继续到 `nsh>` |

因此本提交包含可复现的芯片层、板级配置、正式 XIP 启动、UART/NSH、GPIO
输出和 ES8388 地址 ACK 基线。详细 HIL 记录和边界见
[`docs/VALIDATION.md`](docs/VALIDATION.md)。

## 已知问题与下一步

下一步是 ES8388 codec 寄存器读写，随后再评估 SAI、GPDMA、PCM 采集和
麦克风音量检测。RGB 使用 WS2812B 严格时序，保持在本次最小 BSP 提交之外。

## AI Coding Record

AI assistance was used for BSP scope analysis, NuttX board integration,
hardware-reference cross-checking, build verification, ST-LINK/GDB diagnosis,
and reproducibility documentation. Real AI Coding session logs are submitted
under `logs/` separately; generated firmware artifacts and local debug logs
are intentionally not committed.

## 官方资料

- [大赛总览](https://github.com/open-vela/docs/blob/dev-ai-contest-2026/zh-cn/contest_2026/contest_overview.md)
- [新硬件适配赛道指南](https://github.com/open-vela/docs/blob/dev-ai-contest-2026/zh-cn/contest_2026/hardware_porting/hardware_porting_track_guide.md)
- [代码提交指南](https://github.com/open-vela/docs/blob/dev-ai-contest-2026/zh-cn/contest_2026/code_submission_guide.md)
- [AI Coding 日志手册](https://github.com/open-vela/docs/blob/dev-ai-contest-2026/zh-cn/contest_2026/ai_coding_log_guide.md)
