#!/bin/bash

(cd /home/ubuntu/edk2 && . ./edksetup.sh && build) || exit 1
${HOME}/dev/osbook/devenv/make_image.sh $HOME/disk.img ./mnt $HOME/edk2/Build/MikanLoaderX64/DEBUG_CLANG38/X64/Loader.efi
