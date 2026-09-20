---
name: openvela-wsl2-host-setup
description: "Set up or diagnose the Windows WSL2 host used to build openvela, including distribution storage, WSL virtual-disk errors, and VirtualBox coexistence. Use for WSL installation, import failures, build-host setup, or virtualization conflicts; not for board firmware debugging."
---

# openvela WSL2 Host Setup

## Scope and safety

This skill supports the Windows host that builds openvela. It does not change Windows virtualization, boot configuration, optional features, or an existing VirtualBox installation without explaining the impact and receiving explicit user approval.

Read [local-history.md](references/local-history.md) before diagnosing this machine. It includes the original evidence, final installation command, and research links. Its past evidence is useful but not a universal diagnosis.

## Known-good current setup

- WSL distribution: `Ubuntu-22.04`, WSL version 2.
- Distribution storage: `D:\WSL\distros\Ubuntu_22_04`.
- Host-visible source: `C:\Users\Fu_sheng\Desktop\openvela`, mounted in WSL as `/mnt/c/Users/Fu_sheng/Desktop/openvela`.
- Build copy: `/home/dev/openvela-workspace` is faster because it resides in the WSL Linux filesystem; it must be deliberately synchronized with the Windows contest repository.
- Baseline tools: Ubuntu 22.04, Git, repo launcher, Make, Python 3, and Arm GNU Toolchain are installed.

## Diagnose before changing settings

1. Identify the intended operation: install a distro, run an existing distro, relocate storage, build openvela, use USB/ST-LINK, or restore VirtualBox.
2. Collect only relevant read-only facts first:
   - `wsl --version`, `wsl --status`, `wsl -l -v`
   - WSL feature state and Virtual Machine Platform state from an elevated PowerShell when needed
   - `bcdedit /enum {current}` only to inspect `hypervisorlaunchtype`
   - requested service states (`WslService`, `vmcompute`, `vds`, `vhdmp`) only if the error concerns VHD/WSL import
3. Separate symptom from suspected cause. Do not state that VirtualBox caused a WSL failure unless an experiment or log proves it.
4. Preserve the current distribution and source worktree before any repair, uninstall, conversion, or relocation. Prefer `wsl --export` for a recoverable backup when the distro starts.

### Minimal diagnostic bundle

Run in PowerShell first. Use an elevated PowerShell only for `Get-WindowsOptionalFeature`; do not treat an access-denied message as a feature failure.

```powershell
wsl --version
wsl --status
wsl -l -v
bcdedit /enum "{current}" | Select-String hypervisorlaunchtype
Get-WindowsOptionalFeature -Online |
  Where-Object FeatureName -match 'Microsoft-Windows-Subsystem-Linux|VirtualMachinePlatform|Hyper-V' |
  Select-Object FeatureName, State
```

For a VHD or import error, collect this additional bundle before attempting repair:

```powershell
Get-Service vds, vmcompute, WslService, LxssManager -ErrorAction SilentlyContinue |
  Select-Object Name, Status, StartType
sc.exe query vhdmp
Get-Volume -DriveLetter D | Select-Object DriveLetter, FileSystem, DriveType, HealthStatus, SizeRemaining, Size
```

Do not create, attach, detach, or delete VHD files as a routine check. A DiskPart probe is only justified when the error already names a virtual-disk provider and the user has approved the diagnostic operation.

## Decision rules

- `wsl --install -d Ubuntu-22.04 --location <directory> --no-launch` is preferred over manual rootfs import when a distribution is not yet installed. It uses the supported WSL installer and allows a chosen storage directory.
- A failed `wsl --import` with `Wsl/Service/RegisterDistro/0xc03a0014` plus a failed DiskPart `create vdisk` indicates a Windows virtual-disk-provider issue, not a malformed `.tar.gz` filename and not an openvela problem.
- `wsl --install ...` succeeding proves the core WSL distribution path works. It does not prove that a previous manual import failure was caused by VirtualBox.
- WSL2 requires the Windows hypervisor stack. Modern VirtualBox can coexist through Hyper-V/NEM but may behave differently or more slowly. Do not disable WSL features or set `hypervisorlaunchtype off` merely to help VirtualBox; that will prevent WSL2 from working until reversed and rebooted.
- Windows paths mounted at `/mnt/c` or `/mnt/d` are accessible from WSL but metadata-heavy builds are slower than the Linux filesystem. Use `/home/dev/openvela-workspace` for builds and keep the official Windows repository as the source-of-record unless the user chooses otherwise.

### Failure routing

| Observation | Meaning supported by evidence | Lowest-cost next action |
| --- | --- | --- |
| `wsl -l -v` lists no distribution | WSL runtime may be installed but no distro is registered | Use `wsl --list --online`, then the supported `wsl --install` flow. |
| Microsoft Store Ubuntu package exists but `wsl -l -v` is empty | An Appx package is not the same as an initialized WSL distribution | Install/register a distribution; do not assume it can be launched. |
| `0xc03a0014` during `wsl --import` | Registration cannot obtain a virtual-disk provider | Check VHD services/driver and reproduce only if needed; do not rename the rootfs archive repeatedly. |
| DiskPart VHD creation has the same provider error | The problem is below WSL import and independent of the rootfs archive | Stop troubleshooting openvela and use a supported WSL install path or repair Windows virtualization with explicit approval. |
| `wsl --install -d Ubuntu-22.04 --location ...` succeeds | A working WSL2 distribution is registered | Launch once, create the Linux user, then verify repo access and build tools. |
| VirtualBox no longer starts after WSL2 setup | Hyper-V/NEM coexistence requires a separate VirtualBox decision | Diagnose VirtualBox independently; do not break WSL2 by disabling the hypervisor without a chosen tradeoff. |

## Build-host workflow

1. Verify the distribution starts and reports Ubuntu 22.04.
2. Confirm WSL can read the intended repository and confirm its Git status before building.
3. If moving code into `/home/dev`, copy or synchronize deliberately and record direction of synchronization. Never assume the two worktrees mirror each other.
4. For first build failures, use the existing `openvela-quickstart` or `openvela-build` skill rather than changing WSL/VirtualBox settings.
5. For USB/ST-LINK, start with Windows-native CubeProgrammer and GDB Server. WSL USB passthrough is optional and not required for this project.

## Installation and verification sequence

When no usable distro exists, this is the proven local form. `<distro-directory>` must be a dedicated empty directory, not the parent directory containing several distributions.

```powershell
wsl --install -d Ubuntu-22.04 --location "<distro-directory>" --no-launch
wsl -d Ubuntu-22.04
```

On first launch, create the Linux username/password. Then verify:

```bash
cat /etc/os-release
git --version
repo --version
make --version
python3 --version
arm-none-eabi-gcc --version
ls -la /mnt/c/Users/Fu_sheng/Desktop/openvela
```

`--no-launch` means installation finishes without automatically opening the distribution. It does not disable the installed distro.

## Reporting

When troubleshooting, report the exact failure text, the scope it rules in/out, and the least invasive next validation. Before a reboot or system setting change, state what it changes, how to reverse it, and why it is necessary.
