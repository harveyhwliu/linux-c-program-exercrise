#include "common.h"

int main()
{
	char ch=0;
	while((ch=getchar())!=EOF)//Ctrl+d表示结束输入
	{
		putchar(toupper(ch));
	}
	return 0;
}
