#include "ft_ping.h"

int main(int ac, char **av)
{
	t_opts	opts;

	memset(&opts, 0, sizeof(opts));
	if(parse_args(ac, av, &opts))
		return 64;
	if(opts.help)
	{
		print_usage();
		return 0;
	}
	printf("HELlo");
}