#include "common.h"

//信号的捕捉
/*信号的未决信号集中不支持信号的排队，即当有信号产生后，相应的未诀信号集做记录置1，如何此时再有一个相同信号
出现，未决信号集里面还是1，不会增加；如果阻塞(屏幕)信号集对应的信号标志位被置1（sigprocmask函数设置），则会处理信号（默认是SIG_DFL）
，用户可以通过sigaction函数捕捉相应的信号，并通过struct sigaction字段的sa_handler设置对该信号的处理方式：
SIG_IGN(忽略)，SIG_DFL(默认)，或者指定自己的信号捕捉函数

当前信号如果正在执行信号的捕捉处理函数，则会阻塞当前信号，即对再一次（或多次的）当前信号不会立即处理，而会阻塞，执行完后才会将阻塞标志置0，再次响应处理过程。

*/


//处理函数
void my_sa_handler(int num){
	int nIndex = 3;
	printf("sa_handler:%d\n",num);
	while(nIndex){
		printf("%d\n",nIndex--);
		sleep(1);
	}
}

int main(){
	struct sigaction sigAction;//信号捕捉处理的结构体（信号的存储和处理结构）

	
	//赋值
//	sigAction.sa_handler = SIG_IGN;//对信号处理是忽略
//	sigAction.sa_handler = SIG_DFL;//对信号处理是默认的
	sigAction.sa_handler = my_sa_handler;

	///sa_handler和sa_sigaction是互斥的，因此只需要给其中一个赋值即可
	//sigAction.sa_mask = sigTempMask;
	
	sigemptyset(&sigAction.sa_mask);//清零
	sigaddset(&sigAction.sa_mask,SIGTSTP);//在执行SIGUSR1信号的捕捉时，不立刻嵌套的执行20号，ctrl+z SIGTSTP信号处理
	//SIGUSR1信号的捕捉时，不立刻嵌套的执行3号，ctrl+\ SIGQUIT信号处理
	sigaddset(&sigAction.sa_mask,SIGQUIT);

	sigAction.sa_flags = 0;
	sigAction.sa_restorer = NULL;
		
	//信号捕捉程序，9号SIGKILL和19号SIGSTOP信号是不可以被阻塞、捕捉、忽略的
	//10号SIGUSR1和 12号SIGUSR2信号是用户自定义信号，默认处理是TERM（终止），可以利用这两个信号实现父子进程同步输出
	//原理：子进程可以继承父进程的信号屏蔽字和信号处理动作
	//实现：父进程通过fork获得子进程pid,子进程通过getppid获得父进程pid，这样两者可以通过kill函数互相发送SIGUSER信号
	//父进程扑捉子进程发送的SIGUSER2信号，并在自己的sa_handler处理函数中定义一个static int num,每次增加num值
	//子进程扑捉父进程发送的SIGUSER1信号，并在自己的sa_handler处理函数中每次增加num值
	
	if(sigaction(SIGUSR1,&sigAction,NULL)){
		perror("sigaction failed:");
		exit(-1);
	}

	while(1){
		sleep(1);
	}

	return 0;
}
