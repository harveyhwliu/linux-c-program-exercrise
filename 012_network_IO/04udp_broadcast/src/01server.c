#include "common.h"
#define SERVER_PORT (8123)
#define BUF_SIZE (1024)//根据MTU，最大1500字节
//出错原因
void sys_error(const char* des,int errno){
	perror(des);
	exit(errno);
}


int main(int argc,const char* argv[]){

	struct sockaddr_in serveraddr,clientaddr;//IPV4
	int sockfd,i,readlen;
	socklen_t clientaddlen;
	char clientip[128];
	char readBuf[BUF_SIZE];
	
	//socket
	sockfd = socket(AF_INET,SOCK_DGRAM,0);//AF_INET=IPV4(网络层)，TCP(传输层),0表示TCP协议默认的(数据链路层)
	//bind
	bzero(&serveraddr,sizeof(struct sockaddr_in));
	//地址族协议IPV4
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);//转成网络字节序
	serveraddr.sin_port = htons(SERVER_PORT);
	
	bind(sockfd,(struct sockaddr*)&serveraddr,sizeof(struct sockaddr_in));

	while(1){
		bzero(readBuf,sizeof(readBuf));
		clientaddlen = sizeof(clientaddr);//传入传出参数，必须初始化
		//recvfrom
		readlen = recvfrom(sockfd,readBuf,BUF_SIZE,0,(struct sockaddr *)&clientaddr,&clientaddlen);

		//打印连接的客户端IP地址和端口号
		bzero(clientip,sizeof(clientip));
		//inet_ntop(AF_INET,&(clientaddr.sin_addr.s_addr),clientip,sizeof(clientip));

		printf("client IP:%s\t,port:%d\n",inet_ntop(AF_INET,&(clientaddr.sin_addr.s_addr),clientip,sizeof(clientip)),ntohs(clientaddr.sin_port));
		i=0;
		while( i<readlen ){
			readBuf[i]=toupper(readBuf[i]);
			i++;
		}
		readBuf[readlen]='\n';
		//sendto  这里的sockfd是全双工的
		sendto(sockfd,readBuf,readlen+1,0,(struct sockaddr*)&clientaddr,sizeof(clientaddr));
	}
	
	close(sockfd);
	
	return 0;
}
