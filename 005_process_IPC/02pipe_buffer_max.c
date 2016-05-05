#include "common.h"
//测试pipe默认的管道buffer有多大
int main()
{
	int fd[2];
	pipe(fd);

	long lBufferMaxSizeRead = fpathconf(fd[0],_PC_PIPE_BUF);//管道读端的buffer大小
	long lBufferMaxSizeWrite = fpathconf(fd[1],_PC_PIPE_BUF);//管道写端的buffer大小

	//关闭文件描述符
	close(fd[0]);
	close(fd[1]);

	printf("pipe fd[0].buffer:%ld,fd[1].buffer:%ld\n",lBufferMaxSizeRead,lBufferMaxSizeWrite);

	return 0;
}
