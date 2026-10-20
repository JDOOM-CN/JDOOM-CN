// sockscan.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <stdlib.h>  ////atoi
#include <string.h>  ////strcpy
#include <windows.h> ////CreateThread
#include <assert.h>  ////assert

#pragma comment(lib, "ws2_32.lib")  ////sock

int timeout = 3;
int thread_num = 10;

typedef struct _IPPORT
{
	char ip[16]; ////ip
	int port; ////port
} IPPORT, *PIPPORT;

int jw_split(char *in, const char *delim, char *out[])
{
	char *p = NULL;
	int i = 0;

	p = strtok(in, delim);
	while(p){
		///printf("p=%s\n", p);
		out[i] = new char[16];
		strncpy(out[i], p, 16);
		i ++;
		p=strtok(NULL, delim);
	}

	return i;
}

DWORD WINAPI scan_conn(LPVOID in)
{ 
	int sockfd=0, ret=0;

	//printf("ip=%s, port=%d\n", PIPPORT(in)->ip,  PIPPORT(in)->port);

	
	WSADATA wd;
	WSAStartup(MAKEWORD( 1, 1 ), &wd);  //win下socket要初始化才能使用
	struct sockaddr_in sin;
	///printf("working ... ip=%s, port=%d\n", PIPPORT(in)->ip,  PIPPORT(in)->port);
	sockfd=socket(AF_INET,SOCK_STREAM,0);
	
	if(sockfd<0)
	{
		perror("create socket error");
		return -1;
	}
	
	memset( (void *)&sin, 0, sizeof(struct sockaddr_in) );
	sin.sin_family = AF_INET;

	sin.sin_addr.s_addr = inet_addr(PIPPORT(in)->ip);
	sin.sin_port = htons(PIPPORT(in)->port);

	int timeout_sock = timeout * 1000; //win下sock参数1000为1秒
	setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout_sock, sizeof(int));

	
	ret = connect( sockfd, (struct sockaddr *)&sin, sizeof(struct sockaddr_in) );
	if(ret){
		//perror( "Connect error!" );
		closesocket(sockfd);
		return -1;
	}

	printf("%d open!\n", PIPPORT(in)->port);
	
	return sockfd;
}

int main(int argc, char* argv[])
{
	char *iparr[16] = {0};
	char *portarr[16] = {0};
	int i = 0;
	int ret = 0;

	char ip_start[16]={0};
	char ip_end[16]={0};

	char port_start[16]={0};
	char port_end[16]={0};

	PIPPORT pData;
	HANDLE hThrds[100];
	int pIdx = 0, tIdx = 0;
	DWORD rc;


	if( argc < 3 )
	{
		printf( "sockscan by eca. v1.0\n" );
		printf( "Usage: %s <ip> <port> [timeout]\n", argv[0] );
		printf( "Options:\n" );
		printf( " <ip>	---ip to scan.\n" );
		printf( " <port>	---port to scan.\n" );
		printf( " <thread>	---thread num.\n" );
		printf( " <timeout>	---default is 5s.\n" );
		printf( "\n" );
		return 1;
	}

	if(argv[3])
		thread_num = atoi(argv[3]);
	
	if(argv[4])
		timeout = atoi(argv[4]);
	
	////deal ip
	ret = jw_split(argv[1], "-", iparr);
	///printf("ret=%d\n", ret);
	for (i=0; i<ret; i++){
		if ( i == 0)
			strncpy(ip_start, iparr[0], 16);
		if ( i == 1)
			strncpy(ip_end, iparr[1], 16);
	}

	///deal port
	ret = jw_split(argv[2], "-", portarr);
	//printf("ret=%d\n", ret);
	for (i=0; i<ret; i++){
		if ( i == 0)
			strncpy(port_start, portarr[0], 16);
		if ( i == 1)
			strncpy(port_end, portarr[1], 16);
	}

	////printf("ipstart=%s, ipend=%s, portstart=%s, portend=%s\n", ip_start, ip_end, port_start, port_end);
	for (i = atoi(port_start); i<= atoi(port_end); i++ ){
		if ( i - atoi(port_start) > thread_num ){
			rc = WaitForMultipleObjects(thread_num, hThrds, FALSE, INFINITE); // 等待hThrds数组中任意一个变为激发状态，返回其索引
            pIdx = rc - WAIT_OBJECT_0;
            assert(pIdx >= 0 && pIdx < thread_num);
            ///printf("%d terminated\n", pIdx);
            CloseHandle(hThrds[pIdx]); 
		}
		
		//printf("i=%d\n", i);
		pData = (PIPPORT)HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY, sizeof(IPPORT));
        if(pData == NULL)
			ExitProcess(2);

		strncpy(pData->ip, ip_start, 16);
		pData->port = i;

		hThrds[pIdx++] = CreateThread(NULL, 0, scan_conn, pData, 0, 0);
	}

	WaitForMultipleObjects(thread_num, hThrds, TRUE, INFINITE); // 等待hThrds数组中所有线程变为激发状态
    for (pIdx = 0; pIdx < thread_num; pIdx++)
        CloseHandle(hThrds[pIdx]);

	return 0;
}

