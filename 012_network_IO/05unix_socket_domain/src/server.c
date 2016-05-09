#include "common.h"
/*
socket API原本是为网络通讯设计的，但后来在socket的框架上发展出一种IPC机
制，就是UNIX Domain Socket。虽然网络socket也可用于同一台主机的进程间通讯（通过
loopback地址127.0.0.1），但是UNIX Domain Socket用于IPC更有效率：不需要经过网络协
议栈，不需要打包拆包、计算校验和、维护序号和应答等，只是将应用层数据从一个进程拷
贝到另一个进程。这是因为，IPC机制本质上是可靠的通讯，而网络协议是为不可靠的通讯
设计的。UNIX Domain Socket也提供面向流和面向数据包两种API接口，类似于TCP和UDP，
但是面向消息的UNIX Domain Socket也是可靠的，消息既不会丢失也不会顺序错乱。
*/
#define QLEN 10

//使用unix domain socket创建连接服务器端
//name:unix domain socket绑定的文件名
//返回值：成功返回监听socket，失败则返回错误码
int serv_listen(const char *name)
{
	int fd, len, err, rval;//fd是监听socket,len是初始化socket地址的长度、err临时保存错误码、rval保存失败时的错误码
	struct sockaddr_un un;	//unix domain socket的地址
	//1、创建1个unix domain 流式套接字 socket
	if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
		return(-1);
	//如果文件存在，删除已经存在的文件
	unlink(name);
	//初始化socket地址
	memset(&un, 0, sizeof(un));
	un.sun_family = AF_UNIX;
	strcpy(un.sun_path, name);
	len = offsetof(struct sockaddr_un, sun_path) + strlen(name);//offsetof结构体成员的偏移量
	//2、绑定文件描述符
	if (bind(fd, (struct sockaddr *)&un, len) < 0) {
		rval = -2;  //失败时错误码
		goto errout;//失败处理
	}
	//3、监听
	if (listen(fd, QLEN) < 0) {
		rval = -3;
		goto errout;
	}
	return(fd);//成功返回监听socket,失败返回错误码
errout://错误处理
	err = errno;
	close(fd);
	errno = err;
	return(rval);
}

int serv_accept(int listenfd, uid_t *uidptr)
{
	int clifd, err, rval;
	//time_t staletime;
	struct sockaddr_un un;
	struct stat statbuf;
	socklen_t len;
	len = sizeof(un);
	//接受客户端连接
	if ((clifd = accept(listenfd, (struct sockaddr *)&un, &len)) < 0)
		return(-1); /* often errno=EINTR, if signal caught */
	//获得客户端的uid
	len -= offsetof(struct sockaddr_un, sun_path); /* len of pathname */
	un.sun_path[len] = 0; /* null terminate */

	if (stat(un.sun_path, &statbuf) < 0) {//获得文件状态
		rval = -2;
		goto errout;
	}
	if (S_ISSOCK(statbuf.st_mode) == 0) {//是否是socket
		rval = -3; 
		goto errout;
	}
	
	if (uidptr != NULL)
		*uidptr = statbuf.st_uid; /* return uid of caller */
	unlink(un.sun_path); /* we're done with pathname now */
	return(clifd);
errout:
	err = errno;
	close(clifd);
	errno = err;
	return(rval);
}

int main(void)
{
	int lfd, cfd, n, i;
	uid_t cuid;
	char buf[1024];
	//建立监听的unix domain socket
	lfd = serv_listen("foo.socket");
	//出错处理
	if (lfd < 0) {
		switch (lfd) {
		case -3:perror("listen"); break;
		case -2:perror("bind"); break;
		case -1:perror("socket"); break;
		}	
		exit(-1);
	}
	//接收客户端连接
	cfd = serv_accept(lfd, &cuid);
	if (cfd < 0) {
		switch (cfd) {
		case -3:perror("not a socket"); break;
		case -2:perror("a bad filename"); break;
		case -1:perror("accept"); break;
		}
		exit(-1);
	}
	//业务处理
	while (1) {
r_again:
		bzero(buf,sizeof(buf));
		n = read(cfd, buf, 1024);
		if (n == -1) {
		if (errno == EINTR)//如果读数据时有信号中断的错误
			goto r_again;
		}
		else if (n == 0) {//对端关闭连接
			printf("the other side has been closed.\n");
			break;
		}
		printf("received datas:%s\n",buf);
		//对数据进行处理，并发送过去
		for (i = 0; i < n; i++)
			buf[i] = toupper(buf[i]);
		write(cfd, buf, n);
	}
	close(cfd);
	close(lfd);
	return 0;
}

