#include "ft_ping.h"

int main(int ac, char **av)
{
	t_ping	ping;

	memset(&ping, 0, sizeof(ping));
	if (parse_args(ac, av, &ping.opts))
	{
		fprintf(stderr, "Try 'ft_ping --help' or "
			"'ft_ping --usage' for more information.\n");
		return (64);
	}
	if (ping.opts.help)
		return (print_usage(), 0);
	if (ping.opts.usage)
		return (print_short_usage(), 0);
	ping.id = getpid() & 0xFFFF;
	if (find_host(&ping) || init_socket(&ping))
		return (1);
	setup_signals();
	printf("PING %s (%s): %d data bytes", ping.opts.host,
		ping.ip_str, PING_PKT_S - ICMP_HDR_LEN);
	if (ping.opts.verbose)
		printf(", id 0x%04x = %d", ping.id, ping.id);
	printf("\n");
	ping_loop(&ping);
	print_summary(&ping);
	close(ping.sockfd);
	return (ping.stats.received == 0);
}