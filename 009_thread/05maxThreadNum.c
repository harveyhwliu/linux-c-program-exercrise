#include "common.h"
//线程函数
void* threadFunc(void* args){

	while(1){
		sleep(2);
	}
	return NULL;
}

int main(){
	pthread_t tid;
	int i = 1,err;
	while(1){
		err = pthread_create(&tid,NULL,threadFunc,NULL);//由于默认栈大小是8M，所以3G内存实际能创建382个线程，ulimit -a
		if(err){					//而使用ulimit -s 1024,指定栈大小为1M，实际能创建，3055多线程个数
			fprintf(stderr,"pthread_create:%s\n",strerror(err));
			exit(1);
		}
		printf("%d\n",i++);
	}
	return 0;
}
