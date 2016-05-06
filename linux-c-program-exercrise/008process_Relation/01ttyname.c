#include "common.h"

int main()
{
	//查看当前终端设备文件的文件名
	printf("fd 0:%s\n",ttyname(0));//查看终端设备名称
	printf("fd 1:%s\n",ttyname(1));//查看终端设备名称
	printf("fd 2:%s\n",ttyname(2));//查看终端设备名称
	printf("fd 3:%s\n",ttyname(3));//查看终端设备名称

	return 0;
}
