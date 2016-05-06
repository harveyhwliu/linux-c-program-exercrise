#include "common.h"
/*
	可重入函数：函数中依赖自己栈上的变量，访问全局变量，系统资源等需要利用关中断、信号等方式防止出错，可以被中断，中断返回后没有问题
	如果一个程序的问题是有时候出现，有时候不出现，应考虑是否是在多任务环境下使用了不可重入函数
	strtok是不可重入函数，因为其内部使用了静态指针直线切割后的地址，而strtok_r则是可重入函数
*/

int main()
{
	char Data[]="  我 现 在  正 在 使用 可重入函数 strtok_r    ";
	char *saveptr,*pResturnValue;
	
	saveptr = Data;
	printf("没处理前的字符串：\n%s\n",Data);
	printf("strtok_r后字符串为：\n");
	while((pResturnValue=strtok_r(saveptr," ",&saveptr))){
		printf("%s",pResturnValue);
	}
	printf("\n");

	return 0;
}
