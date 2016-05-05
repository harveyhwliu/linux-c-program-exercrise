#include "common.h"
/*
system可以调用shell命令
system() executes a command specified in command by calling /bin/sh 
*/
int main()
{
	system("echo hello");
	return 0;
}
