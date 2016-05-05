#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
int main(int argc,const char* argv[])
{
	if(argc<2)
	{
		printf("main中文件名不为空\n");
		exit(-2);
	}
	int fd = open(argv[1],O_CREAT,0777);// and ~umask
	if( -1 == 0)
	{
		printf("文件创建失败！\n");
		return -1;
	}
	
	printf("文件创建OK！，返回的文件描述符为:fd=%d\n",fd);
	close(fd);//close file
	return 0;
}
