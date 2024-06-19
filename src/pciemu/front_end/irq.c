#include "qemu/osdep.h"
#include "qemu/log.h"
#include "hw/pci/msi.h"
#include "hw/pci/msix.h"
#include "pciemu/front_end/pciemu.h"
#include "pciemu/glue/glue.h"
#include "irq.h"

/* -----------------------------------------------------------------------------
 *  Private
 * -----------------------------------------------------------------------------
 */

static inline void pciemu_irq_init_msix(PCIEMUDevice *dev, Error **errp)
{
    int i;
    int rc;

    rc = msix_init(&dev->pci_dev, PCIEMU_HW_IRQ_MSIX_CNT, &dev->internal, PCIEMU_HW_INT_BAR,
                   PCIEMU_HW_MSIX_TABLE_OFFSET, &dev->internal, PCIEMU_HW_INT_BAR,
                   PCIEMU_HW_MSIX_PBA_OFFSET, 0, errp);

    if (rc < 0) {
        qemu_log_mask(LOG_GUEST_ERROR, "Failed to initialize MSI-X");
        return;
    }

    for (i = 0; i < PCIEMU_HW_IRQ_MSIX_CNT; i++) {
        msix_vector_use(&dev->pci_dev, i);
    }

    return;
}

static inline void pciemu_irq_init_msi(PCIEMUDevice *dev, Error **errp)
{
    if (msi_init(&dev->pci_dev, 0, PCIEMU_HW_IRQ_CNT, true, false, errp)) {
        qemu_log_mask(LOG_GUEST_ERROR, "MSI Init Error\n");
        return;
    }
}

static inline void pciemu_irq_init_intx(PCIEMUDevice *dev, Error **errp)
{
    uint8_t *pci_conf = dev->pci_dev.config;
    pci_config_set_interrupt_pin(pci_conf, PCIEMU_HW_IRQ_INTX + 1);
}

static inline void pciemu_irq_raise_intx(PCIEMUDevice *dev)
{
    dev->irq.status.pin.raised = true;
    pci_set_irq(&dev->pci_dev, 1);
}

static inline void pciemu_irq_raise_msi(PCIEMUDevice *dev, unsigned int vector)
{
    if (vector >= PCIEMU_IRQ_MAX_VECTORS_MSI)
        return;
    MSIVector *msi_vector = &dev->irq.status.msi.msi_vectors[vector];

    msi_vector->raised = true;
    msi_notify(&dev->pci_dev, vector);
}

static inline void pciemu_irq_raise_msix(PCIEMUDevice *dev, unsigned int vector)
{
    if (vector >= PCIEMU_IRQ_MAX_VECTORS_MSIX)
        return;
    MSIVector *msix_vector = &dev->irq.status.msix.msix_vectors[vector];

    msix_vector->raised = true;
    msix_notify(&dev->pci_dev, vector);
}

static inline void pciemu_irq_lower_intx(PCIEMUDevice *dev)
{
    dev->irq.status.pin.raised = false;
    pci_set_irq(&dev->pci_dev, 0);
}

static inline void pciemu_irq_lower_msi(PCIEMUDevice *dev, unsigned int vector)
{
    if (vector >= PCIEMU_IRQ_MAX_VECTORS_MSI)
        return;
    MSIVector *msi_vector = &dev->irq.status.msi.msi_vectors[vector];
    if (!msi_vector->raised)
        return;
    msi_vector->raised = false;
}

static inline void pciemu_irq_lower_msix(PCIEMUDevice *dev, unsigned int vector)
{
    if (vector >= PCIEMU_IRQ_MAX_VECTORS_MSIX)
        return;
    MSIVector *msix_vector = &dev->irq.status.msix.msix_vectors[vector];
    if (!msix_vector->raised)
        return;
    msix_vector->raised = false;
}

/* -----------------------------------------------------------------------------
 *  Public
 * -----------------------------------------------------------------------------
 */

void pciemu_irq_raise(PCIEMUDevice *dev, unsigned int vector)
{
    /* MSI-X is available */
    if (msix_enabled(&dev->pci_dev)) {
        pciemu_irq_raise_msix(dev, vector);
        return;
    }

    /* MSI is available */
    if (msi_enabled(&dev->pci_dev)) {
        pciemu_irq_raise_msi(dev, vector);
        return;
    }

    /* If no MSI-X & MSI available on host, we should fallback to pin IRQ assertion */
    pciemu_irq_raise_intx(dev);
    return;
}

void pciemu_irq_lower(PCIEMUDevice *dev, unsigned int vector)
{
    /* MSI-X is available */
    if (msix_enabled(&dev->pci_dev)) {
        pciemu_irq_lower_msix(dev, vector);
        return;
    }

    /* MSI is available */
    if (msi_enabled(&dev->pci_dev)) {
        pciemu_irq_lower_msi(dev, vector);
        return;
    }

    /* If no MSI-X & MSI available on host, we should fallback to pin IRQ assertion */
    pciemu_irq_lower_intx(dev);
    return;
}

void pciemu_irq_reset(PCIEMUDevice *dev)
{
    /* MSI-X is available */
    if (msix_enabled(&dev->pci_dev)) {
        for (int i = PCIEMU_HW_IRQ_MSIX_VECTOR_START; i <= PCIEMU_HW_IRQ_MSIX_VECTOR_END; ++i)
            pciemu_irq_lower_msix(dev, i);
        return;
    }

    /* MSI is available */
    if (msi_enabled(&dev->pci_dev)) {
        for (int i = PCIEMU_HW_IRQ_VECTOR_START; i <= PCIEMU_HW_IRQ_VECTOR_END; ++i)
            pciemu_irq_lower_msi(dev, i);
        return;
    }

    /* If no MSI-X & MSI available on host, we should fallback to pin IRQ assertion */
    pciemu_irq_lower_intx(dev);
    return;
}

void pciemu_irq_init(PCIEMUDevice *dev, Error **errp)
{
    /* configure line based interrupt if fallback is needed */
    pciemu_irq_init_intx(dev, errp);
    /* try to confingure MSI based interrupt */
    pciemu_irq_init_msi(dev, errp);
    /* try to confingure MSI-X based interrupt (preferred) */
    pciemu_irq_init_msix(dev, errp);
}

void pciemu_irq_fini(PCIEMUDevice *dev)
{
    pciemu_irq_reset(dev);
    msi_uninit(&dev->pci_dev);
    msix_uninit(&dev->pci_dev, &dev->reg, &dev->reg);
}
