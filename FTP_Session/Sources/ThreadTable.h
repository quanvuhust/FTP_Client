#ifndef THREADTABLE_H
#define THREADTABLE_H
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
	vector<ThreadInfo *> iTable;
public:
	ThreadTable();
	bool addThreadInfo(ThreadInfo *inf);
	bool addThread(HANDLE h);
	bool remove(int id);
	bool removeAll(void);
	bool checkTerminatedThread(int id);
	void print(vector<bool *> &checkStopCondition);
	int getSize(void);
	HANDLE getHandle(int id);
	bool pause(int id);
	bool resume(int id);
	ThreadInfo* getThreadInfo(int id);
};

#endif