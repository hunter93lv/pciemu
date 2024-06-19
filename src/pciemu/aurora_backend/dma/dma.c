#include "qemu/osdep.h"
#include "qemu/log.h"
#include "pciemu/front_end/pciemu.h"
#include "pciemu/front_end/irq.h"
#include "dma.h"
#include "pciemu/aurora_backend/aurora.h"

/* -----------------------------------------------------------------------------
 *  Private
 * -----------------------------------------------------------------------------
 */

static inline bool aurora_dma_valid_access(uint64_t addr)
{
    return (AURORA_HW_REG_DMA_CFG_TXDESC_SRC <= addr && addr <= AURORA_HW_REG_DMA_DOORBELL_RING);
}

static inline dma_addr_t aurora_dma_addr_mask(PCIEMUDevice *dev,
                                              dma_addr_t addr)
{
    AURORADevice *adev = get_aurora_dev(dev);
    dma_addr_t masked = addr & adev->dma.config.mask;
    if (masked != addr) {
        qemu_log_mask(LOG_GUEST_ERROR,
                      "masked (%" PRIx64 ") != addr (%" PRIx64 ") \n", masked,
                      addr);
    }
    return masked;
}

static inline bool aurora_dma_inside_device_boundaries(dma_addr_t addr)
{
    return (AURORA_HW_DMA_AREA_START <= addr &&
            addr <= AURORA_HW_DMA_AREA_START + AURORA_HW_DMA_AREA_SIZE);
}

static void aurora_dma_execute(PCIEMUDevice *dev)
{
    AURORADevice *adev = get_aurora_dev(dev);
    DMAEngine *dma = &adev->dma;
    if (dma->config.cmd != AURORA_HW_DMA_DIRECTION_TO_DEVICE &&
        dma->config.cmd != AURORA_HW_DMA_DIRECTION_FROM_DEVICE)
        return;
    if (dma->config.cmd == AURORA_HW_DMA_DIRECTION_TO_DEVICE) {
        /* DMA_DIRECTION_TO_DEVICE
         *   The transfer direction is RAM(or other device)->device.
         *   The content in the bus address dma->config.txdesc.src, which points
         *   to RAM memory (or other device memory), will be copied to address
         *   dst inside the device.
         *   dma->buff is the dedicated area inside the device to receive
         *   DMA transfers. Thus, dst is basically the offset of dma->buff.
         */
        if (!aurora_dma_inside_device_boundaries(dma->config.txdesc.dst)) {
            qemu_log_mask(LOG_GUEST_ERROR, "dst register out of bounds \n");
            return;
        }
        dma_addr_t src = aurora_dma_addr_mask(dev, dma->config.txdesc.src);
        dma_addr_t dst = dma->config.txdesc.dst - AURORA_HW_DMA_AREA_START;
        int err = pci_dma_read(&dev->pci_dev, src, dma->buff + dst,
                               dma->config.txdesc.len);
        if (err) {
            qemu_log_mask(LOG_GUEST_ERROR, "pci_dma_read err=%d\n", err);
        }
    } else {
        /* DMA_DIRECTION_FROM_DEVICE
         *   The transfer direction is device->RAM (or other device).
         *   This means that the content in the src address inside the device
         *   will be copied to the bus address dma->config.txdesc.dst, which
         *   points to a RAM memory (or other device memory).
         *   dma->buff is the dedicated area inside the device to receive
         *   DMA transfers. Thus, src is basically the offset of dma->buff.
         */
        if (!aurora_dma_inside_device_boundaries(dma->config.txdesc.src)) {
            qemu_log_mask(LOG_GUEST_ERROR, "src register out of bounds \n");
            return;
        }
        dma_addr_t src = dma->config.txdesc.src - AURORA_HW_DMA_AREA_START;
        dma_addr_t dst = aurora_dma_addr_mask(dev, dma->config.txdesc.dst);
        int err = pci_dma_write(&dev->pci_dev, dst, dma->buff + src,
                                dma->config.txdesc.len);
        if (err) {
            qemu_log_mask(LOG_GUEST_ERROR, "pci_dma_write err=%d\n", err);
        }
    }
    pciemu_irq_raise(dev, AURORA_HW_IRQ_DMA_ENDED_VECTOR);
}

static void aurora_dma_config_txdesc_src(PCIEMUDevice *dev, dma_addr_t src)
{
    AURORADevice *adev = get_aurora_dev(dev);
    DMAStatus status = qatomic_read(&adev->dma.status);
    if (status == DMA_STATUS_IDLE)
        adev->dma.config.txdesc.src = src;
}

static void aurora_dma_config_txdesc_dst(PCIEMUDevice *dev, dma_addr_t dst)
{
    AURORADevice *adev = get_aurora_dev(dev);
    DMAStatus status = qatomic_read(&adev->dma.status);
    if (status == DMA_STATUS_IDLE)
        adev->dma.config.txdesc.dst = dst;
}

static void aurora_dma_config_txdesc_len(PCIEMUDevice *dev, dma_size_t size)
{
    AURORADevice *adev = get_aurora_dev(dev);
    DMAStatus status = qatomic_read(&adev->dma.status);
    if (status == DMA_STATUS_IDLE)
        adev->dma.config.txdesc.len = size;
}

static void aurora_dma_config_cmd(PCIEMUDevice *dev, dma_cmd_t cmd)
{
    AURORADevice *adev = get_aurora_dev(dev);
    DMAStatus status = qatomic_read(&adev->dma.status);
    if (status == DMA_STATUS_IDLE)
        adev->dma.config.cmd = cmd;
}

static void aurora_dma_doorbell_ring(PCIEMUDevice *dev)
{
    /* atomic access of dma.status may not be neeeded as the MMIO access
     * will be normally serialized.
     * Though not really necessary, it can show that we need to think of
     * atomic accessing regions, especially if the device is a bit more
     * complex.
     */
    AURORADevice *adev = get_aurora_dev(dev);
    DMAStatus status = qatomic_cmpxchg(&adev->dma.status, DMA_STATUS_IDLE,
                                       DMA_STATUS_EXECUTING);
    if (status == DMA_STATUS_EXECUTING)
        return;
    aurora_dma_execute(dev);
    qatomic_set(&adev->dma.status, DMA_STATUS_IDLE);
}

/* -----------------------------------------------------------------------------
 *  Public
 * -----------------------------------------------------------------------------
 */
int aurora_dma_reg_write(PCIEMUDevice *dev, uint64_t addr, uint64_t value)
{
    if (!aurora_dma_valid_access(addr))
        return -1;

    switch (addr) {
    case AURORA_HW_REG_DMA_CFG_TXDESC_SRC:
        aurora_dma_config_txdesc_src(dev, value);
        break;
    case AURORA_HW_REG_DMA_CFG_TXDESC_DST:
        aurora_dma_config_txdesc_dst(dev, value);
        break;
    case AURORA_HW_REG_DMA_CFG_TXDESC_LEN:
        aurora_dma_config_txdesc_len(dev, value);
        break;
    case AURORA_HW_REG_DMA_CFG_CMD:
        aurora_dma_config_cmd(dev, value);
        break;
    case AURORA_HW_REG_DMA_DOORBELL_RING:
        aurora_dma_doorbell_ring(dev);
        break;
    }

    return 0;
}

void aurora_dma_reset(PCIEMUDevice *dev)
{
    AURORADevice *adev = get_aurora_dev(dev);
    DMAEngine *dma = &adev->dma;
    dma->status = DMA_STATUS_IDLE;
    dma->config.txdesc.src = 0;
    dma->config.txdesc.dst = 0;
    dma->config.txdesc.len = 0;
    dma->config.cmd = 0;

    /* clear the internal buffer */
    memset(dma->buff, 0, AURORA_HW_DMA_AREA_SIZE);
}

void aurora_dma_init(PCIEMUDevice *dev)
{
    AURORADevice *adev = get_aurora_dev(dev);
    /* Basically reset the DMA engine */
    aurora_dma_reset(dev);

    /* and set the DMA mask, which does not change */
    adev->dma.config.mask = DMA_BIT_MASK(AURORA_HW_DMA_ADDR_CAPABILITY);
}

void aurora_dma_fini(PCIEMUDevice *dev)
{
    AURORADevice *adev = get_aurora_dev(dev);
    aurora_dma_reset(dev);
    adev->dma.status = DMA_STATUS_OFF;
}
