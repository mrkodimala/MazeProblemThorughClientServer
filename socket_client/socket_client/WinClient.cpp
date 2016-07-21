#include <winsock2.h>
#include <Stdafx.h>
#include <windows.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <conio.h>

int getsocket()
{
	int hsock;
	int * p_int ;
	hsock = socket(AF_INET, SOCK_STREAM, 0);
	if(hsock == -1){
		printf("Error initializing socket %d\n",WSAGetLastError());
		return -1;
	}
	
	p_int = (int*)malloc(sizeof(int));
	*p_int = 1;
	if( (setsockopt(hsock, SOL_SOCKET, SO_REUSEADDR, (char*)p_int, sizeof(int)) == -1 )||
		(setsockopt(hsock, SOL_SOCKET, SO_KEEPALIVE, (char*)p_int, sizeof(int)) == -1 ) ){
		printf("Error setting options %d\n", WSAGetLastError());
		free(p_int);
		return -1;
	}
	free(p_int);

	return hsock;
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

int DecodeBuffer1(char *buffer, int *Arr1)
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
			//printf("%d\t", Arr[Arr_offset - 1]);
		}
	}
	//printf("\n\n");
	return Arr_offset;
}

int writeToFile(char *filename, char *buffer)
{
	FILE *file;
	errno_t e= fopen_s(&file, filename, "a");
	if (e != 0)
		return 0;
	int D[100];
	int length = DecodeBuffer1(buffer, D);
	int D_offset = 1;
	int size = D[0];
	if (size == 0)
	{
		fprintf(file, "\nPath does not exist\n");
		fclose(file);
	}
	printf("\nsize=%d\n", size);
	fprintf(file,"\n=======Path=======\n ");
	while (size > 0)
	{
		fprintf(file,"%d\t", D[D_offset++]);
		fprintf(file,"%d\n", D[D_offset++]);
		size--;
	}
	fclose(file);
	return 0;
}

void loadfromfile(char *filename, char *buffer)
{
	FILE *file;
	errno_t e = fopen_s(&file, filename, "r+");
	//printf("%d", e);
	//char buffer[256];
	int buffer_offset = 0;
	int q, r;
	if (e == 0)
	{
		char ch;
		while (!feof(file))
		{
			ch = fgetc(file);
			printf("%c", ch);
			if (ch<48 || ch>57)
			{
				buffer[buffer_offset++] = '$';
			}
			else{
				buffer[buffer_offset++] = ch;
			}
		}
		buffer[buffer_offset] = '\0';
		printf("%s", buffer);
		fclose(file);
	}
	else{
		printf("file not opened");
	}
}


void socket_client()
{

	//The port and address you want to connect to
	int host_port= 1101;
	char* host_name="127.0.0.1";

	//Initialize socket support WINDOWS ONLY!
	unsigned short wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD( 2, 2 );
 	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 || ( LOBYTE( wsaData.wVersion ) != 2 ||
		    HIBYTE( wsaData.wVersion ) != 2 )) {
	    fprintf(stderr, "Could not find sock dll %d\n",WSAGetLastError());
		return;
	}

	//Initialize sockets and set any options

	//Connect to the server
	struct sockaddr_in my_addr;

	my_addr.sin_family = AF_INET ;
	my_addr.sin_port = htons(host_port);
	
	memset(&(my_addr.sin_zero), 0, 8);
	my_addr.sin_addr.s_addr = inet_addr(host_name);

	//if( connect( hsock, (struct sockaddr*)&my_addr, sizeof(my_addr)) == SOCKET_ERROR ){
	//	fprintf(stderr, "Error connecting socket %d\n", WSAGetLastError());
	//	goto FINISH;
	//}

	//Now lets do the client related stuff
	char buffer[1024];
	int buffer_len = 1024;
	int bytecount;
	int c;

	while(true) {

		int hsock = getsocket();
		//add error checking on hsock...
		if( connect(hsock, (struct sockaddr*)&my_addr, sizeof(my_addr)) == SOCKET_ERROR ){
			fprintf(stderr, "Error connecting socket %d\n", WSAGetLastError());
			return;
		}

		memset(buffer, '\0', buffer_len);
		getchar();
		printf("Enter your message to send here\n");
		loadfromfile("mazefile.txt", buffer);
		if( (bytecount=send(hsock, buffer, strlen(buffer),0))==SOCKET_ERROR){
			fprintf(stderr, "Error sending data %d\n", WSAGetLastError());
			return;
		}
		printf("Sent bytes %d\n", bytecount);

		if((bytecount = recv(hsock, buffer, buffer_len, 0))==SOCKET_ERROR){
			fprintf(stderr, "Error receiving data %d\n", WSAGetLastError());
			return;
		}
		//printf("Recieved bytes %d\nReceived string \"%s\"\n", bytecount, buffer);
		writeToFile("mazefile.txt", buffer);
		printf("%s\n",buffer);

		closesocket(hsock);
	}

	//closesocket(hsock);
}