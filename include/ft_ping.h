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

#define PING_PKT_S 64
#define PORT_NO 0
#define PING_SLEEP_RATE 1000000
#define RECV_TIMEOUT 1
#define DUP_TABLE_SIZE 8192

typedef struct s_opts
{
	int verbose;
	int help;
	int usage;
	char *host;

} t_opts;

typedef struct s_stats
{
	long	transmitted;
	long	received;
	long	duplicates;
	double	rtt_min;
	double	rtt_max;
	double	rtt_sum;
	double	rtt_sumsq;
}	t_stats;

typedef struct s_ping
{
	t_opts				opts;
	t_stats				stats;
	char				canonname[NI_MAXHOST];
	char				ip_str[INET_ADDRSTRLEN];
	struct sockaddr_in	sockaddr;
	int					sockfd;
	unsigned short		id;
	unsigned char		dup_table[DUP_TABLE_SIZE];
}	t_ping;

// PARSING.C
int 			parse_args(int ac, char **av, t_opts *opts);
void			print_usage(void);
void			print_short_usage(void);
int				find_host(t_ping *ping);

// CHECKSUM.C
unsigned short	checksum(unsigned short *addr , int count);

#endif