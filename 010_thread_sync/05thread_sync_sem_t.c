#include "common.h"
//将信号量用在生产者和消费者模型中，
/*
	信号量：互斥锁的升级版，对应的信号量值为n，互斥锁值只有0 、1两种状态，信号量可用于线程、进程，互斥锁默认只用于线程同步，可以通过pthread_mutexattr_init、pthread_mutexattr_destroy,pthread_mutexattr_setpshared设置互斥锁属性，设置互斥锁从线程锁为进程锁
#include <semaphore.h>
int sem_init(sem_t *sem, int pshared, unsigned int value);//初始化没有命名的信号量，pshared指定用于线程或是进程，value表示信号量初始值
int sem_destroy(sem_t *sem);

int sem_wait(sem_t *sem);信号量-1，阻塞等待，相当于lock
int sem_trywait(sem_t *sem);
int sem_timedwait(sem_t *sem, const struct timespec *abs_timeout);

 int sem_post(sem_t *sem);信号量+1，释放锁，相当于unlock
*/
#define DEF_NUM (20)
int queue[DEF_NUM];
sem_t blank_number,product_number;
//生产一个，消费一个的模型

//生产者线程函数
void* threadFunc_Producer(void* args){
	int p=0;
	while(1){
		//上锁
		sem_wait(&blank_number);//生产前，等待消费掉
		//生产
		queue[p] = rand() % 1000 + 1;//TODO:在放之前应该确保容器没有装满
		printf("Producer %d\n",queue[p]);
		//解锁
		sem_post(&product_number);//生产后，通知消费值消费
		p=(p+1)%DEF_NUM;
		sleep(rand()%5);
	}
	return (void*)0;
}
//消费者线程函数
void* threadFunc_Consumer(void* args){
	int c = 0;
	while(1){
		//上锁
		sem_wait(&product_number);//消费前等待生产完成
		//消费
		printf("Consumer %d\n",queue[c]);
		queue[c]=0;
		//解锁
		sem_post(&blank_number);//消费完成，通知生产者生产
		c=(c+1)%DEF_NUM;
		sleep(rand()%5);
	}
	return (void*)0;
}


int main(){
	pthread_t pid,cid;
	//初始化信号量
	sem_init(&blank_number,0,DEF_NUM);//初始化生产者信号量，线程同步，信号量初始值为DEF_NUM,表示还能生产产品的个数
	sem_init(&product_number,0,0);//消费者 信号量初始值为0，表示不能消费

	//创建线程
	pthread_create(&pid,NULL,threadFunc_Producer,NULL);
	pthread_create(&cid,NULL,threadFunc_Consumer,NULL);

	//线程属性为非分离态，需要阻塞等待线程退出以回收资源
	pthread_join(pid,NULL);
	pthread_join(cid,NULL);

	return 0;
}
