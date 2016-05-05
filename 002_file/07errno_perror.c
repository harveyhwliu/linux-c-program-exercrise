#include "common.h"


int main(int argc,const char* argv[])
{
	int fd = open("errorFile",O_RDWR);
	if( 0>fd )
	{
		printf("errno=%d\t,",errno);
		//perror("打开文件失败.\n");
		printf("%s\n",strerror(errno));
	}
	printf("fd=%d\n",fd);
	return 0;
}
