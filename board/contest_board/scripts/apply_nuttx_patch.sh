#!/usr/bin/env bash
# Apply the STM32N6 chip-layer patch carried by this contest repository.

set -euo pipefail

script_dir=$(cd "$(dirname "$0")" && pwd)
contest_root=$(cd "$script_dir/../../.." && pwd)
workspace=$(cd "$contest_root/.." && pwd)
nuttx_dir="$workspace/nuttx"
patch_file="$script_dir/../patches/nuttx-stm32n6.patch"

if [[ ! -d "$nuttx_dir/.git" ]]; then
  echo "NuttX checkout not found: $nuttx_dir" >&2
  exit 1
fi

if git -C "$nuttx_dir" apply --reverse --check "$patch_file" 2>/dev/null; then
  echo "STM32N6 patch is already applied."
elif git -C "$nuttx_dir" apply --check "$patch_file"; then
  git -C "$nuttx_dir" apply "$patch_file"
  echo "Applied STM32N6 patch to $nuttx_dir"
else
  echo "Patch does not match this NuttX revision." >&2
  exit 1
fi
