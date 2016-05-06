#include "common.h"
#define BUF_SIZE (4*1024)
//自己做一个出错信息函数，重复的代码
void sys_error(const char* reason,int errnu){
	perror(reason);
	exit(errnu);
}

int main(int argc,const char* argv[])
{
	int fd;
	char buffer[BUF_SIZE]="我正在使用fifo有名管道进行IPC，可以在无血缘关闭的进程间通信";
	int nReturnValueTmp;
	if(2>argc){
		printf("need a fifo!\n");
		exit(-1);
	}
	//向管道中写数据
	fd=open(argv[1],O_WRONLY);
	if(-1==fd)
		sys_error("open failed:",-2);
	nReturnValueTmp=write(fd,buffer,strlen(buffer)+1);
	if(-1==nReturnValueTmp){
		close(fd);
		sys_error("write failed:",-3);
	}

	close(fd);
	return 0;
}
