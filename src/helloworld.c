#include <termios.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>

int main()
{
	int fd;
	struct termios attr;
	fd_set fds;
	struct timeval tv;
	unsigned char buf[1024];
	int i,ret;
//�򿪴���
	fd = open("/dev/ttyATH0", O_RDWR|O_NOCTTY);
	if (fd == -1) {
		printf("Cann't open Serial\r\n");
		return -1;
	} else {
		printf("ttyATH0 opened\r\n");
	}
//	��ȡ���ڵ�ǰ����
	tcgetattr(fd, &attr);
//	�������ٽ����ַ�����Ϊ	0
	attr.c_cc[VMIN] = 0;
	/*	������	iflag��oflag��lflag*/
	attr.c_iflag = 0;
	attr.c_oflag = 0;
	attr.c_lflag = 0;
	
	/* ���ò�����Ϊ 9600���ַ�����Ϊ8λ��żУ�飬�������*/
	attr.c_cflag = B115200 | CS8 | CLOCAL | CREAD;
	
	cfsetispeed(&attr,B115200);
	cfsetospeed(&attr,B115200);
	
//	���ô�������
	tcsetattr(fd, TCSANOW, &attr);
//	�����ַ���
	buf[0] = 0x55;
	buf[1] = 0x55;
	buf[2] = 0x00;
	buf[3] = 0x00;
	buf[4] = 0x00;
	buf[5] = 0x00;
	buf[6] = 0x00;
	write(fd, buf, 7);
	
	buf[0] = 0xFF;
	buf[1] = 0x05;
	buf[2] = 0xFB;
	buf[3] = 0xD4;
	buf[4] = 0x14;
	buf[5] = 0x01;
	buf[6] = 0x14;
	buf[7] = 0x00;
	buf[8] = 0x03;
	buf[9] = 0x00;
	write(fd, buf, 9+1);
	
	// while(1){
		// ret = read(fd, buf, 100);
		// for(i=0; i<ret; i++){
			// printf("%2X ", buf[i]);
		// }
		// if(i){
			// printf("read over\r\n");
		// }
		
	// }
//	buf[] = ;
//	buf[] = ;
//	buf[] = ;
	while(1) {
		//	�����⼯��
		FD_ZERO(&fds);
//	�����ھ�����뵽��⼯����
		FD_SET(fd, &fds);

		//���ó�ʱΪ	5	��
		tv.tv_sec = 5;
		tv.tv_usec = 0;
//	��⴮���Ƿ������ݽ��յ�����ʱΪ	5	��
		switch (select(fd+1, &fds, NULL, NULL, &tv) ) {
		case -1:
			/** can reopen serial */
			printf("select error\n");
			return -1;
			break;
		case 0:
			printf("select timeout\n");
//			return 0;
			break;

		default:
			//printf("read from Serial\n");
			//�������100���ַ�
			ret = read(fd, buf, 100);
			for(i=0; i<ret; i++){
				printf("%02X ", buf[i]);
			}
			printf("read over\r\n");
//			write(fd, buf, ret);

			// �رմ���
//			close(fd);
//			printf("return 0\n");
			break;
		}

	}

	return 0;
}
