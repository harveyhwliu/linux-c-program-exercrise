#include "common.h"
#define MAXLINE 80
#define SERVER_PORT 8123

int main(){

	struct sockaddr_in serveraddr;
	char buf[MAXLINE];
	int sockfd,n;

	sockfd = socket(AF_INET,SOCK_STREAM,0);
	bzero(&serveraddr,sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	inet_pton(AF_INET,"127.0.0.1",&serveraddr.sin_addr);
	serveraddr.sin_port = htons(SERVER_PORT);

	connect(sockfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
	
	bzero(buf,sizeof(buf));
	while( fgets(buf,MAXLINE,stdin) ){
		write(sockfd,buf,strlen(buf));
		n = read(sockfd,buf,MAXLINE);
		if( !n ){
			printf("the other side has been closed:\n");
			close(sockfd);
			return (1);
		}
		else
			write(STDOUT_FILENO,buf,n);
	}
	
	close(sockfd);
	return 0;
}
