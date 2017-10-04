#ifndef FTPSESSIONPROGRAM_H
#define FTPSESSIONPROGRAM_H

struct ThreadInfo
{
	int id;
	long totalSize = 0;
	long byteTransfer = 0;
	string source;
	string destination;
};

class ThreadTable
{
private:
	vector<HANDLE> hTable;
	vector<ThreadInfo> iTable;
public:
	ThreadTable(FTPSession *session);
	bool addTheadInfo(ThreadInfo inf);
	bool addThread(HANDLE h);
	int getSize(void);
	bool pause(int id);
	bool resume(int id);
	ThreadInfo& getThreadInfo(int id);
};

ThreadTable::ThreadTable(FTPSession *session): session(session) {}

bool ThreadTable::addTheadInfo(ThreadInfo inf)
{
	iTable.push_back(inf);
}

bool ThreadTable::addThread(HANDLE h)
{
	hTable.push_back(h);
}

int ThreadTable::getSize(void)
{
	return hTable.size();
}

ThreadInfo& ThreadTable::getThreadInfo(int id)
{
	return iTable[id];
}

bool ThreadTable::pause(int id)
{

}

bool ThreadTable::resume(int id)
{
	
}

#endif