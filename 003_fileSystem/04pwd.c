#include "common.h"
#define DEF_BUFFER_LEN (4*1024)

int main()
{
	char nameBuffer[DEF_BUFFER_LEN];
	bzero(nameBuffer,sizeof(nameBuffer));

	//先改变当前目录
	if(!chdir("/"))
		printf("chdir():OK!");
	else
		perror("chdir():Failed-");
	//获取当前目录
	if(getcwd(nameBuffer,sizeof(nameBuffer)))
		printf("getcmd():OK!\t %s\n",nameBuffer);
	else
		perror("getcmd():Failed!-");
	
	printf("\n%s\n",getcwd(nameBuffer,sizeof(nameBuffer)));
	
	
	
	return 0;
}
