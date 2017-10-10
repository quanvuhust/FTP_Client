#include "UserDTP.h"
#include <iostream>
#include <cstring>
#include <cstdlib>

using namespace std;

UserDTP::UserDTP(char* serverIP, char dataPort[]): serverIP(serverIP)
{
	strcpy_s(this->dataPort, NUMBER_DIGIT_PORT, dataPort);
}

UserDTP::~UserDTP(void)
{

}

bool UserDTP::connectServer(void)
{
	if (!dataSocket.connectServer(serverIP, dataPort)) {
		return false;
	}

	return true;
}

bool UserDTP::download(string &buffer)
{
	int iResult = 1;
	char *recvbuf = new char[BLOCK_SIZE + 1];
	while (iResult > 0) {
		iResult = dataSocket.receive(recvbuf, BLOCK_SIZE);
		buffer.append(recvbuf);
	}

	return true;
}

bool UserDTP::download(char *path, ThreadInfo *inf)
{
	inf->byteTransfer = 0;
	HANDLE file = CreateFile(path, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, 0, NULL);
	int iResult = 1;
	long sum = 0;
	char *recvbuf = new char[BLOCK_SIZE + 1];

	while (1) {
		iResult = dataSocket.receive(recvbuf, BLOCK_SIZE);
		if (iResult <= 0) {
			break;
		}
		sum += iResult;
		inf->byteTransfer = sum;
		WriteFile(file, recvbuf, iResult, NULL, NULL);
	}

	delete[] recvbuf;
	CloseHandle(file);
	return true;
}


bool UserDTP::download(char *path, int startOffset, int size, ThreadInfo *inf)
{
	inf->byteTransfer = 0;
	HANDLE file = CreateFile(path, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, 0, NULL);
	SetFilePointer(file, startOffset, NULL, FILE_BEGIN);

	int iResult = 1;
	long sum = 0;
	char *recvbuf = new char[BLOCK_SIZE + 1];

	int nBlock = size / BLOCK_SIZE;

	for (int i = 0; i < nBlock - 1; i++) {
		iResult = dataSocket.receive(recvbuf, BLOCK_SIZE);
		sum += iResult;
		inf->byteTransfer = sum;
		WriteFile(file, recvbuf, iResult, NULL, NULL);
	}
	int sizeEndBlock = size - nBlock * BLOCK_SIZE;
	if (sizeEndBlock != 0) {
		dataSocket.receive(recvbuf, size - nBlock * BLOCK_SIZE);
	}

	WriteFile(file, recvbuf, iResult, NULL, NULL);

	//inf->session->closeConnect(inf->dataSocket);

	delete[] recvbuf;
	CloseHandle(file);
	return true;
}

bool UserDTP::upload(char *path, ThreadInfo *inf)
{
	return true;
}


/*
void UserPI::download(char *destination, char *source, const int numberThread)
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
	SOCKET *dataSocket = new SOCKET[numberThread];
	char **dataPort = new char*[numberThread];

	for(i = 0; i < numberThread; i++) {
		dataSocket[i] = INVALID_SOCKET;
		dataPort[i] = new char[NUMBER_DIGIT_PORT];


		establishDataChanel(dataSocket[i], dataPort[i]);
	}


	for (i = 0; i < numberThread - 1; i++) {
		inf[i] = new DownloadInformation(destination, startPosition, numberBlocksPerThread, INVALID_SOCKET, this);
		downloadSegment(h, inf, i, source, dataSocket[i]);
		startPosition += offset;
	}

	numberBlocksPerThread = numberBlocks - numberBlocksPerThread*(numberThread - 1);
	inf[i] = new DownloadInformation(destination, startPosition, numberBlocksPerThread, INVALID_SOCKET, this);

	downloadSegment(h, inf, i, source, dataSocket[i]);

	WaitForMultipleObjects(numberThread, h, TRUE, INFINITE);

	fopen_s(&fileDownLoad, destination, "a+b");
	fseek(fileDownLoad, getFileSize(source), SEEK_SET);
	int buf = EOF;
	fwrite(&buf, 1, 1, fileDownLoad);
	fclose(fileDownLoad);

	for (int i = 0; i < numberThread; i++) {
		delete inf[i];
	}
	delete[] inf;

	for (int i = 0; i < numberThread; i++) {
		CloseHandle(h[i]);
		delete[] dataPort[i];
	}
	delete[] h;
	delete[] dataSocket;
	delete[] dataPort;

	cout << "Download completed" << endl;
	getchar();
}
*/
