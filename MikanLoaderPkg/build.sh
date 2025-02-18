#!/bin/bash

(cd /home/ubuntu/edk2 && . ./edksetup.sh && build) || exit 1
${HOME}/dev/osbook/devenv/make_image.sh ./disk.img ./mnt Loader.efi
