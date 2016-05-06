#include "common.h"

/*

	在使用信号、多线程、多进程的程序中，如果要使用的类型是如long long 的最大的类型，可能会出现数据不准确性，在
	这种情况下，应该使用sig_atomic_t的平台下的院子类型，该类型根据系统是多少位的自动typedef为最大的位数，避免异步I/O操作

	sig_atomic_t  平台下的原子类型

	volatile 防止编译器开启优化选项时，优化对内存的读写
*/


int main()
{
	sig_atomic_t num = 1000000;
	printf("sig_atomic_t:%d,sizeof(sig_atomic_t)=%d\n",num,sizeof(num));

	return 0;
}
