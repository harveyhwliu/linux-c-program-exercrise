#include "common.h"

int main()
{
	//定时器，alarm函数向调用的进程发送SIGALRM信号，每个进程有且只有一个定时器，返回值是未睡够的秒数
	int nReturnValue = alarm(1);//定时1s，当时间到1s后想当前调用的进程发送14号定时器信号SIGALRM,当前程序会运行结束
	//实验：1s，当前计算机能输出多少个数
	int nCount=0;
	while(1){
		printf("%d;",nCount++);
	}
//alarm和sleep的区别在于：alarm是设置当前的进程定时器，当定时器计时够了就会收到SIGALRM信号而结束，同时程序继续往下执行而不会像sleep一样不再往下执行，sleep睡后还是会继续顺序执行程序，进程没有收到信号而终止
	printf("hhhhhhhh--当前程序会受到SIGALRM信号而死掉");

	return 0;
}	
