#include "common.h"

int main()
{
	char buf[1024];
	time_t t;
	time(&t);//获得当前时间

	ctime_r(&t,buf);//将当前时间转化，buf至少要26个字节

	printf("%s\n",buf);

	return 0;
}
