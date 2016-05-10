#include "common.h"

#define DEFAULT_TIME 10			//用于管理线程池的线程，每隔多久遍历线程池一遍
#define MIN_WAIT_TASK_NUM 10		//用于在任务队列当前任务较多时增加新的线程到线程池的阈值，即queue_size > MIN_WAIT_TASK_NUM 就增加线程个数
#define DEFAULT_THREAD_NUM 10		//每次创建和销毁线程的个数
#define true 1
#define false 0

/*任务结构体*/
typedef struct{
	void* (*function)(void *);	//任务的函数指针，回调函数
	void* args;			//任务函数的参数
}threadpool_task_t;			//任务结构体

/*线程池对象结构体*/
typedef struct{
	pthread_mutex_t lock;		//
	pthread_mutex_t thread_counter;	//操作thread_busy_num时的互斥锁
	pthread_cond_t queue_not_full;	//条件变量任务队列不满时，通知投放任务的线程开始向线程池结构体的环形任务队列中投递任务
	pthread_cond_t queue_not_empty;	//条件变量任务队列不为空时，通知等待处理任务的任务线程工作
	pthread_t* threads;		//保存线程池内所有线程的tid
	pthread_t adjust_tid;		//管理线程池的线程，用于调整线程池内线程的个数（创建、销毁）等
	threadpool_task_t *task_queue;	//环形任务队列

	int pthread_min_num;		//线程池内最小线程个数
	int pthread_max_num;		//线程池内最大线程个数
	int pthread_alive_num;		//线程池内当前存活的线程个数
	int pthread_busy_num;		//线程池内当前正处于忙状态的线程个数
	int pthread_wait_exit_num;	//线程池内等待被销毁的线程个数
	int queue_front;		//环形任务队列的队头索引下标
	int queue_rear;			//环形任务队列的队末尾索引下标
	int queue_size;			//环形任务队列的中当前元素个数
	int queue_max_size;		//环形任务队列的最大容纳的任务个数
	int shutdown;			//线程池使用的状态，true表示不销毁线程池，false表示销毁线程池
}threadpool_t;

//
void* threadpool_thread(void* threadpool);

void* adjust_thread(void* threadpool);

int pthread_is_alive(pthread_t tid);

int threadpool_free(threadpool_t *pool);

//出错处理函数
int sys_error(const char* des,int exit_num){
	perror(des);
	return exit_num;
}

//线程池创建函数
threadpool_t* threadpool_create(int thread_min_num,int thread_max_num,int queue_max_size){
	int i = 0;
	threadpool_t *pool = NULL;
	do{
		//1、创建一个线程池结构体
		if( !(pool = (threadpool_t*)malloc(sizeof(threadpool_t))) ){
			printf("创建线程池结构体失败-malloc falied\n");
			break;
		}
		//2、为线程池结构体赋值
		pool->pthread_min_num = thread_min_num;
		pool->pthread_max_num = thread_max_num;
		pool->pthread_alive_num = thread_min_num;
		pool->pthread_busy_num = 0;
		pool->pthread_wait_exit_num = 0;
		pool->queue_front = 0;
		pool->queue_rear = 0;
		pool->queue_size = 0;
		pool->queue_max_size = queue_max_size;
		pool->shutdown = false;
		
		// 创建线程池用于保存线程tid的线程数组
		pool->threads = (pthread_t*)malloc(sizeof(pthread_t)*thread_max_num);
		if( !(pool->threads) ){
			printf("创建线程池用于保存线程tid的线程数组失败-malloc falied\n");
			break;
		}
		memset(pool->threads,0,sizeof(pool->threads));
		// 创建环形任务队列
		pool->task_queue = (threadpool_task_t*)malloc(sizeof(threadpool_task_t)*queue_max_size);
		if( !(pool->task_queue) ){
			printf("创建线程池内的环形任务队列失败-malloc failed\n");
			break;
		}
		//初始化互斥锁和条件变量（条件变量用于在任务队列有任务时通知线程起来工作）
		if( (pthread_mutex_init(&(pool->lock),NULL))
		  ||(pthread_mutex_init(&(pool->thread_counter),NULL))
		  ||(pthread_cond_init(&(pool->queue_not_full),NULL))
		  ||(pthread_cond_init(&(pool->queue_not_empty),NULL)) ){
		  	printf("创建线程池内互斥锁和条件变量失败-pthread_mutex_init|pthread_cond_init\n");
			break;
		  }
		
		//3、启动thread_min_num个线程执行任务线程函数
		for( i = 0; i < thread_min_num; i++ ){
			pthread_create(&(pool->threads[i]),NULL,threadpool_thread,(void*)pool);
			printf("start thread 0x%x...\n",(unsigned int)pool->threads[i]);
		}

		//4、创建调整线程池的线程
		pthread_create(&(pool->adjust_tid),NULL,adjust_thread,(void*)pool);
		
		return pool;
	}while(0);

	//如果创建线程池失败的话
	threadpool_free(pool);//失败的话就释放pool的存储空间
	return NULL;
}

//将一个任务添加如线程池的任务队列中
int threadpool_add( threadpool_t *pool,void*(*function)(void*args),void*args ){
	//1、获取互斥锁
	pthread_mutex_lock(&(pool->lock));
	//2、当线程池状体为true且当前线程池中任务队列已满时，应该阻塞等待，直到任务队列有位置
	while( (!pool->shutdown) && (pool->queue_size == pool->queue_max_size) )
		pthread_cond_wait(&(pool->queue_not_full),&(pool->lock));

	//如果要销毁线程池时应该释放锁
	if( pool->shutdown )
		pthread_mutex_unlock(&(pool->lock));

	//3、把任务加入到环形任务队列中
	//在队列的尾部添加，如果尾部存在上次任务执行后没有释放，则先释放在保存任务数据
	if( pool->task_queue[pool->queue_rear].args ){
		free(pool->task_queue[pool->queue_rear].args);
		pool->task_queue[pool->queue_rear].args = NULL;
	}
	
	pool->task_queue[pool->queue_rear].args = args;
	pool->task_queue[pool->queue_rear].function = function;
	//环形任务队列末尾索引后移
	pool->queue_rear = (pool->queue_rear+1)%(pool->queue_max_size);
	pool->queue_size++;//环形任务队列中当前任务数+1

	//4、当任务放入任务队列后，任务队列不为空，应该通知线程工作
	pthread_cond_signal(&(pool->queue_not_empty));

	//最后释放锁
	pthread_mutex_unlock(&(pool->lock));

	return 0;
}

//线程池中工作者线程执行任务的任务处理函数
void* threadpool_thread(void* threadpool){
	//1、从参数中拿到线程池结构体
	threadpool_t *pool = (threadpool_t*)threadpool;
	threadpool_task_t task;//每一个任务机构体
	
	//线程的任务处理函数 1、等待任务队列不为空的信号，2、退出多余线程，如果需要销毁线程池则销毁 3、处理任务
	while(true){
	//刚创建的线程，等待任务队列里有任务，否则阻塞
		//首先获得锁
		pthread_mutex_lock(&(pool->lock));
		//没有任务时则阻塞等待任务队列不为空
		while((!pool->shutdown)&&(!pool->queue_size) ){
			printf("thread 0x%x is waiting\n",(unsigned int)pthread_self());
			pthread_cond_wait(&(pool->queue_not_empty),&(pool->lock));//阻塞等待，释放锁，有信号通知时则获取锁继续执行	
			//如果需要让线程退出，则让多余的线程退出
			if( pool->pthread_wait_exit_num > 0 ){
				pool->pthread_wait_exit_num--;
				//只有在线程池中线程的个数大于最小线程数时才让多余线程退出
				if( pool->pthread_alive_num > pool->pthread_min_num ){
					printf("thread 0x%x is exiting\n",(unsigned int)pthread_self());
					pool->pthread_alive_num--;
					pthread_mutex_unlock(&(pool->lock));
					pthread_exit(NULL);//线程退出
				}
			}
		}
		//如果shutdown为true时需要关闭线程池，即关闭线程池中的每一个线程并释放相关资源
		if( pool->shutdown ){
			pthread_mutex_unlock(&(pool->lock));
			printf("thread ox%x is exiting\n",(unsigned int)pthread_self());
			pthread_exit(NULL);
		}
		//最后开始执行任务
		//1、从环形任务队列中那任务
		task.function = pool->task_queue[pool->queue_front].function;
		task.args = pool->task_queue[pool->queue_front].args;
		//环形任务队列有空的位置，更新环形任务队列并通知投递信号的线程工作
		pool->queue_front = (pool->queue_front+1)%(pool->queue_max_size);
		pool->queue_size--;
		
		pthread_cond_broadcast(&(pool->queue_not_full));

		//释放锁
		pthread_mutex_unlock(&(pool->lock));

		//执行任务的毁掉函数
		printf("thread ox%x starting working\n",(unsigned int)pthread_self());
		//开始执行任务函数，线程为忙状态，因此忙线程数++
		pthread_mutex_lock(&(pool->thread_counter));
		pool->pthread_busy_num++;
		pthread_mutex_unlock(&(pool->thread_counter));

		(*(task.function))(task.args);
	}
	pthread_exit(NULL);//这里不会走到
	return NULL;
}

//管理线程的线程函数，用来保证线程池中线程的个数稳定等
void* adjust_thread(void* threadpool){
	int i;
	threadpool_t *pool = (threadpool_t*)threadpool;
	while(!pool->shutdown){
		//获取线程池属性，操作线程池结构体，需要获得线程池结构体的锁
		pthread_mutex_lock(&(pool->lock));
		int queue_size = pool->queue_size;//获取任务的个数
		int pthread_alive_num = pool->pthread_alive_num;//获取线程池内存活的线程个数
		pthread_mutex_unlock(&(pool->lock));
		
		//创建线程，当任务数大于存活线程个数并且当前存活的线程个数小于线程池最大线程个数时才创建
		if( (queue_size > 3 + pthread_alive_num) && (pthread_alive_num + 1< pool->pthread_max_num) ){
			pthread_mutex_lock(&(pool->lock));
			int pthread_add_num = 0;
			// 一次增加DEFAULT_THREAD_NUM个线程
			for(i = 0; (i <pool->pthread_max_num)&& (pthread_add_num <DEFAULT_THREAD_NUM) && (pool->pthread_alive_num < pool->pthread_max_num);i++ ){
				//在线程池的线程数组中从头开始找新创建的线程应存放的位置
				if( !(pool->threads[i]) || !pthread_is_alive(pool->threads[i]) ){//如果线程tid存在且线程已经死掉了
					pthread_create(&(pool->threads[i]),NULL,threadpool_thread,(void*)pool);
					pthread_add_num++;
					pthread_alive_num++;
				}
			}
			pthread_mutex_unlock(&(pool->lock));
		}
		
		//获取当前正在处理任务的忙碌状态的线程个数
		pthread_mutex_lock(&(pool->thread_counter));
		int pthread_busy_num = pool->pthread_busy_num;
		pthread_mutex_unlock(&(pool->thread_counter));
		
		//销毁多余的空闲线程 -- 存活的线程太大而忙的线程太少了
		if( (pthread_busy_num*2 <pthread_alive_num)&&( pthread_alive_num > pool->pthread_min_num  ) ){
			//一次销毁DEFAULT_THREAD_NUM个线程
			pthread_mutex_lock(&(pool->lock));
			pool->pthread_wait_exit_num = DEFAULT_THREAD_NUM;
			pthread_mutex_unlock(&(pool->lock));
			//通知DEFAULT_THREAD_NUM个空闲线程退出
			for( i = 0; i < DEFAULT_THREAD_NUM; i++ )
				pthread_cond_signal(&(pool->queue_not_empty));
		}
		sleep(DEFAULT_TIME);
	}
	return NULL;
}

//销毁线程池
int threadpool_destroy(threadpool_t *pool){
	int i;
	if(!pool)
		return 0;
	//将线程池销毁的标准位shutdown置1
	pool->shutdown = true;
	//先销毁管理线程
	pthread_join(pool->adjust_tid,NULL);
	//在通知所有空闲的线程退出
	pthread_cond_broadcast(&(pool->queue_not_empty));
	//最后释放线程池中仅存的最小线程数的线程
	for( i = 0; i < pool->pthread_min_num; i++ )
		pthread_join(pool->threads[i],NULL);
	
	//释放线程池的内存
	threadpool_free(pool);
	return 0;
}
//释放线程池所占的内存
int threadpool_free(threadpool_t* pool){
	if(!pool)
		return 0;
	//释放环形任务队列
	if(pool->task_queue)
		free(pool->task_queue);
	//释放线程池中线程数组
	if(pool->threads){
		free(pool->threads);
		//销毁锁
		pthread_mutex_destroy(&(pool->lock));
		pthread_mutex_destroy(&(pool->thread_counter));
		pthread_cond_destroy(&(pool->queue_not_empty));
		pthread_cond_destroy(&(pool->queue_not_full));
	}

	free(pool);
	pool=NULL;
	return 0;
}
//获取线程池中所有存活的线程的个数
int threadpool_get_all_pthread_num(threadpool_t* pool){
	int thread_num = -1;
	if (pool){
		pthread_mutex_lock(&(pool->lock));
		thread_num = pool->pthread_alive_num;
		pthread_mutex_unlock(&(pool->lock));
		return thread_num;
	}
	return -1;
}

//获得线程池中所有正在处理任务的忙碌状态的线程个数
int threadpool_get_busy_pthread_num(threadpool_t* pool){
 	int thread_num = -1;
	if (pool){
		 pthread_mutex_lock(&(pool->lock));
		 thread_num = pool->pthread_busy_num;
		 pthread_mutex_unlock(&(pool->lock));
		 return thread_num;
	 }
 	return -1;
}
//测试指定的线程是否存活，用0号信号
int pthread_is_alive(pthread_t tid){
	int nres = pthread_kill(tid,0);
	if(nres == ESRCH)
		return false;//线程已经退出
	return true;
}

/*测试使用，如果作成库时需要注释下面的代码*/
#if 1
void* process(void*args){
	printf("thread 0x%x working on task %d\n",(unsigned int)pthread_self(),*(int*)args);
	sleep(1);
	printf("task %d is end\n",*(int*)args);
	return NULL;
}

int main(int argc,const char* argv[]){
	int *num = (int *)malloc(sizeof(int)*10);
	int i;
	threadpool_t* pool = threadpool_create(3,10,10);//创建线程池，3个最小线程，10个最大线程数，任务最多10个
	printf("pool inited\n");

	for(i=0; i <10;i++){
		num[i] = i;
		printf("add task %d\n",i);
		threadpool_add(pool,process,(void*)&num[i]);

	}
	sleep(1);
	printf("-----准备销毁线程池------\n");
	threadpool_destroy(pool);
	return 0;
}



#endif

