#ifndef PCIEMU_INTERNAL_BAR_H
#define PCIEMU_INTERNAL_BAR_H

#include "pciemu/front_end/pciemu.h"

typedef struct PCIEMUDevice PCIEMUDevice;

void pciemu_internal_bar_reset(PCIEMUDevice *dev);

void pciemu_internal_bar_init(PCIEMUDevice *dev, Error **errp);

void pciemu_internal_bar_fini(PCIEMUDevice *dev);

extern const MemoryRegionOps pciemu_internal_bar_ops;

#endif /* PCIEMU_INTERNAL_BAR_H */
