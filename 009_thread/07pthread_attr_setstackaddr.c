#include "common.h"
#define DEF_STACK_SIZE (0x800000)

//DEF_STACK_SIZE=0x100000;//5*4=20位，2^20=MB,所以分配了1M的内存
int add(int num){
	return num++;
}

void* threadFunc(void* args){
	
/*	int n = 3;
	while(n--){
	printf("%d\n",add(n));
		sleep(1);//pthread_testcancel();//检测该线程是否被别人pthread_cancel()了
	}*/
	sleep(1);
	return (void*)0;
}


int main(int argc,const char* argv[]){
	pthread_t tid;
	int err,detachstate,ncount=0;
	pthread_attr_t attr;
	size_t stacksize;
	void* stackaddr;

	//初始化线程属性
	pthread_attr_init(&attr);
	//获得默认线程栈大小
	pthread_attr_getstack(&attr,&stackaddr,&stacksize);
	printf("stackaddr=%p,stacksize=%x\n",stackaddr,stacksize);

	//获得栈的安全属性
	pthread_attr_getdetachstate(&attr,&detachstate);
	if(detachstate == PTHREAD_CREATE_DETACHED)
		printf("栈安全属性为分离\n");
	else if(detachstate == PTHREAD_CREATE_JOINABLE)
		printf("栈安全属性为非分离\n");
	else
		printf("栈安全属性为未知类型\n");
	while(1){
	//设置线程属性
	pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
	stackaddr = (void*)malloc(DEF_STACK_SIZE);//5*4=20位，2^20=MB,所以分配了1M的内存
	if( !stackaddr ){
		perror("malloc:");
		exit(1);
	}
	stacksize=DEF_STACK_SIZE;
	pthread_attr_setstack(&attr,stackaddr,stacksize);
	
	//创建线程
	err = pthread_create(&tid,&attr,threadFunc,NULL);
	if(err){
		fprintf(stderr,"pthread_create:%s\n",strerror(err));
	}
	else{
		printf("%d\n",++ncount);
	}

/*	//创建线程以后在获取一次试试
	//获得默认线程栈大小
	pthread_attr_getstack(&attr,&stackaddr,&stacksize);
	printf("已经创建线程了：stackaddr=%p,stacksize=%x\n",stackaddr,stacksize);

	//获得栈的安全属性
	pthread_attr_getdetachstate(&attr,&detachstate);
	if(detachstate == PTHREAD_CREATE_DETACHED)
		printf("已经创建线程了：栈安全属性为分离\n");
	else if(detachstate == PTHREAD_CREATE_JOINABLE)
		printf("已经创建线程了：栈安全属性为非分离\n");
	else
		printf("已经创建线程了：栈安全属性为未知类型\n");
*/
	//pthread_join(tid,NULL);//这里是分离态的了，不用等待线程退出了//阻塞，等待回收线程资源
	}
	free(stackaddr);
	stackaddr = NULL;

	//创建的线程属性应该销毁
	pthread_attr_destroy(&attr);

	return 0;
}
