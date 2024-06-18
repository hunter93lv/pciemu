#ifndef AURORA_DMA_H
#define AURORA_DMA_H

#include "qemu/osdep.h"
#include "hw/pci/pci.h"
#include "pciemu/aurora_backend/aurora_hw.h"

#define DMA_BIT_MASK(n) (((n) == 64) ? ~0ULL : ((1ULL << (n)) - 1))

/* forward declaration (defined in pciemu.h) to avoid circular reference */
typedef struct PCIEMUDevice PCIEMUDevice;

/* dma command */
typedef uint64_t dma_cmd_t;

/* dma size */
typedef dma_addr_t dma_size_t;

/* dma mask */
typedef uint64_t dma_mask_t;

/* transfer descriptor */
typedef struct DMATransferDesc {
    dma_addr_t src;
    dma_addr_t dst;
    dma_size_t len;
} DMATransferDesc;

/* configuration of the DMA engine pre-execution */
typedef struct DMAConfig {
    DMATransferDesc txdesc;
    dma_cmd_t cmd;
    dma_mask_t mask;
} DMAConfig;

/* status of the DMA engine */
typedef enum DMAStatus {
    DMA_STATUS_IDLE,
    DMA_STATUS_EXECUTING,
    DMA_STATUS_OFF,
} DMAStatus;

typedef struct DMAEngine {
    DMAConfig config;
    DMAStatus status;
    uint8_t buff[AURORA_HW_DMA_AREA_SIZE];
} DMAEngine;


int aurora_dma_reg_write(PCIEMUDevice *dev, uint64_t addr, uint64_t value);

void aurora_dma_reset(PCIEMUDevice *dev);

void aurora_dma_init(PCIEMUDevice *dev);

void aurora_dma_fini(PCIEMUDevice *dev);

#endif /* AURORA_DMA_H */
