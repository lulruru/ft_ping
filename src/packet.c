#include "ft_ping.h"


void init_packet(t_ping *ping, char *sendbuf, int seq){
	struct icmp	*pkt;

	memset(sendbuf, 0, PING_PKT_S);
	pkt = (struct icmp *)sendbuf;
	pkt->icmp_type = ICMP_ECHO;
	pkt->icmp_code = 0;
	pkt->icmp_id = htons(ping->id);
	pkt->icmp_seq = htons(seq);
	memset(sendbuf + ICMP_HDR_LEN, 0x42, PING_PKT_S - ICMP_HDR_LEN);
	pkt->icmp_cksum = 0;
	pkt->icmp_cksum = checksum(sendbuf, PING_PKT_S);
}

int	send_packet(t_ping *ping, char *buf){
	ssize_t	ret;

	ret = sendto(ping->sockfd, buf, PING_PKT_S, 0,(struct sockaddr *)&ping->sockaddr, sizeof(ping->sockaddr));
	if (ret < 0)
	{
		perror("ft_ping: sendto");
		return (EXIT_FAILURE);
	}
	ping->stats.transmitted++;
	return (EXIT_SUCCESS);
}