#include "common.h"
#define DEF_COUNT 5000

//两个线程访问全局变量，没有使用线程同步方式（最少应该加上锁），发现出现问题：两个线程理想方式应该每人增大5000，结果应该是10000，但是结果没有，数据丢失严重

int g_ncount;//全局变量，多线程访问时需要考虑同步的问题
//锁-互斥量-全局变量，可以静态初始化
pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;

//锁的范围应该越小越好，锁住的范围叫做临界区；范围太大可能不叫并发，而叫串行

/*int pthread_mutex_destroy(pthread_mutex_t *mutex);
  int pthread_mutex_init(pthread_mutex_t *restrict mutex,const pthread_mutexattr_t *restrict attr);
 pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
 
 int pthread_mutex_lock(pthread_mutex_t *mutex);
  int pthread_mutex_trylock(pthread_mutex_t *mutex);
 int pthread_mutex_unlock(pthread_mutex_t *mutex);

*/			    
//线程函数
void* threadFunc(void* args){
	int i,val;
	for(i = 0; i < DEF_COUNT; i++){
		//访问共享资源（全局变量），应该加锁,锁住的范围，叫做临界区
		//上锁
		pthread_mutex_lock(&g_mutex);
		val = g_ncount;
		printf("%x : %d\n",(unsigned int)pthread_self(),val+1);
		g_ncount = val + 1;
		//退出时解锁
		pthread_mutex_unlock(&g_mutex);
	}
	return (void*)0;
}

int main(int argc,const char* argv[]){
	
	//这里使用的锁-互斥量-函数内定义的局部变量-需要调用函数pthread_mutex_init动态初始化锁（所有互斥量）
	pthread_mutex_t mutex;
	pthread_mutex_init(&mutex,NULL);//全局变量锁可以不调用pthread_mutex_init初始化，可以采用静态初始化

	pthread_t tid1,tid2;
	//创建两个线程
	pthread_create(&tid1,NULL,threadFunc,NULL);
	pthread_create(&tid2,NULL,threadFunc,NULL);

	//等待两个线程的线程执行完成，回收线程资源
	pthread_join(tid1,NULL);
	pthread_join(tid2,NULL);

	//销毁锁
	//pthread_mutex_destroy(&mutex);
	printf("destroy-mutex:%d\n",pthread_mutex_destroy(&mutex));
	printf("destroy-g_mutex:%d\n:",pthread_mutex_destroy(&g_mutex));
	
	return 0;
}
