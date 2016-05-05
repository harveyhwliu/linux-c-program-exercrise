#include "common.h"
/*
	session:会话，服务器程序应该是脱离控制终端的，因此linux服务器程序应该都是创建新的会话
	setsid和getsid   ps -ajx
	setsid: 1、调用进程不能是进程组长，该进程变成新会话的首进程，
		2、需要root权限
		3、新会话丢弃了原有的控制终端，新会话没有控制终端
		4、该调用进程是组长进程，则会出错返回
		5、建立新会话，应该是先调用fork,父进程终止后，子进程调用setsid

*/

int main()
{
	pid_t pid;//fork返回值
	pid_t session;//setsid返回值

	//创建新的会话，先调用fork，子进程在调用setsid
	if( 0>(pid=fork())){
		perror("fork():");
		exit(-1);
	}
	else if( !pid ){//创建新会话，父进程终止后，子进程调用setsid
	
	//打印子进程信息
	printf("创建子进程之后:\n");
	printf("child process PID:%d\n",getpid());
	printf("child process Group ID PGID:%d\n",getpgid(0));
	printf("child process Group ID PGID:%d\n",getpgrp());
	printf("child process Session ID SID:%d\n",getsid(0));
	
	sleep(5);//等待父进程死掉，子进程调用setsid创建新会话
	session = setsid();//子进程非租场进程，故其成为新会话首进程，且为租场进程，该进程ID即为会话id,进程组id
	if( -1 == session ){
		close(pid);
		perror("setsid():");
		exit(-2);
	}
	
	printf("建立新会话之后：\n");
	printf("child process PID:%d\n",getpid());
	printf("child process Group ID PGID:%d\n",getpgid(0));
	printf("child process Session DI SID:%d\n",getsid(0));
	sleep(3);
}

	close(pid);
	close(session);
	return 0;
}
