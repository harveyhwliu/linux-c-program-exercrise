#include "common.h"

/*
名字与地址转换
过时，仅用于IPv4，线程不安全
gethostbyaddr--此函数智能获取域名解析服务器的url和/etc/hosts里面登记的IP对应的域名

*/


extern int h_errno;


int main(int argc, char *argv[])
{
	struct hostent *host;
	char str[128];
	struct in_addr addr;

	if( argc<2 | !argv[1] ){
		printf("请输入主机IP!\n");
		return -1;
	}
	//现将IP地址转换成addr
	inet_pton(AF_INET, argv[1], &addr);
	host = gethostbyaddr((char*)&addr,4,AF_INET);

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
