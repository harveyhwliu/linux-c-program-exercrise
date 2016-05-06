#include "common.h"

int main()
{
	//命令参数的指针数组
	char* argv[] = {"ls","-l","./"};

	printf("Hello\n");
//	此时由于调用新的应用，程序分支进入指定的程序中，即execl以下的程序不会再被执行（代码被execl的覆盖了），因此程序是从execl退出的，可以查看本程序的退出码
//	execl("/bin/ls","ls","-l","./",(char*)NULL);
//	execl("./ExecTestApp","./ExecTestApp","1","a",(char*)NULL);
//	execlp("ls","ls","-l","./",(char*)NULL);//execlp 相比于execl，不用给出应用名称的全部地址，只需给出应用名称即可
	
//	execv("/bin/ls",argv);//v的意思是参数给定是通过char*指针数组给定，而不是通过命令参数列表
//	execvp("ls",argv);
//le,ve的区别在于e,就是通过自己指定的环境变量中查找应用程序


	//2、这里的第二个ls就是argv[0]的第一个参数，这里只是起到占位的作用，因为应用程序是通过第一个参数/bin/ls指定了，所以ls可以是其他，但不能省略
	//execl("/bin/ls","ls","-l","./",(char*)NULL);
	execl("/bin/ls","","-l","./",(char*)NULL);


	perror("failed;");//exec族调用成功，应用程序不会继续往下执行，exec执行失败则往下继续执行
	exit(-1);


	printf("World\n");
	return 0;
}
