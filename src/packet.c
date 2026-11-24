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

const char	*icmp_error_msg(struct icmp *icmp)
{
	if (icmp->icmp_type == ICMP_TIME_EXCEEDED)
	{
		if (icmp->icmp_code == ICMP_EXC_TTL)
			return ("Time to live exceeded");
		return ("Frag reassembly time exceeded");
	}
	if (icmp->icmp_type == ICMP_DEST_UNREACH)
	{
		if (icmp->icmp_code == ICMP_NET_UNREACH)
			return ("Destination Net Unreachable");
		if (icmp->icmp_code == ICMP_HOST_UNREACH)
			return ("Destination Host Unreachable");
		if (icmp->icmp_code == ICMP_PROT_UNREACH)
			return ("Destination Protocol Unreachable");
		if (icmp->icmp_code == ICMP_PORT_UNREACH)
			return ("Destination Port Unreachable");
		return ("Dest Unreachable, Bad Code");
	}
	return (NULL);
}

void	print_error_dump(struct ip *in, struct icmp *orig)
{
	unsigned char	*p;
	char			src[INET_ADDRSTRLEN];
	char			dst[INET_ADDRSTRLEN];
	int				j;

	p = (unsigned char *)in;
	printf("IP Hdr Dump:\n ");
	j = 0;
	while (j < 20)
	{
		printf("%02x", p[j]);
		if (j % 2)
			printf(" ");
		j++;
	}
	printf("\nVr HL TOS  Len   ID Flg  off TTL Pro  cks      Src      Dst     Data\n");
	printf(" %1x  %1x  %02x %04x %04x", in->ip_v, in->ip_hl, in->ip_tos, ntohs(in->ip_len), ntohs(in->ip_id));
	printf("   %1x %04x", (ntohs(in->ip_off) & 0xe000) >> 13, ntohs(in->ip_off) & 0x1fff);
	printf("  %02x  %02x %04x", in->ip_ttl, in->ip_p, ntohs(in->ip_sum));
	inet_ntop(AF_INET, &in->ip_src, src, sizeof(src));
	inet_ntop(AF_INET, &in->ip_dst, dst, sizeof(dst));
	printf(" %s  %s \n", src, dst);
	printf("ICMP: type %d, code %d, size %d, id 0x%04x, seq 0x%04x\n", orig->icmp_type, orig->icmp_code, ntohs(in->ip_len) - in->ip_hl * 4, 
		ntohs(orig->icmp_id), ntohs(orig->icmp_seq));
}


int	handle_other(t_ping *ping, char *buf, ssize_t len, int hlen)
{
	struct ip	*ip;
	struct ip	*in;
	struct icmp	*icmp;
	struct icmp	*orig;
	const char	*msg;
	char		src[INET_ADDRSTRLEN];

	ip = (struct ip *)buf;
	icmp = (struct icmp *)(buf + hlen);
	msg = icmp_error_msg(icmp);
	if (!msg || len < hlen + ICMP_HDR_LEN + 20 + 8)
		return (RECV_SKIP);
	in = (struct ip *)((char *)icmp + ICMP_HDR_LEN);
	if (in->ip_hl < 5 || len < hlen + ICMP_HDR_LEN + in->ip_hl * 4 + 8)
		return (RECV_SKIP);
	orig = (struct icmp *)((char *)in + in->ip_hl * 4);
	if (orig->icmp_type != ICMP_ECHO || ntohs(orig->icmp_id) != ping->id)
		return (RECV_SKIP);
	inet_ntop(AF_INET, &ip->ip_src, src, sizeof(src));
	printf("%ld bytes from %s: %s\n", (long)(len - hlen), src, msg);
	if (ping->opts.verbose)
		print_error_dump(in, orig);
	return (RECV_GOT);
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
	if (len < (ssize_t)sizeof(struct ip))
		return (RECV_SKIP);
	ip = (struct ip *)buf;
	hlen = ip->ip_hl * 4;
	if (hlen < 20 || len < hlen + ICMP_HDR_LEN)
		return (RECV_SKIP);
	icmp = (struct icmp *)(buf + hlen);
	if (icmp->icmp_type != ICMP_ECHOREPLY)
		return (handle_other(ping, buf, len, hlen));
	if (len < hlen + ICMP_HDR_LEN + (ssize_t)sizeof(struct timeval)
		|| ntohs(icmp->icmp_id) != ping->id)
		return (RECV_SKIP);
	handle_reply(ping, ip, icmp, len - hlen);
	return (RECV_GOT);
}
