#!/bin/bash

set -e

(cd ./kernel; make kernel.elf)

DISK_IMG="$HOME/mikanos/mikanos.img"
MOUNT_POINT=./mnt
EFI_FILE="$HOME/edk2/Build/MikanLoaderX64/DEBUG_CLANG38/X64/Loader.efi"
KERNEL_FILE="$HOME/mikanos/kernel/kernel.elf"

(cd $HOME/edk2 && . ./edksetup.sh && build)
${HOME}/osbook/devenv/make_image.sh "$DISK_IMG" "$MOUNT_POINT" "$EFI_FILE" "$KERNEL_FILE"
