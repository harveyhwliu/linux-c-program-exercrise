#include "common.h"
#define SERVER_PORT (8123)
#define BUF_SIZE (8*1024)
//出错原因
void sys_error(const char* des,int errno){
	perror(des);
	exit(errno);
}


int main(int argc,const char* argv[]){

	struct sockaddr_in serveraddr,clientaddr;//IPV4
	int sockfd,clientaddrlen,confd;
	char clientip[128];
	char readBuf[BUF_SIZE];
	int i,readlen;
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
