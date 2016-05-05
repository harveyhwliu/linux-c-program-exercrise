#include "common.h"


int main()
{
	int fd = open("TestFile",O_CREAT,0777);
	//如果用户没有权限创建文件，如本程序运行在根目录下，需要在运行指定有效的ID即sudo chmod 04777 ./app的形式执行:
	
	if(-1 == fd)
		printf("create File TestFile Failed!\n");



	//getudi()获取实际用户ID，geteuid()获取有效用户ID。
	printf("getuid():%d,geteuid():%d\n",getuid(),geteuid());

	return 0;
}
