#include "common.h"


int main(){
	
	sleep(3);
	//向调用自己的进程或线程发送信号
	int nReturnValue = raise(9);
	if( nReturnValue ){
		perror("raise failed!");
		exit(-1);
	}
	return 0;
}
