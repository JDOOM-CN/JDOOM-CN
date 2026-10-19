#include <stdio.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main( int argc, char *argv[] )
{

	printf( "start test...\n" );
	int sockfd=0, ret=0;
	struct sockaddr_in sin;

	sockfd=socket(AF_INET,SOCK_STREAM,0);
	if(sockfd<0)
	{
		perror("create socket error");
		return -1;
	}
	printf( "create sock ok!\n" );

	bzero( (void *)&sin, sizeof(struct sockaddr_in) );
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = inet_addr("202.99.27.222");

	int i=0;

	for(i=1;i<1024;i++){
		usleep(100);
		sin.sin_port = htons(i);
		ret = connect( sockfd, (struct sockaddr *)&sin, sizeof(struct sockaddr_in) );
		if(ret)
			printf("[%d]failed,again...\n", i);
		else{
			printf( "[%d]recall connect ok!\n", i);
			return 0;
		}
	}
	
	printf( "all failed!\n" );
	
	return 0;
} 
