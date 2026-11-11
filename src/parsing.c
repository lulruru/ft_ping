#include "ft_ping.h"


int parse_option(char *av, t_opts opts)
{
	int	i;
	i = 1;
	while (av[i])
	{
		if(parse_flag(av[i], ))
			return 1;
		i++;
	}
}

int parse_args(int ac, char **av, t_opts *opts)
{
	int i;
	i = 1;

	if(ac == 1)
	{
		fprintf(stderr, "ft_ping: missing host operand\n");
		return (1);
	}
	while(i < ac)
	{
		if(av[i] == "-" && av[i][1])
		{
			if(parse_arg(av[i], opts))
				return 1;
		}
		else if(!opts->host)
			opts->host = av[i];
		i++;
	}

	return 0;
}


// ping: missing host operand
// Try 'ping --help' or 'ping --usage' for more information.