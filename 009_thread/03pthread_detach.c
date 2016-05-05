#include "common.h"

//线程函数
void* pthreadFunc(void* args){
	int n = 3;
	while( n-- ){
		printf("线程函数计数：%d\n",n);
		sleep(1);
	}
	return (void*)1;
}

int main(int argc,const char* argv[]){
	pthread_t tid;	//创建线程的线程ID
	void* tret;	//pthread_join等待线程退出时接收线程函数返回值

	int err;

	//创建线程
	pthread_create(&tid,NULL,pthreadFunc,NULL);
	//第一次运行时注释掉下面语句，第二次再打开，分析两次结果
//	pthread_detach(tid);//线程分离，分离后不用在使用pthread_join等待线程退出才能释放资源，分离后线程退出系统自动释放资源
	
	while(1){
		err = pthread_join(tid,&tret);//不能对已经pthread_detach分离后的线程使用pthread_join,会返回EINVAL
		if( err )
			fprintf(stderr,"thread %s\n",strerror(err));//线程中错误不会置errno,所以不能使用perror,应该使用strerror
		else
			fprintf(stderr,"thread exit code %d\n",(int)tret);
		
		sleep(1);		
	}

	return 0;
}
