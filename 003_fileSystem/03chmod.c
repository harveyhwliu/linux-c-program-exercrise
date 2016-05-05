#include "common.h"

int main(){
	
	if(!chmod("abc",0777))
		printf("OK！\n");
	else
		perror("changed failed:");
	return 0;
}
