#include "common.h"
//线程函数

void* pthreadFunction(void* arg){
	volatile int i= 0;
	int *p = (int*)arg;//线程参数
	printf("thread - 线程ID LWP:%x\n",(unsigned int)pthread_self());//获得线程ID
	printf("thread - 进程ID PID:%d\n",getpid());//进程ID
	printf("thread - 线程参数：%d\n",*p);//参数
	
	while(1){
		//sleep(1);//此时，pthread_cancel发送的终止信号，因此sleep产生系统调用而终止线程
		i++;//此时，pthread_cancel发送的终止信号，因为没有产生系统调用因此线程没有被立即终止掉，ps -eLf
		//因此这个线程并没有在产生系统调用，因此并没有终止
		pthread_testcancel();//每次调用都是让内核去检测是否该线程已经被cancel了
	}
}

int main(){
	pthread_t tid;//用来获取线程ID
	int num = 100;//线程参数
	int nReturnValue;//创建线程返回值
//创建线程，成功返回0，失败返回错误码
	if( !(nReturnValue = pthread_create(&tid,NULL,pthreadFunction,(void*)&num)) ){//1、创建线程，2、线程号填到tid,3、返回函数调用
		sleep(1);//主线程等待1S，让子线程运行起来
	}
	else{
		fprintf(stderr,"pthread_create失败：%s\n",strerror(nReturnValue));
		exit(1);
	}
	//注意这里返回的tid可能是没有意义的，和pthread_self并不能是完全等同
	printf("main - 主控线程ID LWP:%x\n",(unsigned int)pthread_self());//线程ID
	printf("main - 子线程ID LWP:%x\n",(unsigned int)tid);//线程ID
	printf("main - 进程ID PID:%d\n",getpid());//进程ID
	
	sleep(4);
	pthread_cancel(tid);//pthread_cancel向另一个线程发送终止信号，系统不会马上关闭被取消的线程，
			//只有在被取消的线程下次系统调用时才会真正的结束
			//可以使用pthread_testcancel,让内核去检测是否需要取消当前线程
	while(1){
		sleep(1);
	}
	return 0;
}
