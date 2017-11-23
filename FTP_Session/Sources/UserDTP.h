#ifndef TRANSFER_SESSION_H
#define TRANSFER_SESSION_H

#include "UserPI.h"
#include "HistoryManager.h"
#include <string>

using namespace std;

class UserDTP
{
private:
	ClientSocket dataSocket;
	char dataPort[NUMBER_DIGIT_PORT] = {};
	char* serverIP;
public:
	static const int BLOCK_SIZE = 4 * 1024;
	UserDTP(char* serverIP, char dataPort[]);
	~UserDTP(void);
	
	bool connectServer(void);
	bool download(string &buffer);
	bool download(char *path, ThreadInfo *inf, HistoryManager *gHisManager);
	bool upload(char *path, ThreadInfo *inf);
};

#endif