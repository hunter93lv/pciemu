#ifndef PCIEMU_IRQ_H
#define PCIEMU_IRQ_H

#include "qemu/osdep.h"
#include "hw/pci/pci.h"

#define PCIEMU_IRQ_MAX_VECTORS_MSI 32
#define PCIEMU_IRQ_MAX_VECTORS_MSIX 2048


typedef struct PCIEMUDevice PCIEMUDevice;

typedef struct MSIVector {
    PCIEMUDevice *dev;
    bool raised;
} MSIVector;

typedef struct IRQStatusMSI {
    MSIVector msi_vectors[PCIEMU_IRQ_MAX_VECTORS_MSI];
} IRQStatusMSI;

typedef struct IRQStatusMSIX {
    MSIVector msix_vectors[PCIEMU_IRQ_MAX_VECTORS_MSIX];
} IRQStatusMSIX;


typedef struct IRQStatusPin {
    bool raised;
} IRQStatusPin;

typedef struct IRQStatus {
    union {
        IRQStatusMSIX msix;
        IRQStatusMSI msi;
        IRQStatusPin pin;
    } status;
} IRQStatus;

void pciemu_irq_raise(PCIEMUDevice *dev, unsigned int vector);

void pciemu_irq_lower(PCIEMUDevice *dev, unsigned int vector);

void pciemu_irq_reset(PCIEMUDevice *dev);

void pciemu_irq_init(PCIEMUDevice *dev, Error **errp);

void pciemu_irq_fini(PCIEMUDevice *dev);

#endif /* PCIEMU_IRQ_H */
