#include "SessionTable.h"
#include <iomanip>
#include <cstdio>
#include <psapi.h>
#include <iostream>

bool SessionTable::checkTerminatedProcess(int id)
{
	DWORD exitCode = 0;
	GetExitCodeProcess(prtable[id].hProcess, &exitCode);

	if (exitCode == STILL_ACTIVE) {
		return false;
	}

	return true;
}


bool SessionTable::createSession(char* arguments, char* serverIP)
{
	time_t rawtime;
	struct tm * timeinfo = new struct tm;

	time (&rawtime);
	localtime_s(timeinfo, &rawtime);

	SessionInfo sessInf;
	char startTime[100];
	asctime_s(startTime, 100, timeinfo);
	delete timeinfo;

	sessInf.startTime.assign(startTime);
	sessInf.serverIP.assign(serverIP);

	PROCESS_INFORMATION pi;
	STARTUPINFO si;
	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);

	if (!CreateProcess(NULL, arguments, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {
		printf("Create process failed");
		return false;
	}

	prtable.push_back(pi);
	inf.push_back(sessInf);
	nSession++;
	
	return true;
}

bool SessionTable::isEmpty(void)
{
	return nSession == 0;
}


void SessionTable::listSession(void)
{
	if (isEmpty()) {
		printf("Khong co bat ky phien nao dang hoat dong.\n");
		return;
	}

	cout << "Danh sach cac phien dang hoat dong." << endl;
	cout << left << setw(5) << "ID";
	cout << "\t" << setw(17) << "IP";
	cout << "\t" << "Start time" << endl;
	for (int i = 0; i < nSession; i++) {
		cout << " " << left << setw(5) << i;
		cout << setw(17) << inf[i].serverIP;
		cout << inf[i].startTime << endl;
	}
}

void SessionTable::closeSession(int id)
{
	if (isEmpty()) {
		return;
	}

	if (id >= 0 && id < nSession) {
		TerminateProcess(prtable[id].hProcess, 0);
		CloseHandle(prtable[id].hThread);
		CloseHandle(prtable[id].hProcess);
		prtable.erase(prtable.begin() + id);
		inf.erase(inf.begin() + id);
		nSession--;
	} else {
		printf("Khong ton tai id.\n");
	}
}

void SessionTable::closeAllSession(void) {
	if (isEmpty()) {
		return;
	}

	for (auto pi : prtable) {
		TerminateProcess(pi.hProcess, 0);
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
	}

	prtable.clear();
	inf.clear();
	nSession = 0;
}

int SessionTable::getNumberSession(void) {
	return nSession;
}