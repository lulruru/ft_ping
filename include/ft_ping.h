#ifndef FT_PING_H
# define FT_PING_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netinet/ip.h> 
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <netdb.h> 
#include <math.h>

#define PING_PKT_S 64
#define PORT_NO 0
#define PING_SLEEP_RATE 1000000
#define RECV_TIMEOUT 5
#define DUP_TABLE_SIZE 8192
#define ICMP_HDR_LEN 8

#define RECV_BUF_S 1024
#define RECV_GOT 0
#define RECV_SKIP 1
#define RECV_TIMEDOUT 2
#define RECV_FAIL 3

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

extern volatile sig_atomic_t	g_stop;
void	setup_signals(void);

// PARSING.C
int 			parse_args(int ac, char **av, t_opts *opts);
void			print_usage(void);
void			print_short_usage(void);
int				find_host(t_ping *ping);

// CHECKSUM.C
unsigned short	checksum(void *data, int len);
 
// SOCKET.C
int 			init_socket(t_ping *ping);

// PACKET.C
void			init_packet(t_ping *ping, char *sendbuf, int seq);
int				send_packet(t_ping *ping, char *buf);
int				recv_packet(t_ping *ping);

// STATS.C
void			update_stats(t_stats *s, double rtt);
int				is_dup(t_ping *ping, unsigned short seq);

// LOOP.C
void			ping_loop(t_ping *ping);
void			print_summary(t_ping *ping);



#endif