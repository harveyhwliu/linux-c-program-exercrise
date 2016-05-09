#include "common.h"
/*
224.0.0.0～224.0.0.255为预留的组播地址（永久组地址），地址224.0.0.0保留不做分配，其它地址供路由协议使用；
224.0.1.0～224.0.1.255是公用组播地址，可以用于Internet；
224.0.2.0～238.255.255.255为用户可用的组播地址（临时组地址），全网范围内有效；
239.0.0.0～239.255.255.255为本地管理组播地址，仅在特定的本地范围内有效。

truct ip_mreqn {
struct in_addr imr_multiaddr; //IP multicast group address 
struct in_addr imr_address;   // IP address of local interface 
int            imr_ifindex;   // interface index
};

*/

#define SERVER_PORT (8123) 	//服务器端口号
#define CLIENT_PORT (8124)	//客户端端口号
#define MAXLINE (1500)		//读入用户输入的内容，每一行最多存放的内容，和下面的定义一样
#define BUF_SIZE (1024)		//根据MTU，最大1500字节
#define GROUP ("239.0.0.3")	//本地管理组播地址，仅在特定的本地范围内有效


//出错原因
void sys_error(const char* des,int errno){
	perror(des);
	exit(errno);
}


int main(int argc,const char* argv[]){

	struct sockaddr_in serveraddr,clientaddr;//服务器、客户端地址
	int sockfd;			
	struct ip_mreqn group;//为了加入一个多播组，需要ip_mreqn的结构体  man 7 ip
	char readBuf[BUF_SIZE]; //ipstr[INET_ADDRSTRLEN] INET_ADDRSTRLEN = 16bytes
	
	//1、socket
	sockfd = socket(AF_INET,SOCK_DGRAM,0);//AF_INET=IPV4(网络层)，TCP(传输层),0表示TCP协议默认的(数据链路层)
	//2、bind
	bzero(&serveraddr,sizeof(struct sockaddr_in));
	//地址族协议IPV4
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);//转成网络字节序 INADDR_ANY=0 本地任意IP 
	serveraddr.sin_port = htons(SERVER_PORT);
	
	bind(sockfd,(struct sockaddr*)&serveraddr,sizeof(struct sockaddr_in));
	//3、设置组地址--全部都是网络字节序
	inet_pton(AF_INET,GROUP,&(group.imr_multiaddr));//设置组地址
	inet_pton(AF_INET,"0.0.0.0",&(group.imr_address));//本地任意IP 相当于INADDR_ANY，让系统自动选择 
	/*查看网卡的编号就是interface index 命令 ip ad  ，函数if_nametoindex*/
	group.imr_ifindex = if_nametoindex("eth0");
	/*设置socket的广播属性*/
	setsockopt(sockfd,IPPROTO_IP,IP_MULTICAST_IF,&group,sizeof(group));//设置IPPROTO_IP=IP层的IP_MULTICAST本地多播设备值为group

	/*构造客户端的地址  IP+端口号*/
	bzero(&clientaddr,sizeof(clientaddr));
	clientaddr.sin_family = AF_INET;//ipv4
	inet_pton(AF_INET,GROUP,&(clientaddr.sin_addr.s_addr));//为客户端IP设置组
	clientaddr.sin_port = htons(CLIENT_PORT);

	while(1){
		bzero(readBuf,sizeof(readBuf));
		//读取用户输入的内容并发送到组播中
		fgets(readBuf,sizeof(readBuf),stdin);
		//sendto  这里的sockfd是全双工的
		sendto(sockfd,readBuf,strlen(readBuf),0,(struct sockaddr*)&clientaddr,sizeof(clientaddr));
		sleep(1);
	}
	
	close(sockfd);
	
	return 0;
}
