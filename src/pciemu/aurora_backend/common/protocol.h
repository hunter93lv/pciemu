#ifndef PROTOCOL_H
#define PROTOCOL_H

enum {
    MEM_RW_CMD_R = 1,
    MEM_RW_CMD_W,
};

enum {
    MEM_RW_RESP_OK = 1,
    MEM_RW_RESP_ERR,
};

struct mem_rw_req {
    uint64_t cmd;
    uint64_t addr;
    uint64_t value;
    uint64_t resp;
};

#endif /* PROTOCOL_H */
