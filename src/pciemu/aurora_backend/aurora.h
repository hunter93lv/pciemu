#ifndef AURORA_H
#define AURORA_H

#include "pciemu/front_end/pciemu.h"
#include "aurora_hw.h"
#include "pciemu/aurora_backend/dma/dma.h"

typedef struct AURORADevice {
    PCIEMUDevice *parent;
    DMAEngine dma;
    uint64_t regs[AURORA_HW_BAR0_REG_CNT];
} AURORADevice;


int aurora_write_devmem_by_addr(PCIEMUDevice *dev, uint64_t addr, uint64_t value);

int aurora_read_devmem_by_addr(PCIEMUDevice *dev, uint64_t addr, uint64_t *value);

int aurora_write_reg_by_addr(PCIEMUDevice *dev, uint64_t addr, uint64_t value);

int aurora_read_reg_by_addr(PCIEMUDevice *dev, uint64_t addr, uint64_t *value);

void aurora_backend_reset(PCIEMUDevice *dev);

void aurora_backend_init(PCIEMUDevice *dev);

void aurora_backend_fini(PCIEMUDevice *dev);

#endif /* AURORA_H */
