#include "common.h"
#define CLI_PATH "/var/tmp/" /* +5 for pid = 14 chars */
//
int cli_conn(const char *name)
{
	int fd, len, err, rval;
	struct sockaddr_un un;
	/* 创建UNIX domain stream socket */
	if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
		return(-1);
	/* 初始化本地端地址 */
	memset(&un, 0, sizeof(un));
	un.sun_family = AF_UNIX;
	sprintf(un.sun_path, "%s%05d", CLI_PATH, getpid());
	len = offsetof(struct sockaddr_un, sun_path) + strlen(un.sun_path);
	unlink(un.sun_path); /* in case it already exists */
	//服务器端和客户端都需要绑定UNIX domain stream socket 的地址
	if (bind(fd, (struct sockaddr *)&un, len) < 0) {
		rval = -2;
		goto errout;
	}
	/*初始化服务器短地址 */
	memset(&un, 0, sizeof(un));
	un.sun_family = AF_UNIX;
	strcpy(un.sun_path, name);
	len = offsetof(struct sockaddr_un, sun_path) + strlen(name);
	//连接服务器端
	if (connect(fd, (struct sockaddr *)&un, len) < 0) {
		rval = -4;
		goto errout;
	}
	return(fd);
errout:
	err = errno;
	close(fd);
	errno = err;
	return(rval);
}
int main(void)
{
	int fd;
	char buf[1024];
	fd = cli_conn("foo.socket");
	if (fd < 0) {
		switch (fd) {
			case -4:perror("connect"); break;
			case -3:perror("listen"); break;
			case -2:perror("bind"); break;
			case -1:perror("socket"); break;
		}
		exit(-1);
	}
	while (fgets(buf, sizeof(buf), stdin) != NULL) {
		write(fd, buf, strlen(buf));
		read(fd, buf, sizeof(buf));
		//write(STDOUT_FILENO, buf, n);
		printf("received datas:%s\n",buf);
	}
	close(fd);
	return 0;
}
