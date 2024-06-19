#include "pciemu/front_end/pciemu.h"
#include "irq.h"
#include "regbar.h"
#include "membar.h"
#include "internal_bar.h"
#include "pciemu/glue/glue.h"


/* -----------------------------------------------------------------------------
 *  Internal functions
 * -----------------------------------------------------------------------------
 */

static void pciemu_reset(PCIEMUDevice *dev)
{
    pciemu_regbar_reset(dev);
    pciemu_internal_bar_reset(dev);
    pciemu_membar_reset(dev);
    pciemu_irq_reset(dev);
    glue_backend_reset(dev);
}

static void pciemu_device_init(PCIDevice *pci_dev, Error **errp)
{
    PCIEMUDevice *dev = PCIEMU_DEVICE(pci_dev);
    pciemu_regbar_init(dev, errp);
    pciemu_internal_bar_init(dev, errp);
    pciemu_membar_init(dev, errp);
    pciemu_irq_init(dev, errp);
    glue_backend_init(dev);
}

static void pciemu_device_fini(PCIDevice *pci_dev)
{
    PCIEMUDevice *dev = PCIEMU_DEVICE(pci_dev);
    glue_backend_fini(dev);
    pciemu_irq_fini(dev);
    pciemu_membar_fini(dev);
    pciemu_internal_bar_fini(dev);
    pciemu_regbar_fini(dev);
}

static void pciemu_device_reset(DeviceState *dev)
{
    pciemu_reset(PCIEMU_DEVICE(dev));
}

static void pciemu_class_init(ObjectClass *klass, void *class_data)
{
    DeviceClass *device_class = DEVICE_CLASS(klass);
    PCIDeviceClass *pci_device_class = PCI_DEVICE_CLASS(klass);

    pci_device_class->realize = pciemu_device_init;
    pci_device_class->exit = pciemu_device_fini;
    pci_device_class->vendor_id = PCIEMU_HW_VENDOR_ID;
    pci_device_class->device_id = PCIEMU_HW_DEVICE_ID;
    pci_device_class->revision = PCIEMU_HW_REVISION;
    pci_device_class->class_id = PCI_CLASS_OTHERS;

    set_bit(DEVICE_CATEGORY_MISC, device_class->categories);
    device_class->desc = PCIEMU_DEVICE_DESC;
    device_class->reset = pciemu_device_reset;
}

static const TypeInfo pciemu_info = {
    .name = TYPE_PCIEMU_DEVICE,
    .parent = TYPE_PCI_DEVICE,
    .instance_size = sizeof(PCIEMUDevice),
    .class_init = pciemu_class_init,
    .interfaces =
        (InterfaceInfo[]){
            { INTERFACE_PCIE_DEVICE },
            {},
        },
};

static void pciemu_register_types(void)
{
    type_register_static(&pciemu_info);
}

type_init(pciemu_register_types)
