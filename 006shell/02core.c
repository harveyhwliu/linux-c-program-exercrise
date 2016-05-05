#include "common.h"
//ubuntu默认不开启core文件，使用ulimit -a 查看，设置ulimit -c 1024,生成在程序宕机前保存寄存器的值，内存的值等的core文件，帮助调试bug
int main(){
	char* str = "hello";//不能修改字符常量区的内容
	printf("%s\n",str);
	*str = 'H';//会出错
	printf("%s\n",str);

	return 0;
}
