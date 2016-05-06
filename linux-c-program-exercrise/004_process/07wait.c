#include "common.h"


int main()
{
//当子进程退出后，父进程没有接收到子进程退出状态，即父进程没有回收子进程的PCB，导致子进程为僵尸进程，状态为Z（zombie）
	pid_t pid = fork();
	if( pid >0 ){
		while(1){
			printf("父进程\n");
			sleep(1);
		}
	}
	else if( 0 == pid )
	{
		printf("子进程\n");
		sleep(10);
	}
	else
	{
		perror("fork failed!-");
		exit(-1);
	}


	return 0;
}
