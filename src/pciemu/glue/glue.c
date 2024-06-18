#include "pciemu/front_end/pciemu.h"
#include "pciemu/aurora_backend/aurora.h"
#include "pciemu_hw.h"
#include "glue.h"

int glue_write_devmem_by_addr(PCIEMUDevice *dev, uint64_t addr, uint64_t value)
{
#ifdef AURORA_HW
	return aurora_write_devmem_by_addr(dev, addr, value);
#endif
}

int glue_read_devmem_by_addr(PCIEMUDevice *dev, uint64_t addr, uint64_t *value)
{
#ifdef AURORA_HW
	return aurora_read_devmem_by_addr(dev, addr, value);
#endif
}

int glue_write_reg_by_addr(PCIEMUDevice *dev, uint64_t addr, uint64_t value)
{
#ifdef AURORA_HW
	return aurora_write_reg_by_addr(dev, addr, value);
#endif
}

int glue_read_reg_by_addr(PCIEMUDevice *dev, uint64_t addr, uint64_t *value)
{
#ifdef AURORA_HW
	return aurora_read_reg_by_addr(dev, addr, value);
#endif
}

void glue_backend_reset(PCIEMUDevice *dev)
{
#ifdef AURORA_HW
	return aurora_backend_reset(dev);
#endif
}

void glue_backend_init(PCIEMUDevice *dev)
{
#ifdef AURORA_HW
	return aurora_backend_init(dev);
#endif
}

void glue_backend_fini(PCIEMUDevice *dev)
{
#ifdef AURORA_HW
	return aurora_backend_fini(dev);
#endif
}
