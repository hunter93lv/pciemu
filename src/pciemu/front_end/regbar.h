#ifndef PCIEMU_REGBAR_H
#define PCIEMU_REGBAR_H

#include "pciemu/front_end/pciemu.h"

typedef struct PCIEMUDevice PCIEMUDevice;

void pciemu_regbar_reset(PCIEMUDevice *dev);

void pciemu_regbar_init(PCIEMUDevice *dev, Error **errp);

void pciemu_regbar_fini(PCIEMUDevice *dev);

extern const MemoryRegionOps pciemu_regbar_ops;

#endif /* PCIEMU_REGBAR_H */
