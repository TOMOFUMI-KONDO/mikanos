#!/bin/bash

(cd /home/ubuntu/edk2 && . ./edksetup.sh && build)
${HOME}/dev/osbook/devenv/make_image.sh ./disk.img ./mnt Loader.efi
