#include "common.h"
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
	int fd_mmap_shared,mmap_file_size;
	struct struct_stu* p_mmap;
	//创建进程间通信的mmap共享文件
	fd_mmap_shared=open("mmap.temp",O_RDWR|O_CREAT,0777);
	if(fd_mmap_shared<0)
		sys_error("创建文件失败，请确定是否有权限操作！",-1);
	//获取文件大小，保证共享文件大小不能为0
	mmap_file_size = lseek(fd_mmap_shared,SEEK_END,0);
	if(mmap_file_size < 1){
		close(fd_mmap_shared);
		sys_error("mmap共享文件大小不能为0",-2);
	}

	p_mmap=(struct struct_stu*)mmap(NULL,mmap_file_size,PROT_READ|PROT_WRITE,MAP_SHARED,fd_mmap_shared,0);
	close(fd_mmap_shared);//文件映射创建无论成功与否，后面操作无需直接操作文件了，因为磁盘文件和虚拟内存直接映射了
	//共享内存文件应该是一个临时文件，所以需要unink	
	if(unlink("./mmap.temp")==-1)
		sys_error("共享内存文件设置为临时文件失败",-3);

	if(MAP_FAILED == (void*)p_mmap)
		sys_error("创建文件映射失败！",-2);
	
	//不断的向文件中输入内容
	while(1){
		printf("id:%d,name:%s,sex:%c\n",p_mmap->id,p_mmap->name,p_mmap->sex);//这里是覆盖的写一个字段 mmap_索引号
		sleep(1);
	}

	munmap(p_mmap,mmap_file_size);
	return 0;
}
