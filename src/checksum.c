#include "ft_ping.h"

unsigned short	checksum(void *data, int len)
{
	unsigned short	*ptr;
	unsigned long	sum;

	ptr = data;
	sum = 0;
	while (len > 1)
	{
		sum += *ptr++;
		len -= 2;
	}
	if (len > 0)
		sum += *(unsigned char *)ptr;
	sum = (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);
	return ((unsigned short)~sum);
}