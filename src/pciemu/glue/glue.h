#ifndef GLUE_H
#define GLUE_H

#include "pciemu/front_end/pciemu.h"
#include "pciemu_hw.h"

int glue_write_devmem_by_addr(PCIEMUDevice *dev, uint64_t addr, uint64_t value);

int glue_read_devmem_by_addr(PCIEMUDevice *dev, uint64_t addr, uint64_t *value);

int glue_write_reg_by_addr(PCIEMUDevice *dev, uint64_t addr, uint64_t value);

int glue_read_reg_by_addr(PCIEMUDevice *dev, uint64_t addr, uint64_t *value);

void glue_backend_reset(PCIEMUDevice *dev);

void glue_backend_init(PCIEMUDevice *dev);

void glue_backend_fini(PCIEMUDevice *dev);

#endif /* GLUE_H */
