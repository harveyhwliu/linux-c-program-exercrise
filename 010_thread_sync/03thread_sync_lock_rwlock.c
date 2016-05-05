#include "common.h"

//读写锁：（DB)-查询>写，例如3个线程不定时写同一个全局资源，5个线程不定时读一个线程

int g_ncount;//全局变量，多线程访问时需要考虑同步的问题

//全局读写锁
pthread_rwlock_t rwlock;
//写线程
void* threadFunc_Write(void* args){
	int t;
	while(1){
		pthread_rwlock_wrlock(&rwlock);//写锁，请求不到写锁则阻塞
		t=g_ncount;
		usleep(100);
		printf("write---:%x   :g_ncount=%d - ++g_ncount=%d\n",(unsigned int)pthread_self(),t,++g_ncount);
		pthread_rwlock_unlock(&rwlock);
		sleep(1);
	}
	return (void*)0;
}
//读线程
void* threadFunc_Read(void* args){
	while(1){
		//读锁
		pthread_rwlock_wrlock(&rwlock);
		printf("read:%x   : %d\n",(unsigned int)pthread_self(),g_ncount);
		pthread_rwlock_unlock(&rwlock);
		sleep(1);
	}
	return (void*)0;
}


int main(int argc,const char* argv[]){
	int nIndex = 0,err;
	pthread_t tidw[3],tidr[7];//3个写线程，7个读线程

	//读写锁初始化
	pthread_rwlock_init(&rwlock,NULL);
	
	//创建3个写线程
	for(nIndex = 0;nIndex <3;nIndex++){
		pthread_create(&tidw[nIndex],NULL,threadFunc_Write,NULL);
	}
	//创建7个读线程
	for(nIndex = 0;nIndex <7;nIndex++){
		if( (err = pthread_create(&tidr[nIndex],NULL,threadFunc_Read,NULL)) ) 
		{
			fprintf(stderr,"pthread_create read failed:%s\n",strerror(err));
		}
	}

	//没有设置分离态的线程需要调用pthread_join()等待两个线程的线程执行完成，回收线程资源
	//这里有问题，就是线程结束的顺序是不固定了，因此pthread_join确实阻塞的，解决方法是线程变成detach
	for(nIndex = 0;nIndex < 3; nIndex++){
		pthread_join(tidw[nIndex],NULL);
	}
	for(nIndex = 0;nIndex < 7; nIndex++){
		pthread_join(tidr[nIndex],NULL);
	}


	//销毁锁
	pthread_rwlock_destroy(&rwlock);
	
	return 0;
}
