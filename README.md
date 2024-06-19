<div align="center">

<h1>pciemu</h1>

![License](https://img.shields.io/github/license/luizinhosuraty/pciemu)
![Build](https://img.shields.io/github/actions/workflow/status/luizinhosuraty/pciemu/linux-ci.yml?label=tests)
 
</div>

pciemu provides a PCIe Device Emulation in QEMU.

## Setup and Compiling QEMU

The [build script](utils/build_qemu.sh) will prepare qemu build env & build qemu exectuable.
it will :
- detect qemu build dependency & install dependency libs.
- compile our pcie emulator code into qemu but do not invade the code structure of qemu itself.
- Compile qemu exectuable & cleanup qemu repo.

you can run the following cmd:

```bash
$ ./utils/build_qemu.sh
```

## Running

Once all dependencies are installed and QEMU is properly compiled, all you have
to do is run QEMU:

```bash
$ ./release/start_emu.sh
```

This cmd will launch a Virtual Machine (VM) with precompiled qemu executable in the background with
the correct set of arguments. (see [start_emu.sh](release/start_emu.sh))

- The VM containing a simulated PCIE device
- Script will Find a free port and save it in the config file.You can use this port to ssh the guest OS later.
- If the VM image does not exist, this script will pull the image automatically from remote.
- Create backup files of VM images automatically to keep the Guest OS env clean


Finally, to login into the VM, use the cmd:

```bash
$ ./release/ssh_emu.sh
```
- Guest OS user:emu
- password:emu

### Inside the VM
- The host user's home directory will be automatically mounted to the guest OS's emu user's home directory
- If you do not have permission to mounted the path, you can add it with the following command:
```bash
$ sudo chown emu /home/emu -R
$ sudo chgrp emu /home/emu -R
```
This does not affect the properties of the file on the host, only on the guest.

Later, it's all standard kernel module compilation and insertion into the kernel:

```bash
$ cd /your_pciemu_path/driver/kernel/
$ make
$ sudo insmod pciemu.ko
```

Finally, the userspace program:

```bash
$ cd /your_pciemu_path/driver/userspace/
$ make
$ ./pciemu_example -h
```
