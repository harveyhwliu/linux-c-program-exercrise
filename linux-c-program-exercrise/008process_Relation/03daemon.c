#include "common.h"

//守护进程daemon
void myDaemon(){
	pid_t pid;
	/*
	* 成为一个新会话的首进程，失去控制终端
	*/
	if ((pid = fork()) < 0) {
		perror("fork");
		exit(1);
	} else if (pid != 0) /* parent */
		exit(0);
	setsid();
	/*
	* 改变当前工作目录到/目录下.
	*/
	if (chdir("/") < 0) {
		perror("chdir");
		exit(1);
	}
	/* 设置umask为0 */
	umask(0);
	/*
	* 重定向0，1，2文件描述符到 /dev/null，因为已经失去控制终端，再操作0，1，2没有意义.
	*/
	close(0);//标准输入
	open("/dev/null", O_RDWR);//黑洞文件，重新打开则黑洞文件对应文件描述表中未使用的最小的文件描述符即黑洞文件的文件描述符是0
	dup2(0, 1);//标准输出重定向到黑洞文件
	dup2(0, 2);//标准错误输出重定向到黑洞文件
}

int main(void)
{
	myDaemon();
	while(1); /* 在此循环中可以实现守护进程的核心工作 */
}
