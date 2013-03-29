#include <stdio.h>
#include "uart.h"
#include "net.h"

typedef enum {
	SYS_STA_IDLE,
	SYS_STA_CONNECTED,
} sys_sta_t;

void put_buf(uint8_t *buf, int len)
{
	int i;
	for(i=0; i<len; i++) {
		printf("%02X ", buf[i]);
	}
	printf("\n");
}
int main(int argc, char *argv[])
{
	int serial, net, client, client_tmp;
	struct sockaddr_in client_addr;
	uint8_t buf[300];
	int ret,i;
	sys_sta_t sys_sta = SYS_STA_IDLE;

	serial = uart_open("/dev/ttyATH0", B115200);

	net = net_open(8888);
	if(net == -1) {
		perror("net open failed");
	}

	buf[0] = 0x55;
	buf[1] = 0x55;
	buf[2] = 0x00;
	buf[3] = 0x00;
	buf[4] = 0x00;
	uart_write(serial, buf, 5, 0);

	buf[0] = 0x00;
	buf[1] = 0x00;
	buf[0+2] = 0xFF;
	buf[1+2] = 0x05;
	buf[2+2] = 0xFB;
	buf[3+2] = 0xD4;
	buf[4+2] = 0x14;
	buf[5+2] = 0x01;
	buf[6+2] = 0x14;
	buf[7+2] = 0x00;
	buf[8+2] = 0x03;
	buf[9+2] = 0x00;
	uart_write(serial, buf, 12, 0);
	ret = uart_read(serial, buf, 30, 100);
	if(ret > 0) {
		for(i=0; i<ret; i++) {
			printf("%02X ", buf[i]);
		}
		printf("\n");
	} else {
		printf("read error\n");
	}

	while(1) {
		switch(sys_sta) {
		case SYS_STA_IDLE:
			client = net_accept(net, &client_addr, 3000);
			if(client>0) {
				printf("Client log in\n");
				sys_sta = SYS_STA_CONNECTED;
			}
			break;
		case SYS_STA_CONNECTED:
			ret = net_read(client, buf, 128, 30);
			if(ret>0) {
				uart_write(serial, buf, ret, 0);
				printf("SEND:");
				put_buf(buf, ret);
			} else if(ret == -1) {
				printf("Close client:0\n");
				close(client);
				sys_sta = SYS_STA_IDLE;
			}

			ret = uart_read(serial, buf, 128, 30);
			if(ret>0) {
				if((net_write(client, buf, ret, 0)) == -1) {
					printf("Close client:1\n");
					close(client);
					sys_sta = SYS_STA_IDLE;
				}
				printf("RCV:");
				put_buf(buf, ret);
			}

			client_tmp = net_accept(net, &client_addr, 30);
			if(client_tmp>0) {
				printf("Another client log in, reject\n");
				net_write(client_tmp, "Connection has created!!!", \
				          strlen("Connection has created!!!"), 0);
				close(client_tmp);
			}
			break;
		}
	}
	return 0;
}


