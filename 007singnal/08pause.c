#include "common.h"

void do_sig(int num){
	printf("do_sig:%d\n",num);
}



int main(){
	struct sigaction act;//sigaction信号扑捉
	act.sa_handler = do_sig;
	//act.sa_handler = SIG_IGN;//忽略

	sigemptyset(&act.sa_mask);

	act.sa_flags = 0;

	sigaction(SIGINT,&act,NULL);

	//使用pause，将会将当前进程挂起，直到有信号来到，并按照信号的处理方式处理，如果信号处理是SIG_IGN忽略的，则继续挂起
	pause();


	return 0;
}
