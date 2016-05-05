#include <stdio.h>
#include <unistd.h>

int main()
{
	//打印进程的环境变量 == shell下输入env的结果
	extern char** environ;
	int nIndex = 0;
	while(environ[nIndex]){
		printf("%s\n",environ[nIndex++]);
	}

	return 0;
}
