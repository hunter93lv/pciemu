/* regbar.c - Register BAR operations
 *
 * Official documentation on MMIO and memory operations can be found in :
 *    https://qemu.readthedocs.io/en/latest/devel/memory.html
 *
 * Copyright (c) 2023 Luiz Henrique Suraty Filho <luiz-dev@suraty.com>
 *
 * SPDX-License-Identifier: GPL-2.0
 *
 */
#include "qemu/osdep.h"
#include "exec/target_page.h"
#include "qapi/error.h"
#include "qemu/log.h"
#include "qemu/units.h"
#include "regbar.h"
#include "pciemu/glue/glue.h"

/**
 * pciemu_regbar_read: Callback for read operations
 *
 * Read from the memory region and return the correspondent value.
 * Only valid for regions with READ operations (mostly regiters)
 *
 * @opaque: opaque pointer that points to instantiated object
 * @addr: address being accessed (relative to the Memory Region)
 * @size: read size in bytes (1, 2, 4, or 8)
 */
static uint64_t pciemu_regbar_read(void *opaque, hwaddr addr, unsigned int size)
{
    PCIEMUDevice *dev = opaque;
    uint64_t val = ~0ULL;

    if (glue_read_reg_by_addr(dev, addr, &val))
         return val;
     return val;
}

/**
 * pciemu_regbar_write: Callback for write operations
 *
 * Write to the memory region.
 * For now, it writes to reg0 regardless of the address.
 *
 * @opaque: opaque pointer that points to instantiated object
 * @addr: address being written (relative to the Memory Region)
 * @val: value to be written
 * @size: write size in bytes (1, 2, 4, or 8)
 */
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

/**
 * pciemu_regbar_reset: regbar reset
 *
 * As the regbar block controls the device registers (reg),
 * we just clean them up here.
 *
 * @dev: Instance of PCIEMUDevice object being used
 */
void pciemu_regbar_reset(PCIEMUDevice *dev)
{
	return;
}

/**
 * pciemu_regbar_init: Register BAR initialization
 *
 * Initializes the regbar block for the instantiated PCIEMUDevice object.
 * Note that we receive a pointer for a PCIEMUDevice, but, due to the OOP hack
 * done by the QEMU Object Model, we can easily get the parent PCIDevice.
 *
 * @dev: Instance of PCIEMUDevice object being initialized
 * @errp: pointer to indicate errors
 */
void pciemu_regbar_init(PCIEMUDevice *dev, Error **errp)
{
    /* BAR 0 will have memory region described in mmio (pciemu_mmio_ops) */
    /* Keeping the BAR size as the page size of the guest */
    memory_region_init_io(&dev->reg, OBJECT(dev), &pciemu_regbar_ops, dev,
                          "pciemu-reg", PCIEMU_HW_SIZE_BAR0);
    pci_register_bar(&dev->pci_dev, PCIEMU_HW_BAR0, PCI_BASE_ADDRESS_SPACE_MEMORY,
                     &dev->reg);
}

/**
 * pciemu_regbar_fini: Register BAR finalization
 *
 * Finalizes the regbar block for the instantiated PCIEMUDevice object.
 * Note that we receive a pointer for a PCIEMUDevice, but, due to the OOP hack
 * done by the QEMU Object Model, we can easily get the parent PCIDevice.
 *
 * @dev: Instance of PCIEMUDevice object being finalized
 */
void pciemu_regbar_fini(PCIEMUDevice *dev)
{
    pciemu_regbar_reset(dev);
}

/**
 * pciemu_regbar_ops: Memory region description
 *
 * Describes the operations and behavior (with callbacks)
 * of the device memory region dedicate for regbar.
 *
 */
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
