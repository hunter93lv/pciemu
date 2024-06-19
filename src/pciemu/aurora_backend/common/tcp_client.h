#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include "qemu/thread.h"

typedef struct tcp_mutex {
    QemuMutex tcp_mutex;
    bool init;
}tcp_mutex;

int tcp_send_mem_read_req(uint64_t addr, uint64_t *value);

int tcp_send_mem_write_req(uint64_t addr, uint64_t value);


#endif /* TCP_CLIENT_H */
