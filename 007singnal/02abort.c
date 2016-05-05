#include "common.h"

int main()
{
	sleep(5);
	//向调用自己的进程发送6号终止信号SIGABRT,相当于raise(SIGABRT)
	abort();
	
	return 0;
}
