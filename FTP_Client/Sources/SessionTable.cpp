#include "SessionTable.h"
#include <cstdio>
#include <psapi.h>
#include <iostream>

bool SessionTable::checkTerminatedProcess(int id)
{
	LPDWORD lpExitCode = NULL;
	GetExitCodeProcess(prtable[id].hProcess, lpExitCode);

	if (*lpExitCode == STILL_ACTIVE) {
		return false;
	}

	return true;
}


bool SessionTable::createSession(char* arguments, char* serverIP)
{
	time_t rawtime;
	struct tm * timeinfo = nullptr;

	time (&rawtime);
	localtime_s(timeinfo, &rawtime);

	SessionInfo sessInf;
	char startTime[100];
	asctime_s(startTime, 100, timeinfo);
	sessInf.startTime.assign(startTime);
	
	sessInf.serverIP.assign(serverIP);

	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);

	if (CreateProcess(NULL, arguments, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {
		printf("Create process failed");
		return false;
	}

	prtable.push_back(pi);
	inf.push_back(sessInf);
	nSession++;
	return true;
}

void SessionTable::listSession(void)
{
	if (nSession == 0) {
		printf("Don't have any session.\n");
		return;
	}

	for (int i = 0; i < nSession; i++) {
		if (!checkTerminatedProcess(i)) {
			cout << i << ": ";
			cout << "IP: " << inf[i].serverIP;
			cout << ". Start: " << inf[i].startTime << endl;
		} else {
			closeSession(i);
		}
	}
}

void SessionTable::closeSession(int id)
{
	if (id >= 0 && id < nSession) {
		TerminateProcess(prtable[id].hProcess, 0);
		CloseHandle(prtable[id].hThread);
		CloseHandle(prtable[id].hProcess);
		prtable.erase(prtable.begin() + id);
		inf.erase(inf.begin() + id);
	} else {
		printf("Invalid session ID.\n");
	}
}

void SessionTable::closeAllSession(void) {
	for (auto pi : prtable) {
		TerminateProcess(pi.hProcess, 0);
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
	}

	prtable.clear();
	inf.clear();
}

int SessionTable::getNumberSession(void) {
	return nSession;
}