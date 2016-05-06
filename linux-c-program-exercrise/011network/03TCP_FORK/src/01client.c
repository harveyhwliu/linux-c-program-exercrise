#include "common.h"
#define BUF_SIZE (8*1024)

int main(int argc,const char* argv[]){

	int connfd,len;
	struct sockaddr_in serveraddr;
	char client[]="10.42.1.144";
	char buf[BUF_SIZE];
	
	//创建客户端socket
	connfd = socket(AF_INET,SOCK_STREAM,0);
	//服务器地址
	bzero(&serveraddr,sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	inet_pton(AF_INET,client,&serveraddr.sin_addr.s_addr);
	serveraddr.sin_port = htons(8123);
	//连接服务器
	connect(connfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
	while(fgets(buf,sizeof(buf),stdin)){
		//write
		write(connfd,buf,strlen(buf) );
		bzero(buf,sizeof(buf));
		//read
		len = read(connfd,buf,sizeof(buf));
		write(STDOUT_FILENO,buf,len);
		bzero(buf,sizeof(buf));
	}
	//关闭socket
	close(connfd);
	return 0;

}
