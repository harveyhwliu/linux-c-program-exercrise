#include "common.h"


int main()
{
	int nReturnValue = fpathconf(STDIN_FILENO,_PC_NAME_MAX);
	printf("%d\n",nReturnValue);
	
	nReturnValue = pathconf("common.h",_PC_NAME_MAX);
	printf("%d\n",nReturnValue);
	
	return 0;
}
