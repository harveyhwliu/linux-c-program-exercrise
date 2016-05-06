#include "common.h"

int main(int argc,const char* argv[])
{

	if(!access("abc",F_OK))
		printf("文件存在！");
	else
		perror("文件不存在：");
	if(!access("abc",R_OK))
		printf("文件可读！");
	else
		perror("文件没有读权限：");
	if(!access("abc",W_OK))
		printf("文件可写！");
	else
		perror("文件没有写权限：");
	if(!access("abc",X_OK))
		printf("文件可以被执行！");
	else
		perror("文件没有执行权限：");



	return 0;
}
