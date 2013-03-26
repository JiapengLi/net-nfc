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

// int net_write