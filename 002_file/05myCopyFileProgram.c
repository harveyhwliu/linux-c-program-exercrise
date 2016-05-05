#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define BUF_SIZE (8*1048)

int main(int argc,const char* argv[])
{
	char buffer[BUF_SIZE]="";
	int fd_read_src,fd_write_dest,nReadCount = -1;
	//1、判读能否打开文件
	if(argc<3)
	{
		printf("拷文件程序需要两个参数：源文件、目的文件\n");
		exit(-1);
	}

	//2、打开读、写文件，返回对应的文件描述符
	fd_read_src = open(argv[1],O_RDONLY);
	if(-1 == fd_read_src)
	{
		printf("源文件无法打开，确认源文件存在且有打开文件的权限\n");
		exit(-2);
	}
	umask(0);
	fd_write_dest=open(argv[2],O_CREAT | O_WRONLY | O_TRUNC ,777);
	printf("准备进行文件拷贝\n");
	//3、循环读文件并写入到目的文件中
	while((nReadCount = read(fd_read_src,buffer,sizeof(buffer)))>0)
	{
		write(fd_write_dest,buffer,nReadCount);
		bzero(buffer,BUF_SIZE);
	}
	//4、关闭文件描述符
	close(fd_read_src);
	close(fd_write_dest);

	return 0;
}
