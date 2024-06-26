#!/bin/bash
# file: start_emu.sh
# desc: 
# 	Use the precompiled qemu executable file
#       to start a x86 VM(ubuntu1804) containing a simulated PCIE device.
# features:
#	a.Find a free port and save it in the config file.
#	  You can use this port to ssh the guest OS later.
#	b.If the VM image does not exist, pull the image automatically.
#	c.Create backup files of VM images Automatically to keep the Guest OS env clean


REPOSITORY_DIR=$(git rev-parse --show-toplevel)
REPOSITORY_NAME=$(basename $REPOSITORY_DIR)
QEMU_DRIVE_FILE=$REPOSITORY_DIR/release/images/ubuntu1804.qcow2
QEMU_EXEC=$REPOSITORY_DIR/qemu/build/qemu-system-x86_64
QEMU_DRIVE_FILE_BAK=$REPOSITORY_DIR/release/images/ubuntu1804.qcow2.bak
ORIGINAL_IMAGE=$REPOSITORY_DIR/release/images/ubuntu1804.image.tar.gz
QEMU_TCP_ADDRESS=127.0.0.1
QEMU_TCP_PORT_SSH=1111
QEMU_TCP_PORT_SSH_TMP_FILE=$REPOSITORY_DIR/release/config
QEMU_ACCEL=kvm
QEMU_CORE_CNT=4
QEMU_MEM_SIZE=8G
GUEST_OS_USER=emu
GUEST_OS_PASSWORD=emu
#
# Well-known ports: 0 to 1023
# Registered ports: 1024 to 49151
# Dynamic and/or private ports: 49152 to 65535
#
VALID_PORT_START=1024
VALID_PORT_END=65535
SERVER_IP=127.0.0.1
SERVER_PORT=4444
ORIGINAL_IMAGE_PATH=root@39.103.98.210:/root/ubuntu1804.image.tar.gz

source $REPOSITORY_DIR/utils/log.sh

function is_port_in_use() {
   tcp=`netstat -an | grep ":$1 " | awk '$1 == "tcp" && $NF == "LISTEN" {print $0}' | wc -l`
   udp=`netstat -an | grep ":$1 " | awk '$1 == "udp" && $NF == "0.0.0.0:*" {print $0}' | wc -l`
   (( cnt = tcp + udp ))
   if [ $cnt == 0 ]; then
       echo "0"
   else
       echo "1"
   fi
}

function random_range() {
   shuf -i $1-$2 -n1
}

function get_random_port() {
   rand_port=0
   while true; do
       rand_port=`random_range $1 $2`
       if [ `is_port_in_use $rand_port` == 0 ] ; then
              echo $rand_port
	      exit
       fi
   done
}


if [ -z "$1" ] || [ -z "$2" ]; then
  log $LOG_LEVEL_WARN "Usage: $0 <server_ip> <server_port>"
  log $LOG_LEVEL_WARN "No emu server ip & port parameters detected."
  log $LOG_LEVEL_WARN "Will use default cfg:ip $SERVER_IP,port $SERVER_PORT."
else
  SERVER_IP=$1
  SERVER_PORT=$2
fi

rm -rf $QEMU_TCP_PORT_SSH_TMP_FILE
QEMU_TCP_PORT_SSH=$(get_random_port $VALID_PORT_START $VALID_PORT_END)
echo ${QEMU_TCP_PORT_SSH} > ${QEMU_TCP_PORT_SSH_TMP_FILE}

sudo chmod 666 /dev/kvm

if [ ! -f "$ORIGINAL_IMAGE" ]; then
   log $LOG_LEVEL_INFO "Copying images from remote......"
   scp $ORIGINAL_IMAGE_PATH $ORIGINAL_IMAGE
fi

if [ ! -f "$ORIGINAL_IMAGE" ]; then
   log $LOG_LEVEL_ERROR "Copy remote image failed,exit!!!"
   exit 1
fi

if [ ! -f "$QEMU_DRIVE_FILE_BAK" ]; then
   log $LOG_LEVEL_INFO "Unzipping the image file......"
   tar -xzvf $ORIGINAL_IMAGE  -C  $(dirname "$QEMU_DRIVE_FILE_BAK")
fi

if [ ! -f "$QEMU_DRIVE_FILE_BAK" ]; then
   log $LOG_LEVEL_ERROR "No qcow2 images found,exit!!!"
   exit 1
fi

cp $QEMU_DRIVE_FILE_BAK $QEMU_DRIVE_FILE

export EMU_SERVER_IP=$SERVER_IP
export EMU_SERVER_PORT=$SERVER_PORT

 $QEMU_EXEC\
  -accel $QEMU_ACCEL\
  -cpu host\
  -smp $QEMU_CORE_CNT\
  -m $QEMU_MEM_SIZE\
  -display none\
  -device virtio-net,netdev=vmnic\
  -netdev user,id=vmnic,hostfwd=tcp:$QEMU_TCP_ADDRESS:$QEMU_TCP_PORT_SSH-:22\
  -device pciemu,id=pciemu1\
  -drive file=$QEMU_DRIVE_FILE,if=virtio\
  -virtfs local,path=$HOME,security_model=mapped,mount_tag=hostdir\
  -fw_cfg name=opt/repositoryname,string=$REPOSITORY_NAME &

log $LOG_LEVEL_INFO "EMU start success."
log $LOG_LEVEL_INFO "ssh info:ssh -p $QEMU_TCP_PORT_SSH $GUEST_OS_USER@$QEMU_TCP_ADDRESS"
log $LOG_LEVEL_INFO "password:$GUEST_OS_PASSWORD"
log $LOG_LEVEL_INFO "You can also exec ./ssh_emu.sh"
