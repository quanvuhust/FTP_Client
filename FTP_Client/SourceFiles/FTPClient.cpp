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

	if (argc == 2) {
		getIPServer(argv[1]);
	}

	cout << "Waiting for connecting..." << endl;
	
	connectServer(controlPort, "localhost", controlSocket);
	char *recvbuf = new char[DEFAULT_BUFLEN];
	receiveResponse(recvbuf);
	delete[] recvbuf;

	login("anonymous", "");
	
	download("C:\\Users\\Dell\\Downloads\\TestFTP\\New", "\\New", 2);
	closeConnect(controlSocket);

	return 0;
}

FTPClient::FTPClient(void) : controlSocket(INVALID_SOCKET), ipADDRESS("localhost")
{
	DEBUG_CONSTRUCTOR

	mutex = CreateMutex(NULL, FALSE, NULL);
	
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
	closeConnect(controlSocket);
	CloseHandle(mutex);
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
	sendCommand(userCommand);
	receiveResponse(recvbuf);//Thành công trả về mã 331
	sendCommand(passCommand);
	receiveResponse(recvbuf);//Thành công trả về mã 230

	delete[] userCommand;
	delete[] passCommand;
	delete[] recvbuf;
}

void FTPClient::establishDataChanel(SOCKET &dataSocket, char dataPort[])
{
	//Khởi tạo data chanel ở chế độ Passive mode
	char *recvbuf = new char[DEFAULT_BUFLEN];

	int lenResponse = sendAndReceive("PASV", recvbuf);//Thành công trả về mã 227

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

void FTPClient::list(void) {
	SOCKET dataSocket = INVALID_SOCKET;
	char dataPort[NUMBER_DIGIT_PORT];
	/*Thiết lập data chanel*/
	establishDataChanel(dataSocket, dataPort);
	/*Hàm này viết tạm, về sau sẽ cho hàm này hoạt động trên 1 Thread riêng*/
	if (dataSocket != INVALID_SOCKET) {
		char *recvbuf = new char[DEFAULT_BUFLEN];
		
		sendCommand("MLSD");
		receiveResponse(recvbuf);

		int iResult = 1;

		while (iResult > 0) {
			iResult = recv(dataSocket, recvbuf, DEFAULT_BUFLEN, 0);
			recvbuf[iResult] = '\0';
			cout << recvbuf;
		}

		delete[] recvbuf;
		closeConnect(dataSocket);
	}
}

void FTPClient::setTransferMode(const int type) {
	setTransferMode(BINARY);
	int lenCommand = 8;
	char *command = new char[lenCommand];
	char *recvbuf = new char[DEFAULT_BUFLEN];
	strcpy_s(command, lenCommand, "TYPE ");

	if (type == ASCII) {
		strcat_s(command, lenCommand, "A");
	}
	else if (type == BINARY) {
		strcat_s(command, lenCommand, "I");
	}

	sendCommand(command);
	receiveResponse(recvbuf);

	delete[] recvbuf;
}

void FTPClient::changeDirectory(char *newPath) {
	if (newPath[0] == '\0') {
		return;
	}
	char *recvbuf = new char[DEFAULT_BUFLEN];
	int lenCommand = 5 + strlen(newPath);
	char *command = new char[lenCommand];
	strcpy_s(command, lenCommand, "CWD ");
	strcat_s(command, lenCommand, newPath);

	sendCommand(command);
	receiveResponse(recvbuf);
	
	recvbuf[3] = '\0';
	if (strcmp(recvbuf, "250") == 0) {
		printf("%s\n", &recvbuf[4]);
		getchar();
	}
	else if (strcmp(recvbuf, "550") == 0){
		printf("%s\n", &recvbuf[4]);
		getchar();
	}
	delete[] recvbuf;
	delete[] command;
}

void FTPClient::printCurrentDirectory(void) {
	char *recvbuf = new char[DEFAULT_BUFLEN];

	sendCommand("PWD");
	receiveResponse(recvbuf);

	printf("%s\n", &recvbuf[4]);
	getchar();
	
	delete[] recvbuf;
}

long FTPClient::getFileSize(char *fileName) {
	char *recvbuf = new char[DEFAULT_BUFLEN];
	int lenCommand = 6 + strlen(fileName);
	char *command = new char[lenCommand];

	strcpy_s(command, lenCommand, "SIZE ");
	strcat_s(command, lenCommand, fileName);

	sendCommand(command);
	receiveResponse(recvbuf);

	long result = atol(&recvbuf[4]);
	delete[] recvbuf;
	delete[] command;

	return result;
}

void downloadThread(const DownloadInformation * const inf) {
	HANDLE file = CreateFile(inf->destination, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	SetFilePointer(file, inf->start, NULL, FILE_BEGIN);
	
	char *recvbuf = new char[FTPClient::BLOCK_SIZE];
	int iResult = 0, count = 0, sum = 0;
	
	while (count < inf->numberBlocks) {
		while (iResult <= 0) {
			iResult = recv(inf->dataSocket, recvbuf, FTPClient::BLOCK_SIZE, 0);
		}
		
		sum += iResult;
		WriteFile(file, recvbuf, iResult, NULL, NULL);
		count++;
	}
	cout << sum << endl;
	//inf->client->closeConnect(inf->dataSocket);

	delete[] recvbuf;
	CloseHandle(file);
}

void FTPClient::downloadSegment(HANDLE *h, DownloadInformation **inf, int index, char* source) {
	SOCKET dataSocket = INVALID_SOCKET;
	char dataPort[NUMBER_DIGIT_PORT];
	/*Thiết lập data chanel*/
	
	establishDataChanel(dataSocket, dataPort);
	
	if (dataSocket != INVALID_SOCKET) {	
		inf[index]->dataSocket = dataSocket;
		
		char *recvbuf = new char[DEFAULT_BUFLEN];
		int lenCommand = 6 + strlen(source);
		char *command = new char[lenCommand];

		strcpy_s(command, lenCommand, "REST ");
		snprintf(&command[5], lenCommand, "%d", inf[index]->start);
		
		sendAndReceive(command, recvbuf);

		strcpy_s(command, lenCommand, "RETR ");
		strcat_s(command, lenCommand, source);

		sendAndReceive(command, recvbuf);
		DWORD id;
		h[index] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)downloadThread, inf[index], FALSE, &id);

		delete[] command;
		delete[] recvbuf;
	}
}

void FTPClient::download(char *destination, char *source, const int numberThread)
{
	FILE *fileDownLoad = nullptr;
	fopen_s(&fileDownLoad, destination, "w+b");
	if (fileDownLoad == nullptr) {
		cout << "No such file or directory" << endl;
		return;
	}
	fclose(fileDownLoad);
	
	HANDLE *h = new HANDLE[numberThread];
		
	DownloadInformation **inf = new DownloadInformation*[numberThread];
	int numberBlocks = getFileSize(source) / BLOCK_SIZE + 1;
	int startPosition = 0, i = 0;
	int numberBlocksPerThread = numberBlocks / numberThread;
	int offset = numberBlocksPerThread * BLOCK_SIZE;

	for (i = 0; i < numberThread - 1; i++) {
		inf[i] = new DownloadInformation(destination, startPosition, numberBlocksPerThread, INVALID_SOCKET, this);
		downloadSegment(h, inf, i, source);
		startPosition += offset;
	}
	
	numberBlocksPerThread = numberBlocks - numberBlocksPerThread*(numberThread - 1);
	inf[i] = new DownloadInformation(destination, startPosition, numberBlocksPerThread, INVALID_SOCKET, this);
	
	downloadSegment(h, inf, i, source);
	
	WaitForMultipleObjects(numberThread, h, TRUE, INFINITE);
	/*
	fopen_s(&fileDownLoad, destination, "a+b");
	fseek(fileDownLoad, getFileSize(source), SEEK_SET);
	int buf = EOF;
	fwrite(&buf, 1, 1, fileDownLoad);
	fclose(fileDownLoad);
	*/	
	for (int i = 0; i < numberThread; i++) {
		delete inf[i];
	}
	delete[] inf;

	for (int i = 0; i < numberThread; i++) {
		CloseHandle(h[i]);
	}
	delete[] h;
	cout << "Download completed" << endl;
	getchar();
}

void FTPClient::upload(char *destination, char *source)
{

}

void FTPClient::sendCommand(char* sendbuf)
{
	int result = send(controlSocket, sendbuf, (int)strlen(sendbuf) + 1, 0);

	if (result == SOCKET_ERROR) {
		clog << "-> Line " << __LINE__ << " in file " << __FILE__ << ": " << endl << "send failed with error: " << WSAGetLastError() << endl;
		closeProgram();
	}
}

int FTPClient::receiveResponse(char *recvbuf)
{
	int recvbuflen = DEFAULT_BUFLEN;
	int iResult;
	
	iResult = recv(controlSocket, recvbuf, recvbuflen, 0);
	recvbuf[iResult] = '\0';
	
	if (iResult > 0) {
		clog << "-> Line " << __LINE__ << " in file " << __FILE__ << ": " << endl << recvbuf << endl;
	}
	else if (iResult < 0)
	{
		clog << "-> Line " << __LINE__ << " in file " << __FILE__ << ": " << endl << "recv failed with error: " << WSAGetLastError() << endl;
		closeProgram();
	}
	
	return iResult;
}

int FTPClient::sendAndReceive(char *sendbuf, char *recvbuf) {
	WaitForSingleObject(mutex, INFINITE);
	sendCommand(sendbuf);
	int iResult = receiveResponse(recvbuf);
	ReleaseMutex(mutex);
	
	return iResult;
}

void FTPClient::closeConnect(SOCKET socket)
{
	if (socket != INVALID_SOCKET) {
		int iResult = shutdown(socket, SD_SEND);
		if (iResult == SOCKET_ERROR) {
			clog << "-> Line " << __LINE__ << " in file " << __FILE__ << ": " << endl << "shutdown failed with error: " << WSAGetLastError() << endl;
			closeProgram();
		}
	}
}
