#include "common.h"
//文件锁 flock struct flock{...};,fcntl提供，又称为记录锁，可以锁住文件的某一块
//系统级程序员：变量定义放在开头C89标准；应用程序员，变量随用随定义

int main(){
	struct flock f_lock;//文件锁
	int fd;
	if( 0 > (fd = open("fcntl.tmp",O_CREAT | O_RDWR,0777)) ){
		perror("open failed:");
		exit(-1);
	}
	
	write(fd,"hello world!",sizeof("hello world!"));

	//给这个结构体赋值
	f_lock.l_type = F_WRLCK;//读写锁
	f_lock.l_whence = SEEK_SET;
	f_lock.l_start = 0;
	f_lock.l_len = 0;//0表示整个文件都加锁

	fcntl(fd,F_SETLKW,&f_lock);//阻塞获取文件锁
	printf("上锁\n");
	sleep(10);

	//释放文件锁
	f_lock.l_type = F_UNLCK;//解锁
	fcntl(fd,F_SETLKW,&f_lock);
	printf("解锁\n");
	
	close(fd);

	return 0;
}
