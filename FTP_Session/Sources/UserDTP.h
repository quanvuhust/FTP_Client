#ifndef TRANSFER_SESSION_H
#define TRANSFER_SESSION_H

#include "UserPI.h"
#include <string>

using namespace std;

struct ThreadInfo
{
	int id;
	long totalSize = 0;
	long byteTransfer = 0;
	string source;
	string destination;
};

struct DownLoadLog{
	char* fileName;
	int startOffset;
	int size;
	int completed;
};

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
	bool download(char *path, ThreadInfo *inf);
	bool download(char *path, int startOffset, int size, ThreadInfo *inf);
	bool upload(char *path, ThreadInfo *inf);
};

#endif