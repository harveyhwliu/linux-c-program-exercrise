#include "common.h"

int main(int argc,const char* argv[])
{
	if(1>argc)
		exit(-1);
	int fd = open(argv[1],O_RDWR);
	if( 0 > fd )
	{
		perror("no file exists!\n");
		close(fd);
		exit(-2);
	}
//1、lseek可以用于扩展文件内容，其后面必须紧跟write操作	
	lseek(fd,0x1000,SEEK_SET);
	if(-1 == write(fd,"hello",strlen("hello")+1))
	{
		perror("write failed!\n");
		close(fd);
		exit(-3);
	}	
	close(fd);

//2、lseek可以用来获得一个文件的大小
	int fd_read_file_size = open("abc",O_RDONLY);
	if(fd_read_file_size < 0)
	{
		perror("文件：abc打开失败！");
		exit(-4);
	}
	printf("文件：abc的大小为：%d\n",lseek(fd_read_file_size,0,SEEK_END));
	close(fd_read_file_size);

	return 0;
}
