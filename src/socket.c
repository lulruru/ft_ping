#include "ft_ping.h"


int init_socket(t_ping *ping){
	int fd;
	struct timeval tv_out;

	fd = socket(AF_INET,SOCK_RAW, IPPROTO_ICMP);
	if( fd < 0 ){
		perror("ft_ping: socket");
		fprintf(stderr, "need to be root to create raw socket\n");
		return -1;
	}
	tv_out.tv_sec = RECV_TIMEOUT;
	tv_out.tv_usec = 0;
	if(setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv_out, sizeof(tv_out)) < 0){
		perror("ft_ping: setsockopt");
		close(fd);
		return EXIT_FAILURE;
	}
	// int	ttl;

	// ttl = 1;
	// setsockopt(fd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl));
	ping->sockfd = fd;
	return EXIT_SUCCESS;
}