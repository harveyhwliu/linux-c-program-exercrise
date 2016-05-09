#include "common.h"

/*
名字与地址转换
过时，仅用于IPv4，线程不安全
gethostbyname

*/


extern int h_errno;


int main(int argc, char *argv[])
{
	struct hostent *host;
	char str[128];
	if( argc<2 | !argv[1] ){
		printf("请输入主机名!\n");
		return -1;
	}
		
	host = gethostbyname(argv[1]);
	printf("official name of host:%s\n", host->h_name);
	
	while (*(host->h_aliases) != NULL)
		printf("alias name of host--:%s\n", *host->h_aliases++);

	switch (host->h_addrtype) {
		case AF_INET:
			while (*(host->h_addr_list) != NULL)
				printf("addresses:---%s\n", inet_ntop(AF_INET, (*host->h_addr_list++), str, sizeof(str)));
			break;
		default:
		printf("unknown address type\n");
		break;
	}
	return 0;
}
