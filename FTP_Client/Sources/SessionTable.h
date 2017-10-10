#ifndef SESSIONTABLE_H
#define SESSIONTABLE_H

#include <windows.h>
#include <vector>
#include <ctime>
#include <string>

using namespace std;

struct SessionInfo
{
	string serverIP;
	string startTime;
};

class SessionTable
{
private:
	vector<PROCESS_INFORMATION> prtable;
	vector<SessionInfo> inf;
	int nSession = 0;
public:
	bool checkTerminatedProcess(int id);
	bool isEmpty(void);
	bool createSession(char* arguments, char* serverIP);
	void listSession(void);
	void closeSession(int id);
	void closeAllSession(void);
	int getNumberSession(void);
};

#endif // SESSIONTABLE_H
