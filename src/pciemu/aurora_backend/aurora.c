#include "qemu/osdep.h"
#include "qemu/log.h"
#include "pciemu/front_end/pciemu.h"
#include "pciemu/front_end/irq.h"
#include "pciemu/aurora_backend/common/tcp_client.h"
#include "pciemu/aurora_backend/dma/dma.h"
#include "aurora.h"

static inline bool aurora_regbar_valid_access(uint64_t addr)
{
    return (AURORA_HW_REGBAR_START <= addr && addr <= AURORA_HW_REGBAR_END);
}

int aurora_write_devmem_by_addr(AURORADevice *adev, uint64_t addr, uint64_t value)
{
    return tcp_send_mem_write_req(addr, value);
}

int aurora_read_devmem_by_addr(AURORADevice *adev, uint64_t addr, uint64_t *value)
{
    return tcp_send_mem_read_req(addr, value);
}

int aurora_write_reg_by_addr(AURORADevice *adev, uint64_t addr, uint64_t value)
{

    if (!aurora_regbar_valid_access(addr))
        return -1;

    switch (addr) {
    case AURORA_HW_REG_0:
        adev->regs[0] = value;
        break;
    case AURORA_HW_REG_1:
        adev->regs[1] = value;
        break;
    case AURORA_HW_REG_2:
        adev->regs[2] = value;
        break;
    case AURORA_HW_REG_3:
        adev->regs[3] = value;
        break;
    case AURORA_HW_REG_IRQ_0_RAISE:
        pciemu_irq_raise(get_pciemu_dev(adev), 0);
        break;
    case AURORA_HW_REG_IRQ_0_LOWER:
        pciemu_irq_lower(get_pciemu_dev(adev), 0);
        break;
    case AURORA_HW_REG_DMA_CFG_TXDESC_SRC...AURORA_HW_REG_DMA_DOORBELL_RING:
        aurora_dma_reg_write(adev, addr, value);
        break;
    }

    return 0;
}

int aurora_read_reg_by_addr(AURORADevice *adev, uint64_t addr, uint64_t *value)
{

    if (!value)
        return -1;

    if (!aurora_regbar_valid_access(addr)) {
        *value = 0xdeadbeef;
        return -1;
    }

    switch (addr) {
    case AURORA_HW_REG_0:
        *value = adev->regs[0];
        break;
    case AURORA_HW_REG_1:
        *value  = adev->regs[1];
        break;
    case AURORA_HW_REG_2:
        *value  = adev->regs[2];
        break;
    case AURORA_HW_REG_3:
        *value  = adev->regs[3];
        break;
    }

    return 0;
}

void aurora_backend_reset(AURORADevice *adev)
{
    aurora_dma_reset(adev);
}

void aurora_backend_init(PCIEMUDevice *dev)
{
    AURORADevice *adev = (AURORADevice *)malloc(sizeof(AURORADevice));
    if (!adev) {
        qemu_log_mask(LOG_GUEST_ERROR, "alloc aurora dev failed!!!\n");
        return;
    }

    dev->backend = (void *)adev;
    adev->parent = dev;
    aurora_dma_init(adev);
}

void aurora_backend_fini(AURORADevice *adev)
{
    aurora_dma_fini(adev);
    adev->parent->backend = NULL;
    free(adev);
}
