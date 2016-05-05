#include "common.h"

int main(int argc,const char*argv[]){
	int nIndex = 0;
	printf("%d\n",argc);
	for(;nIndex < argc;nIndex++){
		printf("%s\n",argv[nIndex]);
	}

	return 0;
}
