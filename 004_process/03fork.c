#include "common.h"


int main()
{
//fork调用一次返回两次，返回pid为0表示子进程，返回-1表示fork失败
	pid_t pid = fork();
	if( pid >0 ){
		printf("父进程\n");
		while(1);
//		sleep(1);
	}
	else if( 0 == pid )
	{
		printf("子进程\n");
		while(1);
//		sleep(4);
	}
	else
	{
		perror("fork failed!-");
		exit(-1);
	}


	return 0;
}
