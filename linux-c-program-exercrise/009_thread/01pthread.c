#include "common.h"
//线程函数

void* pthreadFunction(void* arg){
	int *p = (int*)arg;//线程参数
	printf("thread - 线程ID LWP:%x\n",(unsigned int)pthread_self());//获得线程ID
	printf("thread - 进程ID PID:%d\n",getpid());//进程ID
	printf("thread - 线程参数：%d\n",*p);//参数

	//sleep(1);

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
	
	sleep(2);//保证子线程退出
	return 0;
}
