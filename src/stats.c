#include "ft_ping.h"

int	is_dup(t_ping *ping, unsigned short seq)
{
	unsigned char	mask;

	mask = 1 << (seq % 8);
	if (ping->dup_table[seq / 8] & mask)
		return (1);
	ping->dup_table[seq / 8] |= mask;
	return (0);
}

void	update_stats(t_stats *s, double rtt)
{
	if (s->received == 0 || rtt < s->rtt_min)
		s->rtt_min = rtt;
	if (rtt > s->rtt_max)
		s->rtt_max = rtt;
	s->rtt_sum += rtt;
	s->rtt_sumsq += rtt * rtt;
	s->received++;
}