#include "common.h"
/* 多路IO转接模型 -- select 最大转接FD_SETSIZE = 1024个套接字
int select(int nfds, fd_set *readfds, fd_set *writefds,fd_set *exceptfds, struct timeval *timeout);

void FD_CLR(int fd, fd_set *set);//把文件描述符集合里fd清0
int  FD_ISSET(int fd, fd_set *set);//测试文件描述符集合里fd是否为1
void FD_SET(int fd, fd_set *set);//把文件描述符集合里fd位置置1
void FD_ZERO(fd_set *set);//把文件描述符集合里所有的位清0

*/
#define MAXLINE 80
#define SERVER_PORT 8123

int main(){
	int i,maxi,maxfd,listenfd,connfd,sockfd;//maxi是client数组的最大下标值
	int nready,client[FD_SETSIZE];//FD_SETSIZE默认为1024，select最大管理1024个连接，局域网使用
	ssize_t n;		//从客户端读取的数据长度
	fd_set rset,allset;	//读信号集合
	char buf[MAXLINE],str[INET_ADDRSTRLEN];//#define INET_ADDRSTRLEN 16
	socklen_t clientaddr_len;
	struct sockaddr_in clientaddr,serveraddr;
	//数据初始化
	maxi = -1;		//
	for(i = 0;i < FD_SETSIZE; i++)//连接上的客户端文件描述符初始化 为 -1
		client[i]= -1;
	//1、创建监听socket
	listenfd=socket(AF_INET,SOCK_STREAM,0);
	maxfd = listenfd;	//select模型参数需要最大监控文件描述符+1，因此记录最大监听文件描述符
	//将信号全部清空
	FD_ZERO(&allset);//
	FD_SET(listenfd,&allset);//关心监听socket上的accept事件
	
	//2、绑定服务器地址
	bzero(&serveraddr,sizeof(struct sockaddr_in));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVER_PORT);
	//3、绑定监听socket
	bind(listenfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
	//4、监听listen
	listen(listenfd,28);//最大监听128

	//服务器业务逻辑
	while(1){
		rset = allset;//读信号集
		nready = select(maxfd+1,&rset,NULL,NULL,NULL);//select模型，立刻返回读信号集中发生事件通知的个数
		if(nready<0){//出错
			perror("select:");
			exit(-1);
		}
		//是否有新的客户端连接
		if(FD_ISSET(listenfd,&rset)){
			clientaddr_len = sizeof(clientaddr);//客户端地址长度，传入传出参数
			//接收新的客户端连接
			connfd = accept(listenfd,(struct sockaddr*)&clientaddr,&clientaddr_len);
			//打印客户端连接的信息
			printf("received from %s : %d\n",inet_ntop(AF_INET,&clientaddr.sin_addr,str,sizeof(str)),ntohs(clientaddr.sin_port));
			//将客户端连接 保存到本地client数组中
			for( i = 0; i < FD_SETSIZE; i++){
				if(client[i]<0){
					client[i] = connfd;
					break;
				}
			}
			//达到最大的监控个数FD_SETSIZE=1024时退出程序
			if( i == FD_SETSIZE){
				fputs("too many clients(>FD_SETSIZE)\n",stderr);
				exit(-2);
			}
			//将新连接的客户端放入读信号集中，交由select模型监控
			FD_SET(connfd,&allset);
			//更新做大监控的文件描述符，select模型的第一个参数为监控的最大文件描述符+1
			if(connfd > maxfd)
			maxfd = connfd;
			//更新client[]最大下标值
			if(i>maxi)	
				maxi = i;
			if(!(--nready))//如果当前只有1个listenfd上有accept时，处理完客户端的连接后应该返回select出继续阻塞
				continue;
		}
		//如果存在客户端的socket上的数据请求时，遍历client数组查找哪一个client上的信号
		for( i = 0; i<=maxi; i++ ){
		printf("maix = %d,client[%d]=%d\n",maxi,i,client[i]);
			if( 0 > (sockfd = client[i]) )//如果当前client[]对应位置不存在客户端，就跳到下一个client
				continue;
			if(FD_ISSET(sockfd,&rset)){//如果找到那个客户端上发来的数据，则读取数据
				bzero(buf,sizeof(buf));
				if( !( n = read(sockfd,buf,MAXLINE) ) ){//如果客户端关闭，服务器端对应的也要关闭链接，并清除select的监听
					printf("client[%d] closed\n",i);
					close(sockfd);
					FD_CLR(sockfd,&allset);
					client[i] = -1;
				}	
				else{
					int j;
					for(j = 0; j < n; j++ )//将client的数据小写变大写并发送回去
						buf[j] = toupper(buf[j]);
					write(sockfd,buf,n);
				}
				if(!(--nready))//如果已经处理完所有的请求，应该返回select出继续阻塞监听
					break;
			}
		}
	}
	close(listenfd);
	return 0;
}
