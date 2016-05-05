#include "common.h"

int main(int argc,const char* argv[])
{
	int nReturnValue;
	if(argc<3){
		printf("需要两个参数，调用形式如下：./mykill pid sig \n");
		exit(-1);
	}
	
	nReturnValue = kill((pid_t)atoi(argv[1]),atoi(argv[2]));
	if( -1 == nReturnValue  ){
		perror("kill failed!");
		exit(-2);
	}


	return 0;
}
