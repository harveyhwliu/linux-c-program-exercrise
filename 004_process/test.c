#include <stdio.h>

int main(int argc,const char* argv[])
{
	int nIndex = 0;
	while( argc-- ){
		printf("%s\t",argv[nIndex++]);
	}
	printf("\n");

	return 23;
}
