#ifndef DEVICE_MEMORY_H
#define DEVICE_MEMORY_H

#include "qemu/osdep.h"
#include "hw/pci/pci.h"
#include "device_memory.h"


int write_devmem_by_addr(uint64_t addr, uint64_t value);

int read_devmem_by_addr(uint64_t addr, uint64_t *value);

#endif /* DEVICE_MEMORY_H */
