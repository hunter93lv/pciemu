#ifndef PCIEMU_H
#define PCIEMU_H

#include "qemu/osdep.h"
#include "hw/pci/pci_device.h"
#include "irq.h"

#define TYPE_PCIEMU_DEVICE "pciemu"
#define PCIEMU_DEVICE_DESC "PCIEMU Device"

OBJECT_DECLARE_TYPE(PCIEMUDevice, PCIEMUDeviceClass, PCIEMU_DEVICE);


typedef struct PCIEMUDeviceClass {
    PCIDeviceClass parent_class;
} PCIEMUDeviceClass;

typedef struct PCIEMUDevice {
    PCIDevice pci_dev;
    IRQStatus irq;
    MemoryRegion reg;
    MemoryRegion dmem;
    MemoryRegion internal;
    void *backend;
} PCIEMUDevice;

#endif /* PCIEMU_H */
