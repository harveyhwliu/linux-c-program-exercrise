#include "common.h"
//条件变量：生产者和消费者,也适用在一个线程先执行，之后其他线程之后在执行。：

/*
int pthread_cond_destroy(pthread_cond_t *cond);//销毁条件变量
int pthread_cond_init(pthread_cond_t *restrict cond,const pthread_condattr_t *restrict attr);//动态初始化 条件变量
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;//静态初始化 条件变量

int pthread_cond_timedwait(pthread_cond_t *restrict cond,pthread_mutex_t *restrict mutex,const struct timespec *restrict abstime);//定时等待
int pthread_cond_wait(pthread_cond_t *restrict cond,pthread_mutex_t *restrict mutex);//等待


int pthread_cond_broadcast(pthread_cond_t *cond);//唤醒全部等待的线程
int pthread_cond_signal(pthread_cond_t *cond);//唤醒一个等待的线程

*/

struct msg{
	struct msg *next;
	int num;
};

struct msg *head;


/*条件变量 静态初始化*/ 
pthread_cond_t g_cond = PTHREAD_COND_INITIALIZER;
//锁-互斥量 - 静态初始化
pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;

//消费者线程
void* threadFunc_Consumer(void* args){
	struct msg *mp;
	while(1){//只要有商品就消费，没有就等待
		//上锁
		pthread_mutex_lock(&g_mutex);
	
		//消费
		
		//1、如果没有商品就等待被唤醒
		while(!head)
			pthread_cond_wait(&g_cond,&g_mutex);//效果:1、阻塞等待g_cond条件变量的线程被唤醒(pthread_cond_signal等)
							//2、释放互斥锁pthread_mutex_unlock(&g_mutex);
							//3、当被唤醒时,解除阻塞，并且重新去申请获得锁，即调用pthread_mutex_lock(&g_mutex)
		//2、有商品才开始消费
		mp = head;
		head = head->next;
		printf("consumer:%d\n",mp->num);
		
		//解锁
		pthread_mutex_unlock(&g_mutex);
		free(mp);
		sleep( rand() % 5 );
	}
	return (void*)0;
}
//生产者线程
void* threadFunc_Producer(void* args ){
	struct msg *mp;
	while(1){//不停的生产，并唤醒消费者
		mp = malloc(sizeof(struct msg));
		mp->num = rand()% 1000 + 1;//1~1000
		printf("Producer:%d\n",mp->num);
		//上锁
		pthread_mutex_lock(&g_mutex);
		//放入产品链表
		mp->next = head;
		head = mp;
		//解锁
		pthread_mutex_unlock(&g_mutex);
	
		//唤醒其他消费者线程
		//pthread_cond_broadcast(&g_cond);//唤醒等待队列上的所有线程
		pthread_cond_signal(&g_cond);
		sleep(rand()%5);
	}
	return (void*)0;
}	

int main()
{
	pthread_t pid,cid;
	srand(time(NULL));//种时间种子 rand() [0,32767]
	pthread_create(&pid,NULL,threadFunc_Producer,NULL);
	pthread_create(&cid,NULL,threadFunc_Consumer,NULL);

	pthread_join(pid,NULL);
	pthread_join(cid,NULL);

	return 0;
}


