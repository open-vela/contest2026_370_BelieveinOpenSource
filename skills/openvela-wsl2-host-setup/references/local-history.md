# Local WSL2 History and Reference Case

This is a machine-specific record, not general Windows guidance. Other machines should use its decision path, not blindly replay its system changes.

## Host snapshot when investigated

```text
Windows: 10.0.22631.5768 (Windows 11 23H2 family)
WSL: 2.7.12.0
WSL kernel: 6.18.33.2-2
Default WSL version: 2
```

At the time, `VirtualMachinePlatform` and `Microsoft-Windows-Subsystem-Linux` were enabled, `hypervisorlaunchtype` was `Auto`, and `systeminfo` reported that a hypervisor was detected. `WslService` and `vmcompute` were running. This was consistent with a WSL2-capable host.

## Confirmed successful state

On 2026-09, the supported installer succeeded:

```powershell
wsl --install -d Ubuntu-22.04 --location "D:\WSL\distros\Ubuntu_22_04" --no-launch
```

The installed distribution reported:

```text
Ubuntu 22.04.5 LTS
WSL version 2
```

After first launch, it was accessible as:

```text
Windows: C:\Users\Fu_sheng\Desktop\openvela
WSL mount: /mnt/c/Users/Fu_sheng/Desktop/openvela
```

The WSL distribution is stored under `D:\WSL\distros\Ubuntu_22_04`. Installation with `--location` created the dedicated directory; it did not require the parent directory to be pre-created.

## Previous failure

Manual import of a Jammy rootfs failed:

```text
Wsl/Service/RegisterDistro/0xc03a0014
找不到指定文件的虚拟磁盘支持提供程序。
```

DiskPart also failed to create a probe VHDX with the same virtual-disk-provider message. This ruled out the rootfs archive name and target `D:` path as the immediate cause.

`vhdmp` was initially stopped and was successfully started, but that alone did not repair the manual import path. `DISM /ScanHealth` and `sfc /verifyonly` found no component-store or protected-file corruption.

The downloaded rootfs archive existed and was about 234 MB. Its extension `.rootfs.tar.gz` was valid; renaming it would not repair a virtual-disk-provider failure.

## VirtualBox conclusion

VirtualBox configuration had previously required disabling the Windows hypervisor stack. This can conflict with WSL2 requirements. However, no collected evidence established VirtualBox itself as the cause of `0xc03a0014`; do not state that conclusion as fact.

The successful supported WSL install means current WSL2 is operational. Avoid changing Hyper-V-related settings while it works unless a new, independently observed problem requires it.

## Research references consulted

These links were used to understand possible Windows/VirtualBox/WSL interactions. They are not substitutes for inspecting the host state, and they may become outdated.

- Microsoft Q&A, WSL-related question: <https://learn.microsoft.com/zh-cn/answers/questions/3972780/question-3972780>
- Juejin article: <https://juejin.cn/post/7476497261574684687>
- SkyXZ blog post: <https://www.cnblogs.com/SkyXZ/p/18675628>

## Reusable lessons

1. A Store-installed Ubuntu package is not proof that a WSL distribution has been registered; `wsl -l -v` is the authority.
2. A `vhdmp` service/driver being stopped can be relevant, but successfully starting it is not proof that all virtual-disk providers are healthy.
3. When WSL import and DiskPart VHD creation fail with the same provider error, stop changing archive names, paths, and project files; the defect is below those layers.
4. `wsl --install -d Ubuntu-22.04 --location ... --no-launch` is simpler and proved successful here. It is the preferred first install path on a current WSL release.
5. WSL2 and VirtualBox share virtualization infrastructure. Coexistence is a configuration/performance tradeoff, not proof that one product caused every failure in the other.
