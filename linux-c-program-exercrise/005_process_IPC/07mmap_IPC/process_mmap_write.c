#include "common.h"
#define DEF_MMAP_FILE_LEN (0x500)




void sys_error(const char* strDes,int erron){
	perror(strDes);
	exit(erron);
}

int main(int argc,const char* argv[]){
	int fd_mmap_shared,Index=0;
	char* p_mmap;
	//创建进程间通信的mmap共享文件
	fd_mmap_shared=open("mmap.temp",O_RDWR|O_CREAT,0777);
	if(fd_mmap_shared<0)
		sys_error("创建文件失败，请确定是否有权限操作！",-1);
	//保证共享文件大小不能为0
	lseek(fd_mmap_shared,DEF_MMAP_FILE_LEN-1,SEEK_SET);
	write(fd_mmap_shared,"\0",1);
	p_mmap=(char*)mmap(NULL,DEF_MMAP_FILE_LEN,PROT_READ|PROT_WRITE,MAP_SHARED,fd_mmap_shared,0);
	close(fd_mmap_shared);//文件映射创建无论成功与否，后面操作无需直接操作文件了，因为磁盘文件和虚拟内存直接映射了
	if(MAP_FAILED == (void*)p_mmap)
		sys_error("创建文件映射失败！",-2);
	
	//不断的向文件中输入内容
	while(1){
		sprintf(p_mmap,"mmap_%d ",Index++);//这里是覆盖的写一个字段 mmap_索引号
		sleep(2);
	}

	munmap(p_mmap,DEF_MMAP_FILE_LEN);
	return 0;
}
