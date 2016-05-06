#include "common.h"

struct mt {
	int num;		//进程间想要共享的资源
	pthread_mutex_t mutex;	//需要同步的锁
	pthread_mutexattr_t mutexattr;//需要同步用的互斥锁的对象属性，用于设置互斥锁为进程间锁
};

void sys_error(const char* des,int exitNum){
	perror(des);
	exit(exitNum);
}

int main(){
	int fd,i,nReturnValue;
	struct mt *m;//m是一个指针，*m的指向结构体大小的内容
	pid_t pid;

//为什么不需要申请空间？？
//	m = (struct mt*)malloc(sizeof(struct mt));//申请空间

	umask(0);
	if( -1 == (fd=open("mt_test",O_CREAT | O_RDWR,0777)) )
		sys_error("open failed:",-1);
	
	//创建文件，文件大小为0，而利用mmap时需要保证文件大小不为0，因此需要拓展文件大小
	if( ftruncate(fd,sizeof(*m)) ){//拓展的大小就是需要同步资源的大小，因此就是结构体的大小，而且ftruncate拓展的内容都为'\0'
		close(fd);
		sys_error("ftruncate failed:",-2);
	}

	//创建文件映射 mmap
	if( (void*)-1 == (m = mmap(NULL,sizeof(*m),PROT_READ | PROT_WRITE,MAP_SHARED,fd,0)) ){
		close(fd);
		sys_error("mmap failed:",-3);
	}
	//文件映射创建成功，可以关闭文件描述符了
	close(fd);
	//将共享内存清空
	memset(m,0,sizeof(*m));
	
	//初始化互斥对象属性，设置互斥对象为PTHREAD_PROCESS_SHARED进程间共享，默认是PTHREAD_PROCESS_PRIVATE线程间共享
	pthread_mutexattr_init(&(m->mutexattr));
	pthread_mutexattr_setpshared(&(m->mutexattr),PTHREAD_PROCESS_SHARED);

	//初始化互斥锁
	pthread_mutex_init(&(m->mutex),&(m->mutexattr));

	//fork()之后子进程将继承父进程的所有资源，包括互斥锁
	if( !(pid=fork()) ){//子进程　
		for( i = 0; i < 10; i++ ){
			//上锁
			pthread_mutex_lock(&(m->mutex));
			//操作
			(m->num)++;
			printf("子进程：%d\n",m->num);
			//解锁
			pthread_mutex_unlock(&(m->mutex));
			
			sleep( 1 );
		}
	}
	else if(pid > 0){//父进程
		for( i = 0; i < 10; i++ ){
			//上锁
			pthread_mutex_lock(&(m->mutex));
			//操作
			m->num +=2;
			printf("父进程：%d\n",m->num);
			//解锁
			pthread_mutex_unlock(&(m->mutex));
			sleep( 1 );
		}
		//父进程应该wait阻塞等待子进程退出，回收子进程PCB资源
		wait(NULL);//不关心子进程是如何退出的
	}
	//销毁互斥对象属性
	nReturnValue = pthread_mutexattr_destroy(&(m->mutexattr));
	if(nReturnValue){
		printf("%s\n",strerror(nReturnValue));
	}
	//销毁互斥锁
	nReturnValue = pthread_mutex_destroy(&(m->mutex));
	if(nReturnValue){
		printf("%s\n",strerror(nReturnValue));
	}
	
	//解除文件映射
	munmap(m,sizeof(*m));

	//free结构体
//	free(m);
//	m = NULL;
	//将创建的文件变为临时文件,就是删除创建的文件，unlink下
	unlink("mt_test");
	
	return 0;
}
