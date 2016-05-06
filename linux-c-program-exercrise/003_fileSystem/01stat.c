#include "common.h"

int main(int argc,const char* argv[])
{
	struct stat statInfo;
	char access[12]="";
	//参数判断
	if( argc < 2 )
	{
		printf("stat 获取文件inode信息失败！，请输入文件名~");
		exit(-1);
	}
	//获取文献信息
	int nReturn = stat(argv[1],&statInfo);
	if(-1 == nReturn)
	{
		perror("stat file: failed.");
		exit(-2);
	}
	printf("inode:%ld,blocksize:%ld,number of 512B blocks allocates:%ld ",statInfo.st_ino,statInfo.st_blksize,statInfo.st_blocks);
	
//	printf("\t %s\n",statInfo.mode_t);
	
	bzero(access,12);
	//获取文件类型
	access[0]=':';
	access[1]=':';
	 switch (statInfo.st_mode & S_IFMT) {
            case S_IFBLK: access[1]='b';break;
	    case S_IFCHR: access[1]='c';break;
	    case S_IFDIR: access[1]='d';break;
	    case S_IFIFO: access[1]='F';break;
	    case S_IFLNK: access[1]='l';break;
	    case S_IFREG: access[1]='-';break;
	    case S_IFSOCK:access[1]='s';break;
	    default:      access[1]='u';break;
											            }


	//获取文件的访问权限
	if(statInfo.st_mode & S_IRWXU | S_IRUSR)
		access[2]='r';
	else
		access[2]='-';
	if(statInfo.st_mode & S_IRWXU | S_IWUSR)
		access[3]='w';
	else
		access[3]='-';

	if(statInfo.st_mode & S_IRWXU | S_IXUSR)
		access[4]='x';
	else
		access[4]='-';




	printf("flie access 属性：%s\n",access);

	return 0;
}
