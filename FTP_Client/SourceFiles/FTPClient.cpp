#undef UNICODE

#include <iostream>
#include <cstdio>
#include "..\HeaderFiles\FTPClient.h"

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define NAME_FTPClient "FTPClient 1"

using namespace std;

int FTPClient::execute(int argc, char **argv)
{
	SetConsoleTitle(NAME_FTPClient);

	//FTPClient myFTPClient;

	if (argc == 2) {
		getIPServer(argv[1]);
	}

	cout << "Waiting for connecting..." << endl;
	
	connectServer(controlPort, "localhost", controlSocket);
	char *recvbuf = new char[DEFAULT_BUFLEN];
	receiveResponse(this, recvbuf);
	delete[] recvbuf;
	getchar();
	system("cls");

	login("quan", "");
	closeConnect();

	return 0;
}

FTPClient::FTPClient(void) : controlSocket(INVALID_SOCKET), ipADDRESS("localhost"), dataSocket(INVALID_SOCKET)
{
	DEBUG_CONSTRUCTOR
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (iResult != 0) {
		clog << "-> Line " << __LINE__ << " in file " << __FILE__ << ": " << endl << "WSAStartup failed with error: " << iResult << endl;
		closeProgram();
	}
}

FTPClient::~FTPClient(void)
{
	DEBUG_DESTRUCTOR
	closesocket(controlSocket);
	closesocket(dataSocket);
	WSACleanup();
}

void FTPClient::closeProgram(void) {
	cout << "The program have some errors. Please read log for more details!" << endl;
	this->~FTPClient();
	fclose(log);
	exit(1);
}

void FTPClient::getIPServer(char* ipADDRESS)
{
	this->ipADDRESS = ipADDRESS;
}

void FTPClient::connectServer(char *port, char *ipServer, SOCKET &connectSocket) {
	struct addrinfo *result = nullptr, *ptr = nullptr, hints;
	
	// Kết nối sử dụng giao thức IPv4 và TCP/IP
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	int iResult = getaddrinfo(ipServer, port, &hints, &result);

	if (iResult != 0) {
		clog << "-> Line " << __LINE__ << " in file " << __FILE__ << ": " << endl << "getaddrinfo failed with error: " << iResult << endl;
		closeProgram();
	}

	int counter = 0;
	do {
		for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
			// Create a SOCKET for connecting to server
			connectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

			if (connectSocket == INVALID_SOCKET) {
				clog << "-> Line " << __LINE__ << " in file " << __FILE__ << ": " << endl << "socket failed with error: " << WSAGetLastError();
				closeProgram();
			}

			// connect to server.
			iResult = connect(connectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
			if (iResult == SOCKET_ERROR) {
				closesocket(connectSocket);
				connectSocket = INVALID_SOCKET;
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

	if (connectSocket == INVALID_SOCKET) {
		clog << "-> Line " << __LINE__ << " in file " << __FILE__ << ": " << endl << "Unable to connect to server!\n";
		closeProgram();
	}
}

void FTPClient::login(char* username, char* password) {
	int lenUsername = 6 + strlen(username);
	int lenPassword = 6 + strlen(password);
	char *userCommand = new char[lenUsername];
	char *passCommand = new char[lenPassword];
	char *recvbuf = new char[DEFAULT_BUFLEN];

	//Tạo câu lệnh USER và PASS
	strcpy_s(userCommand, lenUsername, "USER ");
	strcat_s(userCommand, lenUsername, username);
	strcpy_s(passCommand, lenPassword, "PASS ");
	strcat_s(passCommand, lenPassword, password);
	if (userCommand[9] == '\0') {
		clog << "-> Line " << __LINE__ << " in file " << __FILE__ << ": " << endl << "YES" << endl;
	}

	//Gửi username và password
	sendCommand(this, userCommand);
	receiveResponse(this, recvbuf);//Thành công trả về mã 331
	sendCommand(this, passCommand);
	receiveResponse(this, recvbuf);//Thành công trả về mã 230

	delete[] userCommand;
	delete[] passCommand;
	delete[] recvbuf;
	getchar();
}

void FTPClient::establishDataChanel(void)
{
	//Khởi tạo data chanel ở chế độ Passive mode
	char *recvbuf = new char[DEFAULT_BUFLEN];
	char parameter[5] = "PASV";
	sendCommand(this, parameter);
	int lenResponse = receiveResponse(this, recvbuf);//Thành công trả về mã 227

	//Kiểm tra mã trả về
	recvbuf[3] = '\0';
	if (strcmp(recvbuf, "227") == 0) {
		int first = 0, second = 0, i = 0;
		recvbuf[lenResponse - 1] = '\0';
		for (i = lenResponse - 2; i > -1; i--) {
			if (recvbuf[i] == ',') {
				recvbuf[i] = '\0';
				second = atoi(&recvbuf[i + 1]);
				break;
			}
		}

		for (; i > -1; i--) {
			if (recvbuf[i] == ',') {
				first = atoi(&recvbuf[i + 1]);
				break;
			}
		}

		snprintf(dataPort, NUMBER_DIGIT_PORT, "%d", (first << 8) + second);
		// Thành lập data chanel
		connectServer(dataPort, "localhost", dataSocket);
	}
	else {
		clog << "-> Line " << __LINE__ << " in file " << __FILE__ << ": " << endl << "Cannot creat data chanel." << endl;
	}
	delete[] recvbuf;
}

void sendCommand(FTPClient *myFTPClient, char* sendbuf)
{
	int result = send(myFTPClient->controlSocket, sendbuf, (int)strlen(sendbuf) + 1, 0);
	if (result == SOCKET_ERROR) {
		clog << "-> Line " << __LINE__ << " in file " << __FILE__ << ": " << endl << "send failed with error: " << WSAGetLastError() << endl;
		myFTPClient->closeProgram();
	}
}

int receiveResponse(FTPClient *myFTPClient, char *recvbuf)
{
	int recvbuflen = DEFAULT_BUFLEN;
	int iResult;

	iResult = recv(myFTPClient->controlSocket, recvbuf, recvbuflen, 0);
	recvbuf[iResult] = '\0';
	
	if (iResult > 0) {
		clog << "-> Line " << __LINE__ << " in file " << __FILE__ << ": " << endl << recvbuf << endl;
	}
	else if (iResult < 0)
	{
		clog << "-> Line " << __LINE__ << " in file " << __FILE__ << ": " << endl << "recv failed with error: " << WSAGetLastError() << endl;
		myFTPClient->closeProgram();
	}

	return iResult;
}

void FTPClient::closeConnect(void)
{
	int iResult = shutdown(controlSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		clog << "-> Line " << __LINE__ << " in file " << __FILE__ << ": " << endl << "shutdown failed with error: " << WSAGetLastError() << endl;
		closeProgram();
	}

	if (dataSocket != INVALID_SOCKET) {
		iResult = shutdown(dataSocket, SD_SEND);
		if (iResult == SOCKET_ERROR) {
			clog << "-> Line " << __LINE__ << " in file " << __FILE__ << ": " << endl << "shutdown failed with error: " << WSAGetLastError() << endl;
			closeProgram();
		}
	}
}
