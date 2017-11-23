#include "UserDTP.h"
#include <iostream>
#include <cstring>
#include <cstdlib>

using namespace std;

extern HANDLE gMutex;

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

bool UserDTP::download(char *path, ThreadInfo *inf, HistoryManager *gHisManager)
{
	long long start = inf->start;
	long long offset = inf->offset;
	HANDLE file = CreateFile(path, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, 0, NULL);
	SetFilePointer(file, start, NULL, FILE_BEGIN);

	long long iResult = 1;
	long long sum = 0;
	char *recvbuf = new char[BLOCK_SIZE + 1];

	int nBlock = offset / BLOCK_SIZE;

	for (int i = 0; i < nBlock; i++) {
		if (*(inf->itCheckStop) == true) {
			break;
		}

		iResult = dataSocket.receive(recvbuf, BLOCK_SIZE);
		sum += iResult;
		WriteFile(file, recvbuf, iResult, NULL, NULL);
	}

	if (*(inf->itCheckStop) == false) {
		while(iResult > 0) {
			iResult = dataSocket.receive(recvbuf, BLOCK_SIZE);
			sum += iResult;
			WriteFile(file, recvbuf, iResult, NULL, NULL);
		}
	}
	
	HistoryRecord record;
	record.start = start;
	record.transByte = sum;
	record.totalSize = inf->totalSize;
	record.source = inf->source;
	record.destination = inf->destination;
	record.completed = false;

	if (sum + start == inf->totalSize) {
		record.completed = true;
	}

	WaitForSingleObject(gMutex, INFINITE);
	gHisManager->add(record);
	ReleaseMutex(gMutex);

	delete[] recvbuf;
	CloseHandle(file);

	if (*(inf->itCheckStop) == true) {
		return false;
	}
	*(inf->itCheckStop) = true;
	return true;
}

bool UserDTP::upload(char *path, ThreadInfo *inf)
{
	return true;
}
