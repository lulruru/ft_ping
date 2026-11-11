#include "ft_ping.h"


void	print_usage(void)
{
	printf("Usage: ft_ping [OPTION...] HOST ...\n");
	printf("Send ICMP ECHO_REQUEST packets to network hosts.\n\n");
	printf(" Options:\n");
	printf("  -v    verbose output\n");
	printf("  -?    give this help list\n");
}

void	print_short_usage(void)
{
	printf("Usage: ft_ping [-?v] [--help] [--usage] [--verbose] \n	       HOST ...\n");
}

int parse_flag(char av, t_opts *opts)
{
	if(av == 'v')
		opts->verbose = 1;
	else if(av == '?')
		opts->help = 1;
	else
	{
		fprintf(stderr, "ft_ping: invalid option -- '%c'\n", av);
		return (1);
	}
	return (0);
}

int parse_option(char *av, t_opts *opts)
{
	int	i;
	i = 1;
	while (av[i])
	{
		if(parse_flag(av[i], opts))
			return 1;
		i++;
	}
	return 0;
}

int	parse_long(char *arg, t_opts *opts)
{
	if (!strcmp(arg, "--help"))
		opts->help = 1;
	else if (!strcmp(arg, "--usage"))
		opts->usage = 1;
	else if (!strcmp(arg, "--verbose"))
		opts->verbose = 1;
	else
	{
		fprintf(stderr, "ft_ping: unrecognized option '%s'\n", arg);
		return (1);
	}
	return (0);
}

int	parse_arg(char *arg, t_opts *opts)
{
	if (arg[1] == '-')
		return (parse_long(arg, opts));
	return (parse_option(arg, opts));
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
		if(av[i][0] == '-' && av[i][1])
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