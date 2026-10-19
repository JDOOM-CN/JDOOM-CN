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

char mststr[] = {
0x03, 0x00, 0x00, 0x13, 0x0e, 0xe0, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x01, 0x00, 0x08, 0x00, 0x01, 
0x00, 0x00, 0x00 };

int create_sock()
{
	int sockfd = 0;
	
	sockfd=socket(AF_INET,SOCK_STREAM,0);
	if(sockfd<0)
	{
		perror("create socket error");
		return -1;
	}
	
	return sockfd;
}

int main( int argc, char *argv[] )
{
	int sockfd = 0;
	struct sockaddr_in sin;
	int port_from = 1, port_to = 65535;
	int timeout = 3;
	char recv_buf[128] = {0};
	struct timeval tv;
	int i=0, ret = 0;

	printf( "deepscan by ecawen. v1.0\n\n");
	
	if(argc < 2){
		printf( "usage:%s ip [from] [to] [timeout]\n\n", argv[0]);
		return 0;
	}

	if(argc>2 && argv[2])
		port_from = atoi(argv[2]);

	if(argc>3 && argv[3])
		port_to = atoi(argv[3]);

	if(argc>4 && argv[4])
		timeout = atoi(argv[4]);

	tv.tv_sec = timeout;
	tv.tv_usec = 0;

	bzero( (void *)&sin, sizeof(struct sockaddr_in) );
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = inet_addr(argv[1]);

	//printf("p1=%d, p2=%d, timeout=%d\n", port_from, port_to, timeout);

	for(i=port_from; i<=port_to; i++){
		usleep(10);
		
		sockfd = create_sock();
		if(sockfd<=0)
			continue;
		
		setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
		sin.sin_port = htons(i);
		//printf("scan %d...\n", i);
		ret = connect( sockfd, (struct sockaddr *)&sin, sizeof(struct sockaddr_in) );
		if(ret)
			continue;
		
		ret=send(sockfd, mststr, sizeof(mststr), 0);
		ret=send(sockfd, "\r\n", sizeof("\r\n"), 0);
		ret=recv(sockfd, recv_buf, 128, 0);
		if(ret>=0)
			printf("%s------[%d] open!\n", argv[1], i);
		//else{
		//	printf("%s------[%d] closed, ret=%d\n", argv[1], i, ret);
		//	perror("ERR:");
		//}
		
		close(sockfd);
	}


	printf("\nscan finished!\n");
	return 0;
} 
