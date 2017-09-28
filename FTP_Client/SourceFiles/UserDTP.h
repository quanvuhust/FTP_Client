#ifndef TRANSFER_SESSION_H
#define TRANSFER_SESSION_H

#include "UserPI.h"
#include <string>

using namespace std;

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
	bool download(char *path);
	bool download(char *path, int startOffset, int size);
	bool upload(char *path);
};

#endif