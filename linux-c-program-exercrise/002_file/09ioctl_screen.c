#include "common.h"
#include <sys/ioctl.h>
#include <linux/fb.h>
//本程序测试linux开发时，遇到不会的东西如何学习，首先知道你要做什么，然后搜你做的这个对应的linux设备驱动，如显卡相关的/dev/fb0，然后百度
//linux fb0,获取你想要的东西
int main(int argc,const char* argv[])
{

	struct fb_var_screeninfo screenInfo;
	//首先打开文件
	int fb = open("/dev/fb0",O_RDWR);
	if(0>fb)
	{
		perror("open /dev/fb0 failed!");
		exit(-1);
	}
	//获取显示屏的大小
	if( ioctl(fb,FBIOGET_VSCREENINFO,&screenInfo) < 0 )
	{
		perror("ioctl TIOCGWINSZ error!");
		exit(-2);
	}
	
	//打印显示屏的大小
	printf("显示屏大小：横像素:%d,竖像素：%d\n",screenInfo.xres,screenInfo.yres);
	//关闭文件
	close(fb);
	return 0;
}
