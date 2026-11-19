#include "ft_ping.h"

int main(int ac, char **av)
{
	t_ping	ping;

	memset(&ping, 0, sizeof(ping));
	if(parse_args(ac, av, &ping.opts))
		return 64;
	if (ping.opts.help)
		return (print_usage(), 0);
	if (ping.opts.usage)
		return (print_short_usage(), 0);
	find_host(&ping);
	ping.id = getpid() & 0xFFFF;
	if (find_host(&ping) || init_socket(&ping))
		return (1);
	ping.id = getpid() & 0xFFFF;
	setup_signals();
	printf("PING %s (%s): %d data bytes\n", ping.opts.host,
		ping.ip_str, PING_PKT_S - ICMP_HDR_LEN);
	ping_loop(&ping);
	print_summary(&ping);
	close(ping.sockfd);
	return (ping.stats.received == 0);
	printf("%s",ping.opts.host);
	
}