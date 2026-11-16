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
	
	printf("%s",ping.opts.host);
	
}