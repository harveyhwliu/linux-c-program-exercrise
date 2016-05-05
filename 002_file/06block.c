#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE (8*1024)

//从终端读并显示
int main(int argc , const char* argv[])
{
	char buffer[BUF_SIZE]="";
	int nReadCount = -1;
	bzero(buffer,sizeof(buffer));
	nReadCount = read(STDIN_FILENO,buffer,sizeof(buffer));
	if(0 < nReadCount ){
		write(STDOUT_FILENO,buffer,nReadCount);
	}
	printf("\n");
	return 0;
}
