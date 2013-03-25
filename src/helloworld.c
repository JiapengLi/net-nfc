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
//打开串口
	fd = open("/dev/ttyATH0", O_RDWR|O_NOCTTY);
	if (fd == -1) {
		printf("Cann't open Serial\r\n");
		return -1;
	} else {
		printf("ttyATH0 opened\r\n");
	}
//	读取串口当前属性
	tcgetattr(fd, &attr);
//	设置最少接收字符个数为	0
	attr.c_cc[VMIN] = 0;
	/*	不处理	iflag、oflag和lflag*/
	attr.c_iflag = 0;
	attr.c_oflag = 0;
	attr.c_lflag = 0;
	
	/* 设置波特率为 9600，字符长度为8位，偶校验，允许接收*/
	attr.c_cflag = B115200 | CS8 | CLOCAL | CREAD;
	
	cfsetispeed(&attr,B115200);
	cfsetospeed(&attr,B115200);
	
//	设置串口属性
	tcsetattr(fd, TCSANOW, &attr);
//	发送字符串
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
		//	清除监测集合
		FD_ZERO(&fds);
//	将串口句柄加入到监测集合中
		FD_SET(fd, &fds);

		//设置超时为	5	秒
		tv.tv_sec = 5;
		tv.tv_usec = 0;
//	监测串口是否有数据接收到，超时为	5	秒
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
			//接收最多100个字符
			ret = read(fd, buf, 100);
			for(i=0; i<ret; i++){
				printf("%02X ", buf[i]);
			}
			printf("read over\r\n");
//			write(fd, buf, ret);

			// 关闭串口
//			close(fd);
//			printf("return 0\n");
			break;
		}

	}

	return 0;
}
