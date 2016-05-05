#include "common.h"
//打开一个文件，目的就是将这个文件中的所有小写字母变成大写后输出到屏幕

int main(int argc,const char* argv[])
{
	//校验参数
	if(argc<2){
		printf("必须输入文件名！\n");
		exit(-1);
	}
	
	//打开一个文件，目的就是将这个文件中的所有小写字母变成大写后输出到屏幕
	int fd = open(argv[1],O_RDONLY);
	if(-1==fd){
		perror("打开文件失败！-");
		exit(-2);
	}
	
	//重定向标准输入文件描述符
	int save_fd = dup(STDIN_FILENO);//用户重新恢复标准输入从屏幕（键盘）读取的
	dup2(fd,STDIN_FILENO);
	close(fd);
	
	//exec调用小写变大写后输出到屏幕的应用程序
	execl("./06upper","",(char*)NULL);
	perror("06upper failed:");
	//---下面的代码在exec成功调用的时候是不会调用的了

	//重新将输入文件描述符重定向到屏幕
	dup2(STDIN_FILENO,save_fd);
	putchar(getchar());
	
	//关闭文件描述符
	close(save_fd);


	return 0;	
}
