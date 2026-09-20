---
name: openvela-dnn647-debug
description: "Debug openvela/NuttX bring-up on 正点原子 DNN647 (STM32N647) with ST-LINK/GDB, NOR XIP, and USART1. Use for boot, NSH, UART, GDB attach, reset, or repeated-flash failures; not for ordinary application development."
---

# openvela DNN647 Debug

## Outcome and truth standard

Work toward a reproducible board-side observation, not a plausible explanation.

- Treat `Flash boot -> NOR XIP -> UART -> nsh>` as the formal MVP.
- Treat `Development boot -> ST-LINK/GDB -> SRAM -> nsh>` as a valid debug result only. Never describe it as NOR boot.
- Treat a CubeProgrammer `Download verified successfully` result as proof of write/readback, not proof that the firmware booted.
- Keep each claim bounded by its actual evidence: compilation, flash verification, cold-boot TX, NSH banner, and RX command response are separate facts.

## DNN647 fixed facts

- Board/MCU: 正点原子 DNN647 / STM32N647X0H3Q.
- UART: USART1 `PE5` TX and `PE6` RX through P12 to CH340; PC is `COM4`, `115200`, `8N1` unless device enumeration proves otherwise.
- Flash boot: `BOOT0=GND`, `BOOT1=GND`.
- Development boot for ST-LINK: `BOOT0=GND`, `BOOT1=3.3V`.
- NOR layout: vendor FSBL begins at `0x70000000`; application XIP image begins at `0x70100400`. Do not overwrite FSBL when updating the application.
- ST-LINK GDB Server uses AP1 (`-m 1`). Windows reserves the usual 612xx ports; use GDB `55000` and SWV `55001`.
- A known SRAM image may have vectors at `0x34000400`. After `load`, explicitly set VTOR, SP, and PC from that vector table before `continue`.

Read [known-state.md](references/known-state.md) before a DNN647 boot investigation. It records current artifacts and their evidentiary status.

## Time-boxed workflow

1. **Freeze a baseline first.** Record the exact branch/commit, configuration, artifact SHA-256, boot straps, serial settings, and observed text. Do not overwrite a working ELF/HEX. Use a new candidate filename and a separate commit for source changes.
2. **State one falsifiable hypothesis.** Change only the smallest parameter set needed to test it. Avoid multi-probe edits unless their output is ordered, rate-limited, and each marker answers a distinct branch question.
3. **Run the shortest discriminating experiment.** Open the COM listener before release/`continue`; use a full power cycle when testing NOR cold boot. The reset button is not equivalent to a cold boot on this board.
4. **Report at least every 10 minutes**, even if no command has completed. The report must say: elapsed time; exact action; observed fact; whether it supports/refutes the hypothesis; current blocker; next action; remaining time in the time box.
5. **Stop one hypothesis after 20 minutes** without a new discriminating observation. Restore the known baseline, preserve logs, and either choose one new hypothesis or ask the user for direction. Do not loop through reflashes or resets without recording what differs.
6. **At 40 minutes total for one defect**, present an evidence table and a decision: one final bounded test, defer it, or switch to the known-good demo. Do not consume the entire session in GDB.

## Before every GDB or serial attempt

- Confirm the user has set and power-cycled the requested BOOT straps. Describe straps as `GND` or `3.3V`, not pin-number shorthand.
- Confirm P12 UART jumpers and identify the actual Windows COM port.
- Check whether the serial program or an old GDB server owns the port. Kill only the known stale `ST-LINK_gdbserver` process; never kill arbitrary processes.
- Start the listener before running the target. Capture raw bytes as well as terminal rendering when diagnosing mojibake or ANSI escape sequences.
- For GDB, confirm server port `55000`, AP1 mode, and attachment state before `load`. A failed attach, failed `load`, reset, and missing UART output are different failures.

## Efficient diagnostic choices

- Prefer a binary boundary test over repeated nearby prints: first prove whether execution reaches a function boundary, then subdivide only the failing interval.
- For UART TX, distinguish `lowputc` execution, UART status bits, and PC-visible bytes. For RX, distinguish electrical input, IRQ pending/status, driver ISR, buffer insertion, and NSH parsing.
- Do not infer an RX problem from a TX-only observation or infer a code boot problem from a serial-terminal reconnect.
- Preserve one known-good recovery path: NOR TX probe and/or SRAM NSH backup. Restore it promptly after a candidate fails.

## Handoff / submission discipline

- Update the project progress/runbook after material findings, including failed hypotheses and artifact hashes.
- Do not commit `artifacts/`, raw memory dumps, temporary GDB logs, or local tool binaries unless the user explicitly requests them.
- Before any remote push, PR, or release claim, show the user the exact files, test evidence, and unresolved gaps. A local commit is reversible; do not use force push or destructive reset to undo experiments.
