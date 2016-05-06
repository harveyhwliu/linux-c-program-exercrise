#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
int main(int argc,const char* argv[])
{
	if(argc<2)
	{
		printf("main中文件名不为空\n");
		exit(-2);
	}
	int fd = open(argv[1],O_RDONLY);// 默认是覆盖，这里用追加内容
	if( -1 == 0)
	{
		printf("文件打开失败！\n");
		return -1;
	}
	
	char buffer[4096]="";
	int nRes = read(fd,buffer,100);
	if(-1 == nRes)
	{
		printf("文件读失败\n");
		close(fd);
		exit(-3);
	}
	printf("%s\n",buffer);
	close(fd);//close file
	return 0;
}
