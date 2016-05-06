#include "common.h"

//线程函数
void* ThreadFunc1(void* args){
	printf("Thread1 returning\n");
	return (void*)1;
}
void* ThreadFunc2(void* args){
	printf("Thread2 exsting\n");
	pthread_exit( (void*)2 );
}
void* ThreadFunc3(void* args){
	while(1){
		printf("Thread3 working\n");
		sleep(1);
	}
}

int main(int argc,const char* argv[]){
	pthread_t tid;//接收创建线程返回的线程ID LPW
	void* tret;//接收线程函数退出返回值
	
	//线程1
	pthread_create(&tid,NULL,ThreadFunc1,NULL);//创建线程
	pthread_join(tid,&tret);//阻塞等待线程1线程函数退出
	printf("线程1线程函数退出返回值：%d\n",(int)tret);
	//线程2
	pthread_create(&tid,NULL,ThreadFunc2,NULL);//创建线程
	pthread_join(tid,&tret);//阻塞等待线程1线程函数退出
	printf("线程2线程函数退出返回值：%d\n",(int)tret);
	//线程3
	pthread_create(&tid,NULL,ThreadFunc3,NULL);//创建线程
	sleep(3);
	pthread_cancel(tid);//主控线程函数中曲线线程3的运行
	pthread_join(tid,&tret);//阻塞等待线程1线程函数退出
	printf("线程3线程函数退出返回值：%d\n",(int)tret);



	return 0;
}


