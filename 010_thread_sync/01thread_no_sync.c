#include "common.h"
#define DEF_COUNT 5000

//两个线程访问全局变量，没有使用线程同步方式（最少应该加上锁），发现出现问题：两个线程理想方式应该每人增大5000，结果应该是10000，但是结果没有，数据丢失严重

int g_ncount;//全局变量，多线程访问时需要考虑同步的问题

//线程函数
void* threadFunc(void* args){
	int i,val;
	for(i = 0; i < DEF_COUNT; i++){
		val = g_ncount;
		printf("%x : %d\n",(unsigned int)pthread_self(),val+1);
		g_ncount = val + 1;
	}
	return (void*)0;
}

int main(int argc,const char* argv[]){
	pthread_t tid1,tid2;
	//创建两个线程
	pthread_create(&tid1,NULL,threadFunc,NULL);
	pthread_create(&tid2,NULL,threadFunc,NULL);

	//等待两个线程的线程执行完成，回收线程资源
	pthread_join(tid1,NULL);
	pthread_join(tid2,NULL);

	return 0;
}
