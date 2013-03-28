#include "net.h"

int net_open(int portnumber)
{
	struct sockaddr_in server_addr;
	int sockfd;
	
	if((sockfd=socket(AF_INET,SOCK_STREAM,0))==-1) {
		fprintf(stderr,"Socket error:%s\n\a",strerror(errno));
		return -1;
	}
	
	bzero(&server_addr,sizeof(struct sockaddr_in));
	server_addr.sin_family=AF_INET;
	server_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	server_addr.sin_port=htons(portnumber);

	if(bind(sockfd,(struct sockaddr *)(&server_addr),sizeof(struct sockaddr))==-1) {
		fprintf(stderr,"Bind error:%s\n\a",strerror(errno));
		return -1;
	}

	if(listen(sockfd,5)==-1) {
		fprintf(stderr,"Listen error:%s\n\a",strerror(errno));
		return -1;
	}
	
	return sockfd;
}

int net_accept(int socket_fd, struct sockaddr_in * client_addr, int timeout)
{
	int new_fd, maxfd, res;
	struct timeval timeout_tv;
	socklen_t sin_size;
	struct sockaddr_in client_addr_size;
	fd_set rfds;
	
	FD_ZERO(&rfds);
	FD_SET (socket_fd, &rfds);
	
	if (timeout > 0) {
		timeout_tv.tv_sec = (timeout / 1000);
		timeout_tv.tv_usec = ((timeout % 1000) * 1000);
	}
	res = select(socket_fd+1, &rfds, NULL, NULL, timeout ? &timeout_tv : NULL);
	
	if(res < 0){
		perror("select error");
		return -1;
	}else if(res == 0){
		perror("select timeout");
		return -1;
	}else{
		perror("select signal**");
	}
	
	if(FD_ISSET(socket_fd, &rfds)){
		sin_size = sizeof(client_addr_size);
		new_fd = accept(socket_fd, (struct sockaddr*)client_addr, &sin_size);
		if(new_fd < 0){
			return -1;
		}
	}
	return new_fd;
}

int net_write(int fd, uint8_t *buf, const size_t len, int timeout)
{
	if( (write(fd, buf, len)) == -1 ){
		return -1;
	}else{
		return 0;
	}
}

int net_read(int fd, uint8_t *buf, const size_t len, int timeout)
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