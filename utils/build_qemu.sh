#!/bin/bash
# file: build_qemu.sh
# desc:
#       Prepare qemu build env & build qemu exectuable
# features:
#       a.Detect qemu build dependency & install dependency libs
#       b.Compile our pcie emulator code into qemu but do not invade the code structure of qemu itself
#       c.Compile qemu exectuable & cleanup qemu repo


REPOSITORY_DIR=$(git rev-parse --show-toplevel)
REPOSITORY_NAME=$(basename $REPOSITORY_DIR)

source $REPOSITORY_DIR/utils/log.sh

sudo apt update
sudo apt install -y git libglib2.0-dev libfdt-dev libpixman-1-dev zlib1g-dev \
                    ninja-build netcat libssh-dev libvde-dev libvdeplug-dev \
                    libcap-ng-dev libattr1-dev libslirp-dev

log $LOG_LEVEL_INFO "Qemu build dependency libs check & install done!"

git submodule update --init

# Edit original build files
cp $REPOSITORY_DIR/qemu/hw/misc/Kconfig $REPOSITORY_DIR/qemu/hw/misc/Kconfig.bak
cp $REPOSITORY_DIR/qemu/hw/misc/meson.build $REPOSITORY_DIR/qemu/hw/misc/meson.build.bak

echo "source $REPOSITORY_NAME/Kconfig" >> $REPOSITORY_DIR/qemu/hw/misc/Kconfig
echo "subdir('$REPOSITORY_NAME')" >> $REPOSITORY_DIR/qemu/hw/misc/meson.build

# Create symbolic links to device files
ln -s $REPOSITORY_DIR/src/hw/$REPOSITORY_NAME/ $REPOSITORY_DIR/qemu/hw/misc/

# Create symbolic link to the pciemu_hw.h include file
# This will avoid changing the meson files to be able to find this include
ln -s $REPOSITORY_DIR/include/hw/pciemu_hw.h $REPOSITORY_DIR/src/hw/$REPOSITORY_NAME/pciemu_hw.h

# Configure QEMU
cd $REPOSITORY_DIR/qemu
log $LOG_LEVEL_INFO "Configuring qemu..."
./configure \
	--disable-bsd-user --disable-guest-agent --disable-gtk --disable-werror \
	--enable-curses --enable-slirp --enable-libssh --enable-vde --enable-virtfs \
	--target-list=x86_64-softmmu

log $LOG_LEVEL_INFO "Configuring qemu done."
log $LOG_LEVEL_INFO "Building qemu...."
make -j128
log $LOG_LEVEL_INFO "Build qemu done."
cd $REPOSITORY_DIR
mv $REPOSITORY_DIR/qemu/hw/misc/Kconfig.bak qemu/hw/misc/Kconfig
mv $REPOSITORY_DIR/qemu/hw/misc/meson.build.bak qemu/hw/misc/meson.build
rm -rf $REPOSITORY_DIR/qemu/hw/misc/$REPOSITORY_NAME
rm -rf $REPOSITORY_DIR/src/hw/$REPOSITORY_NAME/pciemu_hw.h

