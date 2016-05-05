#include "common.h"

int main(int argc,const char* argv[])
{
	int fd[2];//fd[0]是管道读端，fd[1]是管道写端
	pid_t pid;//用于区分父子进程
	char buffer[1024]="HELLO,I am learning IPC.1234567890123456789012345678901234567890";
	//建立管道，pipe(fd)，其中fd是返回值
	if(pipe(fd))
	{
		perror("pipe failed!:");
		exit(-1);
	}
	//创建子进程，父写子读
	if((pid = fork())>0){//父进程则需要关闭父进程的管道的读端
		printf("father:\n");
		close(fd[0]);
		sleep(2);
		
		//父进程向管道中写数据
		write(fd[1],buffer,strlen(buffer)+1);
		//等待子进程退出
		//关闭文件描述符
		close(fd[1]);
		wait(NULL);
	}
	else if(0==pid){//子进程关闭管道写端
		printf("sun:\n");
		close(fd[1]);
		
		bzero(buffer,sizeof(buffer));
		printf("清空接收缓冲区：%s\n",buffer);
		//子进程读数据
		
		int len = read(fd[0],buffer,sizeof(buffer));
		//关闭文件描述符
		close(fd[0]);
		//格式化数据
		char dealReadData[1048];
		bzero(dealReadData,sizeof(dealReadData));
		printf("%s\n",buffer);
		sprintf(dealReadData,"Child Read Datas:%s\n",buffer);
		
		//将读到的数据写入屏幕中
		write(STDOUT_FILENO,dealReadData,strlen(dealReadData)+1);
		printf("\n\n");
		exit(0);
	}
	else{
		perror("fork failed!:");
		exit(-2);
	}



	return 0;
}
