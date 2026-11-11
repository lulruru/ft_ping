#ifndef FT_PING_H
# define FT_PING_H

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>
# include <errno.h>
# include <signal.h>
# include <sys/select.h>
# include <sys/socket.h>
# include <sys/time.h>
# include <netinet/in.h>
# include <netinet/ip.h> 
# include <netinet/ip_icmp.h>
# include <arpa/inet.h>
# include <netdb.h> 

typedef struct ft_ping
{
	int verbose;
	int help;
	int usage;
	char *host;

} t_opts;


// PARSING.C
int 	parse_args(int ac, char **av, t_opts *opts);
void	print_usage(void);
void	print_short_usage(void);

// UTILS.C


#endif