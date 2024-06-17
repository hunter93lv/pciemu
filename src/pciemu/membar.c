/* mmio.c - Memory Mapped IO operations
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
#include "membar.h"
#include "irq.h"
#include "pciemu_hw.h"

/* -----------------------------------------------------------------------------
 *  Private
 * -----------------------------------------------------------------------------
 */

/**
 * pciemu_membar_valid_access: Check whether the access is valid
 *
 * The size verification here is not required.
 * (memory_region_access_valid function in QEMU core will filter those out)
 *
 * @addr: address being accessed (relative to the Memory Region)
 * @size: read size in bytes (1, 2, 4, or 8)
 */
static inline bool pciemu_membar_valid_access(hwaddr addr, unsigned int size)
{
    return true;
}

/**
 * pciemu_membar_read: Callback for read operations
 *
 * Read from the memory region and return the correspondent value.
 * Only valid for regions with READ operations (mostly regiters)
 *
 * @opaque: opaque pointer that points to instantiated object
 * @addr: address being accessed (relative to the Memory Region)
 * @size: read size in bytes (1, 2, 4, or 8)
 */
static uint64_t pciemu_membar_read(void *opaque, hwaddr addr, unsigned int size)
{
    PCIEMUDevice *dev = opaque;
    uint64_t val = ~0ULL;
    if (!pciemu_membar_valid_access(addr, size))
        return val;

    return val;
}

/**
 * pciemu_membar_write: Callback for write operations
 *
 * Write to the memory region.
 * For now, it writes to reg0 regardless of the address.
 *
 * @opaque: opaque pointer that points to instantiated object
 * @addr: address being written (relative to the Memory Region)
 * @val: value to be written
 * @size: write size in bytes (1, 2, 4, or 8)
 */
static void pciemu_membar_write(void *opaque, hwaddr addr, uint64_t val,
                              unsigned size)
{
    PCIEMUDevice *dev = opaque;
    if (!pciemu_membar_valid_access(addr, size))
        return;
    return; 
}

/* -----------------------------------------------------------------------------
 *  Public
 * -----------------------------------------------------------------------------
 */

/**
 * pciemu_membar_reset: Memory BAR reset
 *
 * As the mmio block controls the device registers (reg),
 * we just clean them up here.
 *
 * @dev: Instance of PCIEMUDevice object being used
 */
void pciemu_membar_reset(PCIEMUDevice *dev)
{
	return;
}

/**
 * pciemu_membar_init: Memory BAR initialization
 *
 * Initializes the MMIO block for the instantiated PCIEMUDevice object.
 * Note that we receive a pointer for a PCIEMUDevice, but, due to the OOP hack
 * done by the QEMU Object Model, we can easily get the parent PCIDevice.
 *
 * @dev: Instance of PCIEMUDevice object being initialized
 * @errp: pointer to indicate errors
 */
void pciemu_membar_init(PCIEMUDevice *dev, Error **errp)
{

    memory_region_init_io(&dev->dmem, OBJECT(dev), &pciemu_membar_ops, dev,
                          "pciemu-dmem", PCIEMU_HW_SIZE_BAR1);
    pci_register_bar(&dev->pci_dev, PCIEMU_HW_BAR1, PCI_BASE_ADDRESS_SPACE_MEMORY
                                                  | PCI_BASE_ADDRESS_MEM_TYPE_64
                                                  | PCI_BASE_ADDRESS_MEM_PREFETCH,
                     &dev->dmem);
}

/**
 * pciemu_membar_fini: Memory BAR finalization
 *
 * Finalizes the MMIO block for the instantiated PCIEMUDevice object.
 * Note that we receive a pointer for a PCIEMUDevice, but, due to the OOP hack
 * done by the QEMU Object Model, we can easily get the parent PCIDevice.
 *
 * @dev: Instance of PCIEMUDevice object being finalized
 */
void pciemu_membar_fini(PCIEMUDevice *dev)
{
    pciemu_membar_reset(dev);
}

/**
 * pciemu_membar_ops: Memory region description
 *
 * Describes the operations and behavior (with callbacks)
 * of the device memory region dedicate for MMIO.
 *
 */
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
