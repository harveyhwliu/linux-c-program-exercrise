#include "common.h"
#define SERVER_PORT (8123)
#define BUF_SIZE (8*1024)
//出错原因
void sys_error(const char* des,int errno){
	perror(des);
	exit(errno);
}
//线程函数
void* threadFunc(void* args){
	printf("threadFunc:调用wait\n");
	wait(NULL);
	printf("回收子进程资源线程退出！\n");
	return (void*)0;
}
int main(int argc,const char* argv[]){

	struct sockaddr_in serveraddr,clientaddr;//IPV4
	int sockfd,i,readlen,confd;
	char clientip[128], readBuf[BUF_SIZE];
	pid_t pid;
	socklen_t clientaddrlen;
	pthread_t tid;
 	pthread_attr_t attr;
	
	//socket
	sockfd = socket(AF_INET,SOCK_STREAM,0);//AF_INET=IPV4(网络层)，TCP(传输层),0表示TCP协议默认的(数据链路层)
	//bind
	bzero(&serveraddr,sizeof(struct sockaddr_in));
	//地址族协议IPV4
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);//转成网络字节序
	serveraddr.sin_port = htons(SERVER_PORT);
	
	bind(sockfd,(struct sockaddr*)&serveraddr,sizeof(struct sockaddr_in));
	//listen
	listen(sockfd,128);//默认最大IPV4 连接数是128

	while(1){
	//accept
		clientaddrlen = sizeof(clientaddr);//传入传出参数
		confd = accept(sockfd,(struct sockaddr *)&clientaddr,&clientaddrlen);
		
		//打印连接的客户端IP地址和端口号
		bzero(clientip,sizeof(clientip));
		inet_ntop(AF_INET,&(clientaddr.sin_addr.s_addr),clientip,sizeof(clientip));

		printf("client IP:%s\t,port:%d\n",inet_ntop(AF_INET,&(clientaddr.sin_addr.s_addr),clientip,sizeof(clientip)),ntohs(clientaddr.sin_port));
		//采用多进程模型，提高服务器端的并发性能，多进程并发模型，浪费资源多但是性能稳定
		pid = fork();
		if( !pid ){//子进程
			close(sockfd);//子进程用不到sockfd，应该关闭
			while(1){
				//read
				bzero(readBuf,sizeof(readBuf));
				readlen =read(confd,readBuf,sizeof(readBuf));
				i=0;
				while( i<readlen ){
					readBuf[i]=toupper(readBuf[i]);
					i++;
				}
				//write
				write(confd,readBuf,readlen);	
			}
			close(confd);//子进程退出时应该关闭confd
		}
		else if (pid>0){//父进程
			close(confd);//用不到应该关闭，不关闭出现资源浪费，将文件描述符耗尽
			//父进程继续accept
			//设置线程为分离态
			pthread_attr_init(&attr);
			pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
			//开启一个子线程，该子线程用于等待回收子进程的资源
			pthread_create(&tid,&attr,threadFunc,NULL);
			pthread_attr_destroy(&attr);

		}
		else{//出错不退出
			perror("fork:");
			//exit(-1);	
		}
		//read
		bzero(readBuf,sizeof(readBuf));
		readlen =read(confd,readBuf,sizeof(readBuf));
		i=0;
		while( i<readlen ){
			readBuf[i]=toupper(readBuf[i]);
			i++;
		}

		//write
		write(confd,readBuf,readlen);
		close(confd);
	}
	
	close(sockfd);
	
	return 0;
}
