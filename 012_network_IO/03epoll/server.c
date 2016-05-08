#include "common.h"
/* 多路IO转接模型 -- epoll 最大转接个数为 最大打开文件描述符的个数 （cat /proc/sys/fd/file-max系统最大打开文件个数）
修改资源文件的限制--/etc/security/limits.conf
//1、创建爱你一个epoll句柄（epoll内部存储数据为tree），size告诉内核监听的数目
int epoll_create(int size);
//2、控制某个epoll监控的文件描述符上的时间：注册、修改、删除
int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
//3、等待锁监控文件描述符上有事件的产生，类似于select  timeout = -1表示阻塞，0为立即返回，>0为指定毫秒
int epoll_wait(int epfd, struct epoll_event *events,int maxevents, int timeout);
int epoll_pwait(int epfd, struct epoll_event *events,int maxevents, int timeout,const sigset_t *sigmask);

*/

#define MAXLINE 80
#define SERVER_PORT 8123
#define MAX_OPEN_FILE_NUM 4096  //最大打开的文件个数
//出错处理函数
void sys_error( const char* des,int exitNum ){
	perror(des);
	exit(exitNum);
}

int main(){
	int i,j,maxi,nready,listenfd,connfd,sockfd,epollfd,client[MAX_OPEN_FILE_NUM]; 	  //client数组记录连接的客户端文件描述符
	ssize_t n;				  //从客户端读取的数据长度
	char buf[MAXLINE],str[INET_ADDRSTRLEN];   //#define INET_ADDRSTRLEN 16
	socklen_t clientaddr_len;		  //客户端地址长度
	struct sockaddr_in clientaddr,serveraddr; //客户端、服务器地址
	struct epoll_event set_epoll_event,epoll_return_client[MAX_OPEN_FILE_NUM];
	//set_epoll_event用于每次调用epoll_ctl时设置文件描述符上事件等属性
	//epoll_return_client数组记录返回了epoll监控的有事件发生
	//epoll最大监控的socket个数是最大打开文件描述符个数,可以修改系统资源文件的
	
	//数据初始化
	maxi = -1;
	for(i = 0;i < MAX_OPEN_FILE_NUM; i++){//连接上的客户端文件描述符初始化 为 -1
		bzero(&epoll_return_client[i],sizeof(epoll_return_client[i]));
		client[i]= -1;
	}
	//1、创建监听socket、创建epoll
	listenfd=socket(AF_INET,SOCK_STREAM,0);
	epollfd = epoll_create(MAX_OPEN_FILE_NUM);
	if(epollfd < 0)
		sys_error("epoll_create():",-1);
	//2、绑定服务器地址
	bzero(&serveraddr,sizeof(struct sockaddr_in));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVER_PORT);
	//3、绑定监听socket
	bind(listenfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
	//4、监听listen
	listen(listenfd,28);//最大监听128
	//5、将监听socket放入到epoll中监控
	set_epoll_event.events = EPOLLIN;
	set_epoll_event.data.fd = listenfd;
	if( -1 == epoll_ctl(epollfd,EPOLL_CTL_ADD,listenfd,&set_epoll_event) )
		sys_error("epoll_ctl():",-2);
	//服务器业务逻辑
	while(1){
		nready = epoll_wait(epollfd,epoll_return_client,MAX_OPEN_FILE_NUM,-1);//-1表示阻塞的等待
		if(nready<0)//出错
			sys_error("epoll_wait():",-3);
		//是否有新的客户端连接
		for(i = 0; i < nready; i++){
			//这里仅处理客户端是读数据事件
			//if( !(epoll_return_client[i].events & EPOLLIN) )
			//	continue;
			if( epoll_return_client[i].data.fd == listenfd ){
				clientaddr_len = sizeof(clientaddr);//客户端地址长度，传入传出参数
				//接收新的客户端连接
				connfd = accept(listenfd,(struct sockaddr*)&clientaddr,&clientaddr_len);
				//打印客户端连接的信息
				printf("received from %s : %d\n",inet_ntop(AF_INET,&clientaddr.sin_addr,str,sizeof(str)),ntohs(clientaddr.sin_port));
				//本地保存连接的客户端文件描述符
				for(j = 0; j < MAX_OPEN_FILE_NUM;j++){
					if(client[j]<0){
						client[j] = connfd;
						printf("new client:client[%d]\n",j);
						break;
					}
				}
				if(j == MAX_OPEN_FILE_NUM)
					sys_error("too many clients:",-4);
				if( j > maxi)
					maxi = j;
				//将新的客户端连接放入epoll中
				set_epoll_event.events = EPOLLIN;
				set_epoll_event.data.fd = connfd;
				if( 0 > epoll_ctl(epollfd,EPOLL_CTL_ADD,connfd,&set_epoll_event) )
					sys_error("epoll_ctl():connfd-",-4);	
			}else{
				if(epoll_return_client[i].events & EPOLLIN){//如果找到那个客户端上发来的数据，则读取数据
					bzero(buf,sizeof(buf));
					sockfd = epoll_return_client[i].data.fd;
					if( !( n = read(sockfd,buf,MAXLINE) ) ){
					//如果客户端关闭，服务器端对应的也要关闭链接，并清除epoll的监听
						//将客户端的文件描述符从epoll中移除
						if( -1 == epoll_ctl(epollfd,EPOLL_CTL_DEL,sockfd,NULL) )
							sys_error("epoll_clt():EPOLL_CTL_DEL-",-5);
						//在关闭客户端
						for( j=0; j <=maxi; j++ ){
							if(client[j] == sockfd ){
								printf("client[%d] closed connection\n",j);
								close(client[j]);
								client[j] = -1;
							}
						}
					}
					else if(n<0){
						if(errno == ECONNRESET){/*当收到RST标志时*/
							//connecion reset by client
							printf("client[%d] aborted connection\n",i);
							close(client[i]);
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
				}
			}
		}
	}
	for(i =0; i < MAX_OPEN_FILE_NUM;i++){
		close(client[i]);
		client[i] = -1;
	}
	return 0;
}
