#include "qemu/osdep.h"
#include "qapi/error.h"
#include "qemu/log.h"
#include "qemu/sockets.h"
#include "tcp_client.h"

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

int send_mem_read_req(uint64_t addr, uint64_t *value)
{
	uint64_t buff[128];

	buff[0] = addr;

	if (send_to_server((char *)buff, sizeof(uint64_t)))
		return -1;


	if (recv_from_server((char *)buff, sizeof(uint64_t) * 2))
		return -1;

	if (buff[0] != addr)
		return -1;

	*value = buff[1];

	return 0;
}

int send_mem_write_req(uint64_t addr, uint64_t value)
{
	uint64_t buff[128];

	buff[0] = addr;
	buff[1] = value;

	return send_to_server((char *)buff, sizeof(uint64_t) * 2);
}
