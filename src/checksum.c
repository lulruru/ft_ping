#include "ft_ping.h"

unsigned short checksum(unsigned short *addr , int count){
	unsigned long sum = 0;

	while(count > 1){
		sum += *addr++;
		count -=2;
	}
	if (count > 0){
		sum += *(unsigned char *)addr;
	}
	sum = (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);
	return (~sum);
}