#include "qemu/osdep.h"
#include "qapi/error.h"
#include "qemu/log.h"
#include "qemu/sockets.h"
#include "tcp_client.h"
#include "protocol.h"

int fd = -1;
int port = 4444;

static int link_emu_server(void)
{
	int r;
	struct sockaddr_in sddr;

	if (fd > 0)
		return -1;

	fd = qemu_socket(AF_INET, SOCK_STREAM, 0);
	if(fd < 0) {
		qemu_log_mask(LOG_GUEST_ERROR, "Failed to create socket!!");
		return -1;
	}

	memset(&sddr, '\0', sizeof(sddr));
	sddr.sin_family = AF_INET;
	sddr.sin_port = htons(port);
	sddr.sin_addr.s_addr = inet_addr("127.0.0.1");

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

	if (fd < 0) {
		link_emu_server();
		if (fd < 0)
			return -1;
	}

	if (send(fd, buff, len, 0) < 0) {
		qemu_log_mask(LOG_GUEST_ERROR, "Failed to send to server!!");
		return -1;
	}

	return 0;
}

static int recv_from_server(char *buff, int len)
{
	if (!buff)
		return -1;

	if (fd < 0) {
		link_emu_server();
		if (fd < 0)
			return -1;
	}

	if (recv(fd, buff, len, 0) < 0) {
		qemu_log_mask(LOG_GUEST_ERROR, "Failed to recv from server!!");
		return -1;
	}

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
