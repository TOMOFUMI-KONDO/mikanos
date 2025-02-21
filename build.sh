#!/bin/bash

set -e

(\
. $HOME/osbook/devenv/buildenv.sh; \
cd ./kernel; \
clang++ $CPPFLAGS -O2 -Wall -g --target=x86_64-elf -ffreestanding -mno-red-zone -fno-exceptions -fno-rtti -std=c++17 -c main.cpp; \
ld.lld $LDFLAGS --entry KernelMain -z norelro --image-base 0x100000 --static -o kernel.elf main.o \
)

DISK_IMG="$HOME/mikanos/mikanos.img"
MOUNT_POINT=./mnt
EFI_FILE="$HOME/edk2/Build/MikanLoaderX64/DEBUG_CLANG38/X64/Loader.efi"
KERNEL_FILE="$HOME/mikanos/kernel/kernel.elf"

(cd $HOME/edk2 && . ./edksetup.sh && build)
${HOME}/osbook/devenv/make_image.sh "$DISK_IMG" "$MOUNT_POINT" "$EFI_FILE" "$KERNEL_FILE"
