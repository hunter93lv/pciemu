#ifndef AURORA_H
#define AURORA_H

#include "pciemu/front_end/pciemu.h"
#include "aurora_hw.h"
#include "pciemu/aurora_backend/dma/dma.h"

typedef struct AURORADevice {
    PCIEMUDevice *parent;
    DMAEngine dma;
    uint64_t regs[AURORA_HW_REGBAR_REG_CNT];
} AURORADevice;

static inline AURORADevice *get_aurora_dev(PCIEMUDevice *dev)
{
    if (!dev)
        return NULL;

    return (AURORADevice *)(dev->backend);
}

static inline PCIEMUDevice *get_pciemu_dev(AURORADevice *adev)
{
    if (!adev)
        return NULL;

    return adev->parent;
}


int aurora_write_devmem_by_addr(AURORADevice *adev, uint64_t addr, uint64_t value);

int aurora_read_devmem_by_addr(AURORADevice *adev, uint64_t addr, uint64_t *value);

int aurora_write_reg_by_addr(AURORADevice *adev, uint64_t addr, uint64_t value);

int aurora_read_reg_by_addr(AURORADevice *adev, uint64_t addr, uint64_t *value);

void aurora_backend_reset(AURORADevice *adev);

void aurora_backend_init(PCIEMUDevice *dev);

void aurora_backend_fini(AURORADevice *adev);

#endif /* AURORA_H */
