[CmdletBinding()]
param(
  [Parameter(Mandatory = $true)]
  [ValidateScript({ Test-Path -LiteralPath $_ -PathType Leaf })]
  [string]$ElfPath,

  [string]$StlinkSerial = '57FF6D067180555711252487',

  [string]$CubeProgrammerBin = 'D:\embeddedTool\STM32Dev\STM32CubeProgrammer\bin',

  [string]$CubeIdeRoot = 'D:\embeddedTool\STM32Dev\STM32CubeIDE_1.17.0\STM32CubeIDE\plugins'
)

$ErrorActionPreference = 'Stop'

# This is a volatile SRAM debug loader. It does not erase or program NOR.
$gdbServer = Join-Path $CubeIdeRoot 'com.st.stm32cube.ide.mcu.externaltools.stlink-gdb-server.win32_2.2.0.202409170845\tools\bin\ST-LINK_gdbserver.exe'
$gdb = Join-Path $CubeIdeRoot 'com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.12.3.rel1.win32_1.1.0.202410251130\tools\bin\arm-none-eabi-gdb.exe'
$gdbCommands = Join-Path $PSScriptRoot 'run_sram_image.gdb'
$elf = (Get-Item -LiteralPath $ElfPath).FullName
$logDir = Join-Path ([System.IO.Path]::GetTempPath()) 'dnn647-sram-gdb'
$serverOut = Join-Path $logDir 'gdbserver.out.log'
$serverErr = Join-Path $logDir 'gdbserver.err.log'

foreach ($path in @($CubeProgrammerBin, $gdbServer, $gdb, $gdbCommands))
{
  if (-not (Test-Path -LiteralPath $path))
  {
    throw "Required file is missing: $path"
  }
}

$existingListener = Get-NetTCPConnection -LocalPort 55000 -ErrorAction SilentlyContinue |
  Select-Object -First 1

if ($existingListener)
{
  $existingProcess = Get-Process -Id $existingListener.OwningProcess -ErrorAction SilentlyContinue

  if ($existingProcess -and $existingProcess.ProcessName -eq 'ST-LINK_gdbserver')
  {
    Stop-Process -Id $existingProcess.Id
    Start-Sleep -Seconds 1
  }
  else
  {
    throw 'TCP port 55000 is in use by another process. Close it before running this script.'
  }
}

New-Item -ItemType Directory -Force -Path $logDir | Out-Null

$serverArgs = @(
  '-e', '-p', '55000', '-z', '55001', '-d',
  '-i', $StlinkSerial, '-m', '1', '-cp', $CubeProgrammerBin
)

$server = Start-Process -FilePath $gdbServer -ArgumentList $serverArgs `
  -WindowStyle Hidden -RedirectStandardOutput $serverOut `
  -RedirectStandardError $serverErr -PassThru

try
{
  Start-Sleep -Seconds 3

  if (-not (Get-NetTCPConnection -LocalPort 55000 -ErrorAction SilentlyContinue))
  {
    Get-Content $serverOut, $serverErr -ErrorAction SilentlyContinue
    throw 'ST-LINK GDB Server did not start.'
  }

  Write-Host "Loading volatile SRAM image: $elf"
  Write-Host 'Target requirements: BOOT0=GND, BOOT1=3.3V, then power-cycle before loading.'
  & $gdb --quiet -x $gdbCommands $elf
}
finally
{
  if (-not $server.HasExited)
  {
    Stop-Process -Id $server.Id -ErrorAction SilentlyContinue
  }
}
