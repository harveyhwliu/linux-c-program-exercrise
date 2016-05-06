#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
int main(int argc,const char* argv[])
{	
	int fd = -1;
	int fileNum = 0;
	char fileName[1024];
	while(1)
	{
		sprintf(fileName,"FileName%d",fileNum);
		umask(0);//只会在本进程中生效，shell中umask的值没变
		fd = open("TestFile",O_CREAT,0777);// 文件属性还要与上~umask
		if( -1 == fd)
		{
			printf("文件打开失败！\n");
			exit(-1);
		}
		printf("%d\n",fd);
	}
	//close(fd);//close file
	return 0;
}
