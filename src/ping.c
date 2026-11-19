#include "ft_ping.h"

volatile sig_atomic_t	g_stop = 0;

void	handle_sigint(int sig)
{
	(void)sig;
	g_stop = 1;
}

void	setup_signals(void)
{
	struct sigaction	sa;

	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = handle_sigint;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGINT, &sa, NULL);
}
void	wait_rest(struct timeval *start)
{
	struct timeval	now;
	long	elapsed;

	gettimeofday(&now, NULL);
	elapsed = (now.tv_sec - start->tv_sec) * 1000000L
		+ (now.tv_usec - start->tv_usec);
	if (elapsed < 1000000L && !g_stop)
		usleep(1000000L - elapsed);
}
void	ping_loop(t_ping *ping)
{
	char			buf[PING_PKT_S];
	struct timeval	start;
	int				seq;
	int				ret;

	seq = 0;
	while (!g_stop)
	{
		gettimeofday(&start, NULL);
		init_packet(ping, buf, seq);
		if (send_packet(ping, buf))
			return ;
		ret = RECV_SKIP;
		while (!g_stop && ret == RECV_SKIP)
			ret = recv_packet(ping);
		if (ret == RECV_FAIL)
			return ;
		wait_rest(&start);
		seq++;
	}
}

void	print_summary(t_ping *ping)
{
	t_stats	*s;
	double	avg;
	double	var;
	double	loss;

	s = &ping->stats;
	loss = 0;
	if (s->transmitted)
		loss = (s->transmitted - s->received) * 100.0 / s->transmitted;
	printf("--- %s ping statistics ---\n", ping->opts.host);
	printf("%ld packets transmitted, %ld packets received, ",
		s->transmitted, s->received);
	if (s->duplicates)
		printf("+%ld duplicates, ", s->duplicates);
	printf("%.0f%% packet loss\n", loss);
	if (!s->received)
		return ;
	avg = s->rtt_sum / s->received;
	var = s->rtt_sumsq / s->received - avg * avg;
	if (var < 0)
		var = 0;
	printf("round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n",
		s->rtt_min, avg, s->rtt_max, sqrt(var));
}
