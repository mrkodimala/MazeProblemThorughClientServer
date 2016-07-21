#include "stdafx.h"
#include <winsock2.h>
#include <windows.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

/*
Global Variables of maxe function
*/
int Arr[100][100] = { { 0, 0, 0, 0, 0, 0 }, { 1, 1, 1, 1, 0, 0 }, { 0, 0, 0, 1, 1, 0 }, { 0, 1, 1, 1, 1, 0 }, { 0, 0, 0, 0, 1, 1 } };
int m, n;
int a1, b1;
int a2, b2;
int pathx[50];
int pathy[50];
int path_offset = 0;
/*
End of global variables
*/


struct node
{
	char msg[128];
	int msg_id;
	node *next;
}*flist,*alist,*printid;

struct bufserv{
	
		int userId;
		int forumId;
		int msgId;
		int commentId;
		int choice;
		char *forumname;
		char msg[128];
}buf1;

bool flag=true;
int mid = 0;
int count1 =0;
char *Data[100];
int count=1;
int values[100];
DWORD WINAPI SocketHandler(void*);
void replyto_client(char *buf, int *csock);

void socket_server() {

	//The port you want the server to listen on
	int host_port= 1101;

	unsigned short wVersionRequested;
	WSADATA wsaData;
	int err;
	wVersionRequested = MAKEWORD( 2, 2 );
 	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 || ( LOBYTE( wsaData.wVersion ) != 2 ||
		    HIBYTE( wsaData.wVersion ) != 2 )) {
	    fprintf(stderr, "No sock dll %d\n",WSAGetLastError());
		goto FINISH;
	}

	//Initialize sockets and set options
	int hsock;
	int * p_int ;
	hsock = socket(AF_INET, SOCK_STREAM, 0);
	if(hsock == -1){
		printf("Error initializing socket %d\n",WSAGetLastError());
		goto FINISH;
	}
	
	p_int = (int*)malloc(sizeof(int));
	*p_int = 1;
	if( (setsockopt(hsock, SOL_SOCKET, SO_REUSEADDR, (char*)p_int, sizeof(int)) == -1 )||
		(setsockopt(hsock, SOL_SOCKET, SO_KEEPALIVE, (char*)p_int, sizeof(int)) == -1 ) ){
		printf("Error setting options %d\n", WSAGetLastError());
		free(p_int);
		goto FINISH;
	}
	free(p_int);

	//Bind and listen
	struct sockaddr_in my_addr;
	my_addr.sin_family = AF_INET ;
	my_addr.sin_port = htons(host_port);
	
	memset(&(my_addr.sin_zero), 0, 8);
	my_addr.sin_addr.s_addr = INADDR_ANY ;
	
	/* if you get error in bind 
	make sure nothing else is listening on that port */
	if( bind( hsock, (struct sockaddr*)&my_addr, sizeof(my_addr)) == -1 ){
		fprintf(stderr,"Error binding to socket %d\n",WSAGetLastError());
		goto FINISH;
	}
	if(listen( hsock, 10) == -1 ){
		fprintf(stderr, "Error listening %d\n",WSAGetLastError());
		goto FINISH;
	}
	
	//Now lets do the actual server stuff

	int* csock;
	sockaddr_in sadr;
	int	addr_size = sizeof(SOCKADDR);
	
	while(true){
		printf("waiting for a connection\n");
		csock = (int*)malloc(sizeof(int));
		
		if((*csock = accept( hsock, (SOCKADDR*)&sadr, &addr_size))!= INVALID_SOCKET ){
			//printf("Received connection from %s",inet_ntoa(sadr.sin_addr));
			CreateThread(0,0,&SocketHandler, (void*)csock , 0,0);
		}
		else{
			fprintf(stderr, "Error accepting %d\n",WSAGetLastError());
		}
	}

FINISH:
;
}

int convertStringtoNum(char *str)
{
	int sum = 0;
	for (int i = 0; str[i] != '\0'; i++)
	{
		sum = sum * 10 + (str[i] - '0');
	}
	return sum;
}

int DecodeBuffer(char *buffer, int *Arr1)
{
	int Arr_offset = 0;
	int length = strlen(buffer);
	char x[10];
	int x_offset = 0;
	for (int i = 0; i < length - 1; i++)
	{
		x[x_offset++] = buffer[i];
		if (buffer[i] == '$')
		{
			x[x_offset - 1] = '\0';
			x_offset = 0;
			Arr1[Arr_offset++] = convertStringtoNum(x);
		}
	}
	return Arr_offset;
}


int checkifValidpoint(int a, int b)
{
	if (a >= 0 && a < m)
	{
		if (b >= 0 && a < n)
		{
			if (Arr[a][b] == 1)
			{
				//int flag = checkpointvisitArrays(a, b);
				int flag = 0;
				if (flag == 0)
				{
					Arr[a][b] = 2;
					return 1;
				}
			}
		}
	}
	return 0;
}


int ispathexist(int a1, int b1, int a2, int b2)
{
	printf("a1=%d\tb1=%d\n", a1, b1);
	//visitx[visit_offset] = a1;
	//visity[visit_offset++] = b1;
	if (Arr[a1][b1] == 0)
	{
		printf("returning in fisrt if");
		return 0;
	}
	if (Arr[a2][b2] == 0)
	{
		printf("returning in second");
		return 0;
	}
	if (a1 == a2&&b1 == b2)
	{
		printf("Both are equal\n");
		//printf("%d\t%d\n", a1, b1);
		//pathx[path_offset] = a1;
		//pathy[path_offset++] = b1;
		return 1;
	}
	int flag;
	flag = checkifValidpoint(a1 + 1, b1);
	//printf("%d", flag);
	if (flag == 1)
	{
		//printf("left ");
		int k = ispathexist(a1 + 1, b1, a2, b2);
		if (k == 1)
		{
			printf("%d\t%d\n", a1 + 1, b1);
			pathx[path_offset] = a1 + 1;
			pathy[path_offset++] = b1;
			return k;
		}
	}
	flag = checkifValidpoint(a1 - 1, b1);
	//printf("%d", flag);
	if (flag == 1)
	{
		//printf("right\n");
		int k = ispathexist(a1 - 1, b1, a2, b2);
		if (k == 1)
		{
			printf("%d\t%d\n", a1 - 1, b1);
			pathx[path_offset] = a1 - 1;
			pathy[path_offset++] = b1;
			return k;
		}
	}
	flag = checkifValidpoint(a1, b1 + 1);
	//printf("%d", flag);
	if (flag == 1)
	{
		//printf("top\n");
		int k = ispathexist(a1, b1 + 1, a2, b2);
		if (k == 1)
		{
			printf("%d\t%d\n", a1, b1 + 1);
			pathx[path_offset] = a1;
			pathy[path_offset++] = b1 + 1;
			return k;
		}
	}
	flag = checkifValidpoint(a1, b1 - 1);
	//printf("%d", flag);
	if (flag == 1)
	{
		//printf("bottom\n");
		int k = ispathexist(a1, b1 - 1, a2, b2);
		if (k == 1)
		{
			printf("%d\t%d\n", a1, b1 - 1);
			pathx[path_offset] = a1;
			pathy[path_offset++] = b1 - 1;
			return k;
		}
	}
	return 0;
}

int addnumber(int n, char *buffer)
{
	int length = strlen(buffer);
	int r;
	if (n == 0)
	{
		buffer[length++] = '0';
		buffer[length++] = '$';
		buffer[length] = '\0';
		return length;
	}
	while (n > 0)
	{
		r = n % 10;
		buffer[length++] = char(r + '0');
		n = n / 10;
	}
	buffer[length++] = '$';
	buffer[length] = '\0';
	return length;
}

int converStringtoresultBuffer(char *buffer)
{
	buffer[0] = '\0';
	addnumber(path_offset+1, buffer);
	for (int i = 0; i < path_offset; i++)
	{
		addnumber(pathx[i], buffer);
		addnumber(pathy[i], buffer);
	}
	addnumber(a1, buffer);
	addnumber(b1, buffer);
	printf("%s", buffer);
	return 0;
}

int loadtoMazeValues(int *mazeArr, int length)
{
	m = mazeArr[0];
	n = mazeArr[1];
	int maze_offset = 2;
	//printf("m=%d\tn=%d\n", m, n);
	printf("\n\n");
	for (int i = 0; i < m; i++)
	{
		for (int j = 0; j < n; j++)
		{
			Arr[i][j] = mazeArr[maze_offset++];
			printf("%d\t", Arr[i][j]);
		}
		printf("\n");
	}
	maze_offset++;
	a1 = mazeArr[maze_offset++];
	b1 = mazeArr[maze_offset++];
	a2 = mazeArr[maze_offset++];
	b2 = mazeArr[maze_offset++];
	printf("a1=%d\tb1=%d\ta2=%d\tb2=%d\n", a1, b1, a2, b2);
	return 0;
}

void process_input(char *recvbuf, int recv_buf_cnt, int* csock) 
{

	char replybuf[1024]={'\0'};
	printf("%s",recvbuf);
	//printf("mahendar Reddy\n");
	int Arr_decode[200];
	int length = DecodeBuffer(recvbuf, Arr_decode);
	loadtoMazeValues(Arr_decode, length);
	printf("m=%d\tn=%d\n", m, n);
	for (int i = 0; i < m; i++)
	{
		for (int j = 0; j < n; j++)
		{
			printf("%d\t", Arr[i][j]);
		}
		printf("\n");
	}
	printf("a1=%d\tb1=%d\ta2=%d\tb2=%d\n", a1, b1, a2, b2);
	int flag = ispathexist(a1, b1, a2, b2);
	char buffer[256];
	if (flag == 1)
	{
		converStringtoresultBuffer(buffer);
	}
	else{
		addnumber(0, buffer);
	}
	printf("%s", buffer);
	replyto_client(buffer, csock);
	replybuf[0] = '\0';
}

void replyto_client(char *buf, int *csock) {
	int bytecount;
	
	if((bytecount = send(*csock, buf, strlen(buf), 0))==SOCKET_ERROR){
		fprintf(stderr, "Error sending data %d\n", WSAGetLastError());
		free (csock);
	}
	printf("replied to client: %s\n",buf);
}

DWORD WINAPI SocketHandler(void* lp){
    int *csock = (int*)lp;

	char recvbuf[1024];
	int recvbuf_len = 1024;
	int recv_byte_cnt;

	memset(recvbuf, 0, recvbuf_len);
	if((recv_byte_cnt = recv(*csock, recvbuf, recvbuf_len, 0))==SOCKET_ERROR){
		fprintf(stderr, "Error receiving data %d\n", WSAGetLastError());
		free (csock);
		return 0;
	}

	printf("Received bytes %d\nReceived string \"%s\"\n", recv_byte_cnt, recvbuf);
	process_input(recvbuf, recv_byte_cnt, csock);
	//replyto_client("Helloworld", csock);
	//closesocket(*csock);
    return 0;
}