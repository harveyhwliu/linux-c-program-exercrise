#include "common.h"
/*
	目标，利用sigpedding获取未诀信号集，利用sigprocmask设置信号屏蔽字屏蔽ctrl+c,ctrl+z，打印当前的未决信号集
*/

//打印未诀信号集
void printPendSignalSet(sigset_t *sigPending){
	int nIndex = 1,nBlockTemp=0;
	for(;nIndex<33;nIndex++){
		if( sigismember(sigPending,nIndex) )
			printf("1");//putchar('1');
		else
			printf("0");//putchar('0');
		//打印格式，每一个字节中间加一个空格
		if(!(++nBlockTemp%8))
			printf("  ");
	}
	printf("\n");
	//printf("\n%d\n",sizeof(*sigPending));//128字节

}


int main(){
	sigset_t sigPending;//用来获取未决信号的信号集
	sigset_t sigProcMask;//用来屏蔽信号的信号集
	//信号集先初始化
	sigemptyset(&sigPending);
	sigemptyset(&sigProcMask);
	//添加一个信号SIGKILL信号
	sigaddset(&sigPending,SIGKILL);
	//移除一个信号SIGKILL
	sigdelset(&sigPending,SIGKILL);


	//设置要屏蔽的信号
	sigaddset(&sigProcMask,SIGINT);
	sigaddset(&sigProcMask,SIGTSTP);
	sigaddset(&sigProcMask,SIGTERM);//kill 默认发送的是15号信号 SIGTERM终止信号
	
	//现在要屏蔽SIGSTOP,SIGQUIT信号
	sigaddset(&sigProcMask,SIGSTOP);//19号信号不可以被阻塞屏蔽
	sigaddset(&sigProcMask,SIGQUIT);//3号信号ctrl+\可以被阻塞屏蔽
	
	//设置信号屏蔽字,屏蔽ctrl+c,ctrl+z
	sigprocmask(SIG_BLOCK,&sigProcMask,NULL);

	while(1){
		//获取未诀信号集并打印出来
		sigpending(&sigPending);
		printPendSignalSet(&sigPending);
		sleep(1);
	}
	return  0;	
}


