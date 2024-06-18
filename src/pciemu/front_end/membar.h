/* mmio.h - Memory Mapped IO operations
 *
 * Copyright (c) 2023 Luiz Henrique Suraty Filho <luiz-dev@suraty.com>
 *
 * SPDX-License-Identifier: GPL-2.0
 *
 */

#ifndef PCIEMU_MEMBAR_H
#define PCIEMU_MEMBAR_H

#include "pciemu/front_end/pciemu.h"

/* forward declaration (defined in pciemu.h) to avoid circular reference */
typedef struct PCIEMUDevice PCIEMUDevice;

void pciemu_membar_reset(PCIEMUDevice *dev);

void pciemu_membar_init(PCIEMUDevice *dev, Error **errp);

void pciemu_membar_fini(PCIEMUDevice *dev);

extern const MemoryRegionOps pciemu_membar_ops;

#endif /* PCIEMU_MEMBAR_H */
