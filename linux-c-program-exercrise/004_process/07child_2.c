#include "common.h"
//父进程死掉，子进程没退出则成为孤儿进程，被1号进程init领养
int main()
{
	pid_t pid = fork();
	int nIndex = 10;
	if(0==pid){//子进程
		while(nIndex--){
			printf("子进程:%d,父进程：%d\n",getpid(),getppid());
			sleep(1);
		}
	}
	else if(0<pid){//父进程
		printf("父进程:%d\n",getpid());
	}
	else{
		perror("fork 失败！：");
		exit(-1);
	}

	return 0;
}
