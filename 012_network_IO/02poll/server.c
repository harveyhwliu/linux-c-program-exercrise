#include "common.h"
/* 多路IO转接模型 -- poll 最大转接个数为 1024（ulimit -a open file最大打开文件个数）
#include <poll.h>
int poll(struct pollfd *fds, nfds_t nfds, int timeout);
ppoll模型可以设置临时屏蔽字
int ppoll(struct pollfd *fds, nfds_t nfds,const struct timespec *timeout_ts, const sigset_t *sigmask);
 struct pollfd {
int   fd;          file descriptor 
short events;      requested events 
short revents;     returned events 
					                 };
*/
#define MAXLINE 80
#define SERVER_PORT 8123
#define MAX_OPEN_FILE_NUM 1024  //最大打开的文件个数
int main(){
	int i,maxi,nready,listenfd,connfd,sockfd;//maxi是client数组的最大下标值
	ssize_t n;		//从客户端读取的数据长度
	char buf[MAXLINE],str[INET_ADDRSTRLEN];//#define INET_ADDRSTRLEN 16
	socklen_t clientaddr_len;
	struct sockaddr_in clientaddr,serveraddr;
	struct pollfd client[MAX_OPEN_FILE_NUM];//poll最大监控的socket个数是最大打开文件描述符个数，ulimit -a 
	//数据初始化
	maxi = -1;		//被监控的客户端数组最大索引
	for(i = 0;i < MAX_OPEN_FILE_NUM; i++)//连接上的客户端文件描述符初始化 为 -1
		client[i].fd= -1;//fd=-1,poll不对齐进行监控
	//1、创建监听socket
	listenfd=socket(AF_INET,SOCK_STREAM,0);
	
	//2、绑定服务器地址
	bzero(&serveraddr,sizeof(struct sockaddr_in));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVER_PORT);
	//3、绑定监听socket
	bind(listenfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
	//4、监听listen
	listen(listenfd,28);//最大监听128
	//5、将监听socket放入到poll中监控
	client[0].fd = listenfd;
	client[0].events = POLLIN;//关心读数据事件
	maxi = 0;

	//服务器业务逻辑
	while(1){
		nready = poll(client,maxi+1,-1);//-1表示阻塞的等待
		if(nready<0){//出错
			perror("poll:");
			exit(-1);
		}
		//是否有新的客户端连接
		if(client[0].revents & POLLIN){
			clientaddr_len = sizeof(clientaddr);//客户端地址长度，传入传出参数
			//接收新的客户端连接
			connfd = accept(listenfd,(struct sockaddr*)&clientaddr,&clientaddr_len);
			//打印客户端连接的信息
			printf("received from %s : %d\n",inet_ntop(AF_INET,&clientaddr.sin_addr,str,sizeof(str)),ntohs(clientaddr.sin_port));
			//将客户端连接 保存到本地client数组中
			for( i = 0; i < MAX_OPEN_FILE_NUM; i++){
				if(client[i].fd<0){//找到client数组中从前面数空闲的位置
					client[i].fd = connfd;
					client[i].events = POLLIN;
					break;
				}
			}
			//达到最大的监控个数ulimit -a open file = 1024时退出程序
			if( i == MAX_OPEN_FILE_NUM){
				fputs("too many clients(>1024)\n",stderr);
				exit(-2);
			}
			//更新client[]最大下标值
			if(i>maxi)	
				maxi = i;
			if(!(--nready))//如果当前只有1个listenfd上有accept时，处理完客户端的连接后应该返回select出继续阻塞
				continue;
		}
		//如果存在客户端的socket上的数据请求时，遍历client数组查找哪一个client上的信号
		for( i = 0; i<=maxi; i++ ){
		printf("maix = %d,client[%d]=%d\n",maxi,i,client[i].fd);
			if( 0 > (sockfd = client[i].fd) )//如果当前client[]对应位置不存在客户端，就跳到下一个client
				continue;
			if(client[i].revents&(POLLIN | POLLERR)){//如果找到那个客户端上发来的数据，则读取数据
				bzero(buf,sizeof(buf));
				if( !( n = read(sockfd,buf,MAXLINE) ) ){//如果客户端关闭，服务器端对应的也要关闭链接，并清除select的监听
					printf("client[%d] closed connection\n",i);
					close(sockfd);
					client[i].fd = -1;
				}
				else if(n<0){
					if(errno == ECONNRESET){/*当收到RST标志时*/
						//connecion reset by client
						printf("client[%d] aborted connection\n",i);
						close(sockfd);
						client[i].fd = -1;
					}else{
						perror("read");
						exit(-3);
					}
						
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
	for(i =0; i < maxi;i++){
		close(client[i].fd);
	}
	return 0;
}
