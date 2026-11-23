#include "ft_ping.h"


void init_packet(t_ping *ping, char *sendbuf, int seq){
	struct icmp	*pkt;
	struct timeval	tv;

	memset(sendbuf, 0, PING_PKT_S);
	pkt = (struct icmp *)sendbuf;
	pkt->icmp_type = ICMP_ECHO;
	pkt->icmp_code = 0;
	pkt->icmp_id = htons(ping->id);
	pkt->icmp_seq = htons(seq);
	memset(sendbuf + ICMP_HDR_LEN, 0x42, PING_PKT_S - ICMP_HDR_LEN);
	gettimeofday(&tv, NULL);
	memcpy(sendbuf + ICMP_HDR_LEN, &tv, sizeof(tv));
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

double	get_rtt(struct icmp *icmp)
{
	struct timeval	now;
	struct timeval	sent;

	gettimeofday(&now, NULL);
	memcpy(&sent, (char *)icmp + ICMP_HDR_LEN, sizeof(sent));
	return ((now.tv_sec - sent.tv_sec) * 1000.0
		+ (now.tv_usec - sent.tv_usec) / 1000.0);
}

void	print_reply(struct ip *ip, struct icmp *icmp, int len, double rtt)
{
	char	src[INET_ADDRSTRLEN];

	inet_ntop(AF_INET, &ip->ip_src, src, sizeof(src));
	printf("%d bytes from %s: icmp_seq=%d ttl=%d time=%.3f ms",
		len, src, ntohs(icmp->icmp_seq), ip->ip_ttl, rtt);
}

void	handle_reply(t_ping *ping, struct ip *ip, struct icmp *icmp, int len)
{
	double	rtt;
	int		dup;

	rtt = get_rtt(icmp);
	dup = is_dup(ping, ntohs(icmp->icmp_seq));
	if (dup)
		ping->stats.duplicates++;
	else
		update_stats(&ping->stats, rtt);
	print_reply(ip, icmp, len, rtt);
	if (dup)
		printf(" (DUP!)");
	printf("\n");
}

static int	recv_failed(void)
{
	if (errno == EAGAIN || errno == EWOULDBLOCK)
		return (RECV_TIMEDOUT);
	if (errno == EINTR)
		return (RECV_SKIP);
	perror("ft_ping: recvfrom");
	return (RECV_FAIL);
}

int	recv_packet(t_ping *ping)
{
	char		buf[RECV_BUF_S];
	ssize_t		len;
	struct ip	*ip;
	struct icmp	*icmp;
	int			hlen;

	len = recvfrom(ping->sockfd, buf, sizeof(buf), 0, NULL, NULL);
	if (len < 0)
		return (recv_failed());
	ip = (struct ip *)buf;
	hlen = ip->ip_hl * 4;
	if (len < hlen + ICMP_HDR_LEN + (ssize_t)sizeof(struct timeval))
		return (RECV_SKIP);
	icmp = (struct icmp *)(buf + hlen);
	if (icmp->icmp_type != ICMP_ECHOREPLY || ntohs(icmp->icmp_id) != ping->id)
		return (RECV_SKIP);
	handle_reply(ping, ip, icmp, len - hlen);
	return (RECV_GOT);
}
