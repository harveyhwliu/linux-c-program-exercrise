#include "common.h"

void* threadFunc(void*args){
	int n = 10;
	while(n--){
		printf("%x %d\n",(unsigned int)pthread_self(),n);
		sleep(1);
	}
	return (void*)1;
}

int main(){

	pthread_t tid;
	pthread_attr_t attr;
	int err;
	//初始化线程属性
	pthread_attr_init(&attr);

	//设置线程是分离属性

	//线程属性有分离和非分离属性PTHREAD_CREATE_JOINABLE 
	pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);

	pthread_create(&tid,&attr,threadFunc,NULL);
	
	err = pthread_join(tid,NULL);
	while(1){
	if(err){
		fprintf(stderr,"pthread_join %s",strerror(err));
		printf("主线程已经开庭时退出了\n");
		sleep(10);//把这行注释掉，子线程变成僵尸线程
		pthread_exit((void*)-1);//这样主线程退出，不影响子线程的运行,但是如果子线程没有退出，将会变成僵尸线程
	}
	}

	return 0;
}
