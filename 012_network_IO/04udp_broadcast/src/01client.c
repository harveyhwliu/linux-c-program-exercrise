#include "common.h"
#define SERVER_PORT (8123)      //服务器端口号
#define CLIENT_PORT (8124)      //客户端端口号
#define MAXLINE (1500)          //读入用户输入的内容，每一行最多存放的内容，和下面的定义一样
#define BUF_SIZE (1500)         //根据MTU，最大1500字节
#define GROUP ("239.0.0.3")     //本地管理组播地址，仅在特定的本地范围内有效


int main(int argc,const char* argv[]){

	int connfd,len;
	struct sockaddr_in localaddr;//组播需要绑定本地IP+端口号
	char buf[BUF_SIZE];
	//组播结构体
	struct ip_mreqn group;

	//创建客户端socket
	connfd = socket(AF_INET,SOCK_DGRAM,0);
	//本地端地址
	bzero(&localaddr,sizeof(localaddr));
	localaddr.sin_family = AF_INET;
	inet_pton(AF_INET,"0.0.0.0",&localaddr.sin_addr.s_addr);//本地IP任意
	localaddr.sin_port = htons(CLIENT_PORT);
	//组播 绑定本地IP地址
	bind(connfd,(struct sockaddr*)&localaddr,sizeof(localaddr));


	/*设置组地址*/
	inet_pton(AF_INET,GROUP,&group.imr_multiaddr);
	/*本地任意IP*/
	inet_pton(AF_INET,"0.0.0.0",&group.imr_address);
	/*ip ad    获取网卡的interface index if_nametoindex*/
	group.imr_ifindex = if_nametoindex("eth0");
	
	/*设置客户端加入组播中*/
	setsockopt(connfd,IPPROTO_IP,IP_ADD_MEMBERSHIP,&group,sizeof(group));

	while(1){
		bzero(buf,sizeof(buf));
		//recvfrom
		len = recvfrom(connfd,buf,sizeof(buf),0,NULL,0);
		write(STDOUT_FILENO,buf,len);
	}
	//关闭socket
	close(connfd);
	return 0;

}
