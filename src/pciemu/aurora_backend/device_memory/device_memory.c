#include "qemu/osdep.h"
#include "qapi/error.h"
#include "qemu/log.h"
#include "qemu/sockets.h"
#include "device_memory.h"
#include "pciemu/aurora_backend/common/tcp_client.h"

int write_devmem_by_addr(uint64_t addr, uint64_t value)
{
	return send_mem_write_req(addr, value);
}

int read_devmem_by_addr(uint64_t addr, uint64_t *value)
{
	return send_mem_read_req(addr, value);
}

