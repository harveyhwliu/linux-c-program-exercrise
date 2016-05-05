#include "common.h"
/*
sigaction是linux提供的函数接口
signal是c标准提供的函数接口，可以在linux，windows下通用
	 typedef void (*sighandler_t)(int);
	 sighandler_t signal(int signum, sighandler_t handler);
signal:优点简单，可以跨平台，缺点是没有sigaction功能强大，不能在捕捉时屏蔽其他信号等
*/

void do_sig(int num){
	printf("do_sig:%d\n",num);
}


int main(){
	//直接对信号进行扑捉
	signal(SIGINT,do_sig);

	while(1){
		sleep(1);
	}

	return 0;
}
