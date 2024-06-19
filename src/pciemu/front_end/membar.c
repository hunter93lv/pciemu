#include "qemu/osdep.h"
#include "exec/target_page.h"
#include "qapi/error.h"
#include "qemu/log.h"
#include "qemu/units.h"
#include "membar.h"
#include "pciemu/glue/glue.h"

/* -----------------------------------------------------------------------------
 *  Private
 * -----------------------------------------------------------------------------
 */

static uint64_t pciemu_membar_read(void *opaque, hwaddr addr, unsigned int size)
{
    PCIEMUDevice *dev = opaque;
    uint64_t val = ~0ULL;

    if (glue_read_devmem_by_addr(dev, addr, &val))
        return val;

    return val;
}

static void pciemu_membar_write(void *opaque, hwaddr addr, uint64_t val,
                              unsigned size)
{
    PCIEMUDevice *dev = opaque;

    if (glue_write_devmem_by_addr(dev, addr, val))
        return ;

    return; 
}

/* -----------------------------------------------------------------------------
 *  Public
 * -----------------------------------------------------------------------------
 */

void pciemu_membar_reset(PCIEMUDevice *dev)
{
	return;
}

void pciemu_membar_init(PCIEMUDevice *dev, Error **errp)
{
    uint8_t bar_type = PCI_BASE_ADDRESS_SPACE_MEMORY;

    if (PCIEMU_HW_MEM_BAR_WIDE == 64)
        bar_type |= PCI_BASE_ADDRESS_MEM_TYPE_64;
    else if (PCIEMU_HW_MEM_BAR_WIDE == 32)
        bar_type |= PCI_BASE_ADDRESS_MEM_TYPE_32;
    else
        bar_type |= PCI_BASE_ADDRESS_MEM_TYPE_64;

    if (PCIEMU_HW_MEM_BAR_PREFETCHABLE)
        bar_type |= PCI_BASE_ADDRESS_MEM_PREFETCH;

    memory_region_init_io(&dev->dmem, OBJECT(dev), &pciemu_membar_ops, dev,
                          "pciemu-dmem", PCIEMU_HW_MEM_BAR_SIZE);
    pci_register_bar(&dev->pci_dev, PCIEMU_HW_MEM_BAR, bar_type, &dev->dmem);
}

void pciemu_membar_fini(PCIEMUDevice *dev)
{
    pciemu_membar_reset(dev);
}

const MemoryRegionOps pciemu_membar_ops = {
    .read = pciemu_membar_read,
    .write = pciemu_membar_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
    .valid = {
    .min_access_size = 4,
    .max_access_size = 8,
    },
    .impl = {
        .min_access_size = 4,
        .max_access_size = 8,
    },
};
