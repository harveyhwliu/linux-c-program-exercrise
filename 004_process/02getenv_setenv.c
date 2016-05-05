#include "common.h"


int main(){
	//使用char** environ变量获取环境变量使用不方便
	//使用getenv()获得关心的环境变量，使用setenv设置需要的环境变量
	if(getenv("PATH"))
		printf("PATH=%s\n",getenv("PATH"));
	else
		perror("not find environment variable:PATH-");

	
	setenv("PATH","HELLO",1);
	printf("PATH=%s\n",getenv("PATH"));


	return 0;
}
