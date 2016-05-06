#include "common.h"

int main(int argc,const char* argv[])
{
	int fd[2];//fd[0]是管道读端，fd[1]是管道写端
	int flags;//设置管道读端非阻塞
	int len;//读到数据的长度
	pid_t pid;//用于区分父子进程
	char buffer[1024]="HELLO,I am learning IPC.1234567890123456789012345678901234567890";
	//建立管道，pipe(fd)，其中fd是返回值
	if(pipe(fd)){
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
		//关闭文件描述符
		close(fd[1]);
		//等待子进程退出
		wait(NULL);
	}
	else if(0==pid){//子进程关闭管道写端
		printf("sun:\n");
		close(fd[1]);
		bzero(buffer,sizeof(buffer));
	//	printf("清空接收缓冲区：%s\n",buffer);
		//设置子进程的管道是非阻塞的方式
		flags = fcntl(fd[0],F_GETFL);
		flags |=O_NONBLOCK;
		flags = fcntl(fd[0],F_SETFL,flags);
		//子进程读数据
tryagain:
		len = read(fd[0],buffer,sizeof(buffer));
		if(-1==len){//出错了
			if(errno == EAGAIN){//当读取一个非阻塞的文件，当数据没到达，会出现EAGAIN的错误码。
				write(STDOUT_FILENO,"try again!\n",strlen("try again!")+1);
				goto tryagain;
			}
			else{
				perror("fcntl() failed:");
				exit(-2);
			}
		}
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
