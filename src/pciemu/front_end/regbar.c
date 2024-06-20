#include "qemu/osdep.h"
#include "exec/target_page.h"
#include "qapi/error.h"
#include "qemu/log.h"
#include "qemu/units.h"
#include "regbar.h"
#include "pciemu/glue/glue.h"


static uint64_t pciemu_regbar_read(void *opaque, hwaddr addr, unsigned int size)
{
    PCIEMUDevice *dev = opaque;
    uint64_t val = ~0ULL;

    if (glue_read_reg_by_addr(dev, addr, &val))
         return val;
     return val;
}

static void pciemu_regbar_write(void *opaque, hwaddr addr, uint64_t val,
                              unsigned size)
{
    PCIEMUDevice *dev = opaque;

    glue_write_reg_by_addr(dev, addr, val);
    return;
}

/* -----------------------------------------------------------------------------
 *  Public
 * -----------------------------------------------------------------------------
 */

void pciemu_regbar_reset(PCIEMUDevice *dev)
{
	return;
}

void pciemu_regbar_init(PCIEMUDevice *dev, Error **errp)
{
    uint8_t bar_type = PCI_BASE_ADDRESS_SPACE_MEMORY;

    if (PCIEMU_HW_REG_BAR_WIDE == 64)
        bar_type |= PCI_BASE_ADDRESS_MEM_TYPE_64;
    else if (PCIEMU_HW_REG_BAR_WIDE == 32)
        bar_type |= PCI_BASE_ADDRESS_MEM_TYPE_32;
    else
        bar_type |= PCI_BASE_ADDRESS_MEM_TYPE_32;

    if (PCIEMU_HW_REG_BAR_PREFETCHABLE)
        bar_type |= PCI_BASE_ADDRESS_MEM_PREFETCH;

    memory_region_init_io(&dev->reg, OBJECT(dev), &pciemu_regbar_ops, dev,
                          "pciemu-reg", PCIEMU_HW_REG_BAR_SIZE);
    pci_register_bar(&dev->pci_dev, PCIEMU_HW_REG_BAR, bar_type, &dev->reg);
}

void pciemu_regbar_fini(PCIEMUDevice *dev)
{
    pciemu_regbar_reset(dev);
}

const MemoryRegionOps pciemu_regbar_ops = {
    .read = pciemu_regbar_read,
    .write = pciemu_regbar_write,
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
