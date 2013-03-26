
#include "uart.h"

int uart_open(char *dev, int br)
{
	int fd;
	struct termios term;

	/** open serial port */
	fd = open(dev, O_RDWR | O_NOCTTY | O_NDELAY);
	if(fd == -1) {
		/** open error */

	} else {
		/** non blocking */
		fcntl(fd, F_SETFL, 0);
	}

	/** set baud rate and mode */
	if( tcgetattr(fd, &term) ) {
		/** not a terminal */
		return -1;
	}
	term.c_cc[VMIN] = 0;

	/** 8N1 */
	term.c_cflag &= ~PARENB;
	term.c_cflag &= ~CSTOPB;
	term.c_cflag &= ~CSIZE;
	term.c_cflag |= CS8;
	term.c_oflag &= ~OPOST;/* raw output */
	term.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);/* raw input */

	/** set i/o baud rate */
	cfsetispeed(&term, br);
	cfsetospeed(&term, br);

	/** enable receive */
	term.c_cflag |= (CLOCAL | CREAD);

	/** write configuration */
	if( tcsetattr(fd, TCSANOW, &term) ) {
		/** set baud error */
		return -1;
	}

	return fd;
}

int uart_read(int fd, uint8_t *buf, const size_t len, int timeout)
{
	int received_bytes_count = 0;
	int available_bytes_count = 0;
	const int expected_bytes_count = (int)len;
	int res;
	fd_set rfds;

	do {
select:
		// Reset file descriptor
		FD_ZERO (&rfds);
		FD_SET (fd, &rfds);

		struct timeval timeout_tv;
		if (timeout > 0) {
			timeout_tv.tv_sec = (timeout / 1000);
			timeout_tv.tv_usec = ((timeout % 1000) * 1000);
		}

		res = select (fd + 1, &rfds, NULL, NULL, timeout ? &timeout_tv : NULL);

		if ((res < 0) && (EINTR == errno)) {
			// The system call was interupted by a signal and a signal handler was
			// run.  Restart the interupted system call.
			goto select;
		}

		// Read error
		if (res < 0) {

			return -1;
		}
		// Read time-out
		if (res == 0) {
			if(received_bytes_count){
				return received_bytes_count;
			}
			return -2;
		}

		// Retrieve the count of the incoming bytes
		res = ioctl (fd, FIONREAD, &available_bytes_count);
		if (res != 0) {
			return -1;
		}

		// There is something available, read the data
		res = read (fd, buf + received_bytes_count, MIN(available_bytes_count, (expected_bytes_count - received_bytes_count)));
		// Stop if the OS has some troubles reading the data
		if (res <= 0) {
			return -1;
		}
		received_bytes_count += res;

	} while (expected_bytes_count > received_bytes_count);

	return received_bytes_count;
}

int uart_write(int fd, uint8_t *buf, const size_t len, int timeout)
{
	if( ((int)len == write(fd, buf, len)) ){
		return 0;
	}else{
		return -1;
	}
}

void uart_send_pkt(int fd, uint8_t *cmd, int len)
{
	uint8_t buf[64], checksum;
	int i;
	buf[0] = 0x00;
	buf[1] = 0x00;
	buf[2] = 0xFF;
	buf[3] = (uint8_t)len;
	buf[4] = ~buf[3] + 1;
	checksum = 0;
	for(i=0; i<len; i++) {
		buf[4+i+1] = cmd[i];
		checksum+=cmd[i];
	}
	buf[4+len+1] = (~checksum+1);
	buf[4+len+1+1] = 0x00;
	write(fd, buf, 4+len+1+1+1);
}