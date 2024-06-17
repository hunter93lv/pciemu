#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include "qemu/osdep.h"
#include "hw/pci/pci.h"

int send_mem_read_req(uint64_t addr, uint64_t *value);

int send_mem_write_req(uint64_t addr, uint64_t value);


#endif /* TCP_CLIENT_H */
