#!/bin/bash
# file: ssh_emu.sh
# desc:
#       ssh to guest OS
# features:
#       a.Read ssh port from config file
#	b.Guest OS user:emu
#	c.Guest OS pswd:emu

REPOSITORY_DIR=$(git rev-parse --show-toplevel)
QEMU_TCP_ADDRESS=127.0.0.1
QEMU_TCP_PORT_SSH=1111
QEMU_TCP_PORT_SSH_TMP_FILE=$REPOSITORY_DIR/release/config
GUEST_OS_USER=emu
GUEST_OS_PASSWORD=emu

source $REPOSITORY_DIR/utils/log.sh

if [ ! -f "$QEMU_TCP_PORT_SSH_TMP_FILE" ]; then
    log $LOG_LEVEL_ERROR "No ssh port config exist, link failed!!"
    exit 1
else
   QEMU_TCP_PORT_SSH=$(cat $QEMU_TCP_PORT_SSH_TMP_FILE)
fi

ssh -p $QEMU_TCP_PORT_SSH $GUEST_OS_USER@$QEMU_TCP_ADDRESS
