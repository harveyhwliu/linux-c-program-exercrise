#include "common.h"
#include <sys/ioctl.h>

int main(int argc,const char* argv[])
{

	/*然后我又在termios.h中查看了struct winsize：
	struct winsize {
   	unsigned short ws_row;        //窗口字符行数 
        unsigned short ws_col;         //窗口字符列数
	unsigned short ws_xpixel;     //窗口宽度，像素值
	unsigned short ws_ypixel;     //窗口高度，像素值
	};*/
	struct winsize size;
	if(isatty(STDOUT_FILENO) == 0)//用来判读STDOUT_FILENO是否是一个终端设备，1表示是，0表示不是
		exit(-1);
	//获取终端设备的大小
	if( ioctl(STDOUT_FILENO,TIOCGWINSZ,&size) < 0 )
	{
		perror("ioctl TIOCGWINSZ error!");
		exit(-2);
	}
	
	//打印终端设备的大小
	printf("%d rows,%d columns\n",size.ws_row,size.ws_col);

	return 0;
}
