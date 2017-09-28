#include "FTPSession.h"

#include <cstdio>

#define NAME_FTPSession "UserPI 1"

void T1(FTPSession *session)
{
	session->download("C:\\Users\\Dell\\Downloads\\TestFTP\\SandboxieInstall.exe", "SandboxieInstall.exe");
}

int main(int argc, char** argv) {
	FILE *log = nullptr;
	freopen_s(&log, "FTPSessionLog.txt", "w+t", stderr);
	SetConsoleTitle(NAME_FTPSession);
	FTPSession *session = new FTPSession("localhost", "quan", "");
	HANDLE h[2];
	DWORD threadId;

	h[0] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)T1, session, 0, &threadId);

	getchar();
	
	
	delete session;
	fclose(log);
	return 0;
}