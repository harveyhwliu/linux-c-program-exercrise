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
	char buffer[BUF_SIZE];
	int nReturnValueTmp;
	if(2>argc){
		printf("need a fifo!\n");
		exit(-1);
	}
	//从fifo中读数据
	fd=open(argv[1],O_RDONLY);
	if(-1==fd)
		sys_error("open failed:",-2);
	bzero(buffer,sizeof(buffer));
	nReturnValueTmp=read(fd,buffer,sizeof(buffer));
	if(-1==nReturnValueTmp){
		close(fd);
		sys_error("read failed:",-3);
	}
	printf("read datas:%s\n",buffer);

	close(fd);
	return 0;
}
