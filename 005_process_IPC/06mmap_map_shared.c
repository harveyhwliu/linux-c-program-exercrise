#include "common.h"

int main(int argc,const char* argv[]){
	
	int *p = NULL;
	int nFileLen;
	int fd = open("04hello",O_RDWR);
	if(-1 == fd)
		perror("open:");
	//获取文件大小 lseek,fstat
	nFileLen = lseek(fd,0,SEEK_END);
	printf("FileLen:%d\n",nFileLen);
	p=(int*)mmap(NULL,nFileLen,PROT_READ | PROT_WRITE,MAP_SHARED,fd,0);
	if( MAP_FAILED == p){
		perror("mmap failed:");
		exit(-2);
	}
	p[0]=0x30313233;//该文件开头的原来的数据为0123
	
	//释放掉,防止内存泄露
	munmap(p,nFileLen);
	close(fd);


	return 0;
}
