# DNN647 Known State

Update this reference only after a material hardware-in-the-loop result.

## Working baselines

| Result | Artifact | Evidence |
| --- | --- | --- |
| NOR cold-boot UART TX probe | `C:\Users\Fu_sheng\Desktop\openvela\artifacts\openvela-dnn647-xip-repeating-uart-tx-probe.hex` | Flash boot after a full power cycle continuously prints `DNN647 EARLY UART TX PROBE` on COM4. |
| SRAM/GDB NSH | `C:\Users\Fu_sheng\Desktop\openvela\artifacts\openvela-dnn647-txfix-tc.elf` | Development boot with VTOR/SP/PC set from `0x34000400` produced `NuttShell (NSH)` and `nsh>`. |

These results do not establish NOR Flash boot into NSH or stable NSH RX.

## Current gaps

| Item | Status |
| --- | --- |
| Production NOR image build and CubeProgrammer verification | Passed |
| NOR Flash boot emits `nsh>` | Not demonstrated |
| NSH `help` RX interaction | Not demonstrated |

## Evidence locations

- Actual contest repository: `C:\Users\Fu_sheng\Desktop\openvela`
- Existing local baseline commit: `b8a2260 feat: add STM32N647 DNN647 board port`
- Historical progress: `C:\Users\Fu_sheng\Desktop\workspace\OPENVELA_DNN647_PROGRESS_2026-09-17.md`
- Debug runbook: `C:\Users\Fu_sheng\Desktop\workspace\OPENVELA_DNN647_DEBUG_RUNBOOK_2026-09-19.md`

## Local tool paths

```text
CubeProgrammer CLI:
D:\embeddedTool\STM32Dev\STM32CubeProgrammer\bin\STM32_Programmer_CLI.exe

External loader:
D:\BaiduNetdiskDownload\N647SoftwareExtract\SoftwarePackage\External_Loader\
MX25UM25645G_ATK-CNN647B\Binary\MX25UM25645G_ATK-CNN647B_ExtMemLoader.stldr
```
