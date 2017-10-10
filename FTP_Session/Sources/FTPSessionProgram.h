#ifndef FTPSESSIONPROGRAM_H
#define FTPSESSIONPROGRAM_H
#define WIN32_LEAN_AND_MEAN

#include <vector>
#include <Windows.h>
#include "FTPSession.h"

using namespace std;

#define SPACE 32

class ThreadTable
{
private:
	vector<HANDLE> hTable;
	vector<ThreadInfo> iTable;
public:
	ThreadTable();
	bool addThreadInfo(ThreadInfo inf);
	bool addThread(HANDLE h);
	int getSize(void);
	bool pause(int id);
	bool resume(int id);
	ThreadInfo& getThreadInfo(int id);
};

#endif