#include "common.h"
//使用pause函数会交出时间片，因此可能出现时序竞态：应该使用sigsuspend函数
void do_sig(int num){}//信号捕捉函数中什么也不做

unsigned int mysleep(unsigned int nSecs){//睡眠多少秒，返回值是未睡够的秒数
	struct sigaction newact,oldact;//表示要设置的、以及设置之前原来的sigaction结构
	sigset_t newmask,oldmask,suspendmask;//设置阻塞SIGALRM信号的信号集、设置之前原来系统的信号集、以及sigsuspend临时的屏蔽信号集
	unsigned int unsleepsec;

	//捕捉信号 SIGALRM信号
	newact.sa_handler = do_sig;
	sigemptyset(&newact.sa_mask);
	newact.sa_flags = 0;
	sigaction(SIGALRM,&newact,&oldact);
	
	//阻塞SIGALRM信号,利用alarm定时器，通过sigsuspend挂起并等待定时结束的SIGALRM信号
	sigemptyset(&newmask);
	sigaddset(&newmask,SIGALRM);
	sigprocmask(SIG_BLOCK,&newmask,&oldmask);//先阻塞SIGALRM信号，防止时序竞态
	
	alarm(nSecs);//进程定时器定时 nSecs秒

	suspendmask = oldmask;
	sigdelset(&suspendmask,SIGALRM);//解除之前阻塞的SIGALRM信号
	sigsuspend(&suspendmask);//在进程挂起的时候临时解除SIGALRM信号的屏蔽，并等待SIGALRM信号

	unsleepsec = alarm(0);

	sigaction(SIGALRM,&oldact,NULL);
	sigprocmask(SIG_SETMASK,&oldmask,NULL);//设置原来的阻塞信号集
	return unsleepsec;
}

int main(){
	int nSec = 2;
	while(1){
		mysleep(nSec);
		printf("%d seconds passed\n",nSec);
	}
	return 0;
}
