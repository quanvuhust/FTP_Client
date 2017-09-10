#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <cstdio>
#include "..\HeaderFiles\FTPClient.h"

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define DEFAULT_PORT "21"
#define NAME_FTPClient "FTPClient 1"

using namespace std;

extern int g_PosOutput;
extern int g_PosInput;
extern int g_Columns, g_Rows;


int main(int argc, char **argv)
{
	SetConsoleTitle(NAME_FTPClient);

	FTPClient myFTPClient;

	if (argc == 2) {
		myFTPClient.GetIPServer(argv[1]);
	}

	cout << "Waiting for connecting..." << endl;
	myFTPClient.Connect();
	getchar();
	system("cls");

	//myFTPClient.SendAndReceive();
	myFTPClient.CloseConnect();

	return 0;
}

FTPClient::FTPClient(void) : ConnectSocket(INVALID_SOCKET), result(NULL), ptr(NULL), IP_ADDRESS("localhost")
{
	DEBUG_CONSTRUCTOR
		freopen_s(&log, "FTPClientLog.txt", "a+t", stderr);

	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (iResult != 0) {
		cerr << "WSAStartup failed with error: " << iResult << endl;
		this->~FTPClient();
		exit(1);
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
}

FTPClient::~FTPClient(void)
{
	DEBUG_DESTRUCTOR
		fclose(log);
	closesocket(ConnectSocket);
	WSACleanup();
}

void FTPClient::GetIPServer(char* IP_ADDRESS)
{
	this->IP_ADDRESS = IP_ADDRESS;
}

void FTPClient::Connect(void) {
	// Resolve the server address and port
	iResult = getaddrinfo(IP_ADDRESS, DEFAULT_PORT, &hints, &result);

	if (iResult != 0) {
		cerr << "getaddrinfo failed with error: " << iResult << endl;
		this->~FTPClient();
		exit(1);
	}

	int counter = 0;

	do {
		for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
			// Create a SOCKET for connecting to server
			ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

			if (ConnectSocket == INVALID_SOCKET) {
				cerr << "socket failed with error: " << WSAGetLastError();
				this->~FTPClient();
				exit(1);
			}

			// Connect to server.
			iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
			if (iResult == SOCKET_ERROR) {
				closesocket(ConnectSocket);
				ConnectSocket = INVALID_SOCKET;
				continue;
			}
			counter = 9;
			break;
		}

		counter++;

		if (counter == 10) {
			break;
		}
	} while (1);

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) {
		cerr << "Unable to connect to server!\n";
		system("pause");
		this->~FTPClient();
		exit(1);
	}
	ReceiveData(this);
}

void FTPClient::Login(char* username, char* password) {

}


void SendData(FTPClient *myFTPClient, char sendbuf[DEFAULT_BUFLEN])
{
	myFTPClient->iResult = send(myFTPClient->ConnectSocket, sendbuf, (int)strlen(sendbuf) + 1, 0);
	if (myFTPClient->iResult == SOCKET_ERROR) {
		cerr << "send failed with error: " << WSAGetLastError() << endl;
		myFTPClient->~FTPClient();
		exit(1);
	}
}

void ReceiveData(FTPClient *myFTPClient)
{
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

	// Receive until the peer closes the connection

	myFTPClient->iResult = recv(myFTPClient->ConnectSocket, recvbuf, recvbuflen, 0);
	recvbuf[myFTPClient->iResult] = '\0';

	if (myFTPClient->iResult > 0) {
		cout << recvbuf << endl;
	}
	else if (myFTPClient->iResult < 0)
	{
		cerr << "recv failed with error: " << WSAGetLastError() << endl;
		myFTPClient->~FTPClient();
		exit(1);
	}

}

void FTPClient::CloseConnect(void)
{
	// shutdown the connection since no more data will be sent
	iResult = shutdown(ConnectSocket, SD_SEND);

	if (iResult == SOCKET_ERROR) {
		cerr << "shutdown failed with error: " << WSAGetLastError() << endl;
		this->~FTPClient();
		exit(1);
	}
}
