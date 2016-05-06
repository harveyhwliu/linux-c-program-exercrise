#include "common.h"
#define DEF_MMAP_FILE_LEN (0x500)
//通信，自定义消息结构体
struct struct_stu{
	int id;
	char name[20];
	char sex;
};

void sys_error(const char* strDes,int erron){
	perror(strDes);
	exit(erron);
}

int main(int argc,const char* argv[]){
	int fd_mmap_shared,Index=0;
	struct struct_stu *p_mmap;
	//创建进程间通信的mmap共享文件
	fd_mmap_shared=open("mmap.temp",O_RDWR|O_CREAT,0777);
	if(fd_mmap_shared<0)
		sys_error("创建文件失败，请确定是否有权限操作！",-1);
	//保证共享文件大小不能为0
	lseek(fd_mmap_shared,DEF_MMAP_FILE_LEN-1,SEEK_SET);
	write(fd_mmap_shared,"\0",1);
	p_mmap=(struct struct_stu*)mmap(NULL,DEF_MMAP_FILE_LEN,PROT_READ|PROT_WRITE,MAP_SHARED,fd_mmap_shared,0);
	close(fd_mmap_shared);//文件映射创建无论成功与否，后面操作无需直接操作文件了，因为磁盘文件和虚拟内存直接映射了
	if(MAP_FAILED == (void*)p_mmap)
		sys_error("创建文件映射失败！",-2);
	
	//不断的向文件中输入内容,覆盖的写入同一段地址
	while(1){
		p_mmap->id = Index;
		sprintf(p_mmap->name,"zhang-%d",Index);
		if(Index%2)
			p_mmap->sex='w';
		else
			p_mmap->sex='m';
		Index++;
		sleep(2);
	}
	//共享内存应该是一个临时文件，即需要unlink
	munmap(p_mmap,DEF_MMAP_FILE_LEN);
	return 0;
}
