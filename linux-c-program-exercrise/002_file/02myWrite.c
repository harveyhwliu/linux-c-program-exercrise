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
	int fd = open(argv[1],O_RDWR | O_APPEND,0777);// 默认是覆盖，这里用追加内容
	if( -1 == 0)
	{
		printf("文件打开失败！\n");
		return -1;
	}
	
	char buffer[] = "以指定的追加方式写文件内容！\n";
	int nRes = write(fd,buffer,strlen(buffer));
	if(-1 == nRes)
	{
		printf("文件写失败\n");
		close(fd);
		exit(-3);
	}
	
	close(fd);//close file
	return 0;
}
