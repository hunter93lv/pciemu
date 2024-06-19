#ifndef PCIEMU_MEMBAR_H
#define PCIEMU_MEMBAR_H

#include "pciemu/front_end/pciemu.h"

typedef struct PCIEMUDevice PCIEMUDevice;

void pciemu_membar_reset(PCIEMUDevice *dev);

void pciemu_membar_init(PCIEMUDevice *dev, Error **errp);

void pciemu_membar_fini(PCIEMUDevice *dev);

extern const MemoryRegionOps pciemu_membar_ops;

#endif /* PCIEMU_MEMBAR_H */
