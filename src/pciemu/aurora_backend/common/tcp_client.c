#include "qemu/osdep.h"
#include "qapi/error.h"
#include "qemu/log.h"
#include "qemu/sockets.h"
#include "tcp_client.h"
#include "protocol.h"

int fd = -1;

tcp_mutex t_mutex = {
    .init = false
};

static inline void tcp_mutex_lock(void)
{
    if (!t_mutex.init) {
        qemu_mutex_init(&t_mutex.tcp_mutex);
    }
    qemu_mutex_lock(&t_mutex.tcp_mutex);
}

static inline void tcp_mutex_unlock(void)
{
    if (!t_mutex.init) {
        qemu_mutex_init(&t_mutex.tcp_mutex);
    }
    qemu_mutex_unlock(&t_mutex.tcp_mutex);
}

static int link_emu_server(void)
{
    int r;
    struct sockaddr_in sddr;
    char sever_ip[32] = {"127.0.0.1"};
    char sever_port_str[20] = {0};
    int sever_port = 4444;

    if (fd > 0)
        return -1;

    sprintf(sever_ip, "%s\n", getenv("EMU_SERVER_IP"));
    sprintf(sever_port_str, "%s\n", getenv("EMU_SERVER_PORT"));
    sever_port = atoi(sever_port_str);

    fd = qemu_socket(AF_INET, SOCK_STREAM, 0);
    if(fd < 0) {
        qemu_log_mask(LOG_GUEST_ERROR, "Failed to create socket!!");
        return -1;
    }

    memset(&sddr, '\0', sizeof(sddr));
    sddr.sin_family = AF_INET;
    sddr.sin_port = htons(sever_port);
    sddr.sin_addr.s_addr = inet_addr(sever_ip);
    
    r = connect(fd, (struct sockaddr*)&sddr, sizeof(sddr));
    if(r < 0) {
        qemu_log_mask(LOG_GUEST_ERROR, "Failed to connect server!!");
        return -1;
    }

    return 0;
}

static int send_to_server(char *buff, int len)
{
    if (!buff)
        return -1;

    tcp_mutex_lock();
    if (fd < 0) {
        link_emu_server();
        if (fd < 0) {
            tcp_mutex_unlock();
            return -1;
        }
    }

    if (send(fd, buff, len, 0) < 0) {
        qemu_log_mask(LOG_GUEST_ERROR, "Failed to send to server!!");
        tcp_mutex_unlock();
        return -1;
    }

    tcp_mutex_unlock();
    return 0;
}

static int recv_from_server(char *buff, int len)
{
    if (!buff)
        return -1;

    tcp_mutex_lock();
    if (fd < 0) {
        link_emu_server();
        if (fd < 0) {
            tcp_mutex_unlock();
            return -1;
        }
    }

    if (recv(fd, buff, len, 0) < 0) {
        qemu_log_mask(LOG_GUEST_ERROR, "Failed to recv from server!!");
        tcp_mutex_unlock();
        return -1;
    }

    tcp_mutex_unlock();
    return 0;
}

int tcp_send_mem_read_req(uint64_t addr, uint64_t *value)
{
    struct mem_rw_req req;

    if (!value)
        return -1;

    req.cmd = MEM_RW_CMD_R;
    req.addr = addr;

    if (send_to_server((char *)&req, sizeof(req)))
        return -1;

    if (recv_from_server((char *)&req, sizeof(req)))
        return -1;

    if (req.addr != addr || req.resp != MEM_RW_RESP_OK) {
        *value = 0xdeadbeef;
        return -1;
    }

    *value = req.value;

    return 0;
}

int tcp_send_mem_write_req(uint64_t addr, uint64_t value)
{
    struct mem_rw_req req;

    req.cmd = MEM_RW_CMD_W;
    req.addr = addr;
    req.value = value;

    if (send_to_server((char *)&req, sizeof(req)))
        return -1;

    if (recv_from_server((char *)&req, sizeof(req)))
        return -1;

    if (req.addr != addr || req.resp != MEM_RW_RESP_OK)
        return -1;

    return 0;
}
