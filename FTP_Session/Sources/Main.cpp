#include "ThreadTable.h"
#include "FTPSession.h"
#include "HistoryManager.h"
#include <iostream>
#include <cstdio>
#include <vector>

using namespace std;

#define SESSION_ID argv[1]
#define SERVER_IP argv[2]
#define USERNAME argv[3]
#define PASSWORD argv[4]

FTPSession *gSession;
HistoryManager *gHisManager;
HANDLE gMutex;

void help(int argc)
{
	if (argc > 1) {
		printf("Too many arguments to command help.\n");
		return;
	}

	puts("\t\tWELCOME TO MY CLIENT\n");
	puts("\tMy Client supports the following commands:");

	printf("\t%-8s : List the content of the current directory.\n", "ls");

	printf("\t%-8s : Close the session.\n", "exit");
}

void normalizeStr(char *str)
{
	int i, position, len = (int)strlen(str);

	/*Xoa khoang trong cuoi xau*/
	for (i = len - 1; isspace(str[i]); i--);

	str[++i] = '\0';
	len = i;

	/*Xoa khoang trong dau xau*/
	for (i = 0; isspace(str[i]); i++);

	len -= i;
	memcpy(str, str + i, len);

	/*Xoa khoang trong giua xau*/
	i = len - 2;
	while (i > 0)
	{
		if (isspace(str[i]))
		{
			position = i;
			str[i] = SPACE;

			while (isspace(str[--i]));

			memcpy(str + i + 1, str + position, len - position);
			len -= (position - i - 1);
		}
		i--;
	}

	str[len] = '\0';
}

void parseCommand(int *pArgc, char ***pArgv, char *input)
{
	int countSpace = 0;
	size_t len = strlen(input);
	char *p, *str = input;

	input[len] = SPACE;
	input[len + 1] = '\0';

	while ((p = strchr(str, SPACE)) != NULL) {
		countSpace++;
		str = p + 1;
	}

	*pArgc = countSpace;
	*pArgv = (char **)malloc(*pArgc * sizeof(char*));

	/*Lay tham so vao argv[0]*/
	p = strchr(input, SPACE);
	(*pArgv)[0] = (char *)malloc((p - input + 1) * sizeof(char));
	*p = '\0';
	memcpy((*pArgv)[0], input, p - input + 1);
	input = p + 1;

	/*Lay tham so vao argv[1] -- argv[argc - 2]*/
	int i = 1;
	while ((p = strchr(input, SPACE)) != NULL) {
		(*pArgv)[i] = (char *)malloc((p - input + 1) * sizeof(char));
		*p = '\0';
		memcpy((*pArgv)[i++], input, p - input + 1);
		input = p + 1;
	}
}

void tolowerStr(char *str)
{
	size_t len = strlen(str);
	size_t i;

	for (i = 0; i < len; i++)
	{
		if (isupper(str[i]))
		{
			str[i] = tolower(str[i]);
		}
	}
}

void threadDownload(ThreadInfo *inf)
{
	size_t lenDes = inf->destination.length(), lenSour = inf->source.length();

	char *destination = new char[lenDes + 1];
	strcpy_s(destination, lenDes + 1, inf->destination.c_str());
	char *source = new char[lenSour + 1];
	strcpy_s(source, lenSour + 1, inf->source.c_str());
	
	gSession->download(destination, source, inf);
	
	delete[] destination;
	delete[] source;
}

void cd(int argc, char* path) {
	if (argc == 2) {
		gSession->changeDirectory(path);
	}
	else {
		cout << "Invalid arguments." << endl;
	}
}

void ls(int argc) {
	if (argc == 1) {
		gSession->list();
	}
	else {
		cout << "Invalid arguments." << endl;
	}
}

void size(int argc, char* fileName) {
	if (argc == 2) {
		gSession->getFileSize(fileName);
	}
	else {
		cout << "Invalid arguments." << endl;
	}
}

void download(int argc, ThreadTable *thrTable, char* source, char* destination, bool *pCheckStop) 
{
	if (argc == 3) {
		DWORD threadId;
		int id = thrTable->getSize();

		ThreadInfo *inf = new ThreadInfo();
		inf->start = 0;
		inf->pCheckStop = pCheckStop;
		inf->id = id;
		inf->totalSize = gSession->getFileSize(source);
		inf->offset = inf->totalSize;
		inf->source.assign(source);
		inf->destination.assign(destination);

		thrTable->addThreadInfo(inf);
		thrTable->addThread(CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)threadDownload, thrTable->getThreadInfo(id), 0, &threadId));
	}
	else {
		cout << "Invalid arguments." << endl;
	}
}

void pause(int argc, ThreadTable *thrTable, int id) 
{
	if (argc == 2) {
		thrTable->pause(id);
	} 
	else {
		cout << "Invalid arguments." << endl;
	}
}

void resume(int argc, ThreadTable *thrTable, int id) {
	if (argc == 2) {
		thrTable->resume(id);
	}
	else {
		cout << "Invalid arguments." << endl;
	}
}

void stop(int argc, bool *pCheckStop) 
{
	if (argc == 2) {
		*pCheckStop = true;
	}
	else {
		cout << "Invalid arguments." << endl;
	}
}

void listDownload(int argc, ThreadTable *thrTable, vector<bool *> &checkStopCondition)
{
	if (argc == 1) {
		thrTable->print(checkStopCondition);
	}
	else {
		cout << "Invalid arguments." << endl;
	}
}

bool restart(int id, ThreadTable *thrTable, bool *pCheckStop) {
	DWORD threadId;

	WaitForSingleObject(gMutex, INFINITE);
	HistoryRecord record = gHisManager->getRecord(id);
	ReleaseMutex(gMutex);
	
	ThreadInfo *inf = new ThreadInfo();
	inf->start = record.start + record.transByte;
	inf->offset = record.totalSize - inf->start;
	inf->pCheckStop = pCheckStop;
	inf->id = id;
	inf->totalSize = gSession->getFileSize(record.source.c_str());
	inf->source = record.source;
	inf->destination = record.destination;

	WaitForSingleObject(gMutex, INFINITE);
	gHisManager->remove(id);
	ReleaseMutex(gMutex);

	thrTable->addThreadInfo(inf);
	thrTable->addThread(CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)threadDownload, thrTable->getThreadInfo(id), 0, &threadId));

	return true;
}

void history(int argc, char* argv[], ThreadTable *thrTable, vector<bool *> &checkStopCondition) {
	if (argc < 2) {
		cout << "Invalid arguments." << endl;
	}

	if (argc == 2) {
		if(strcmp(argv[1], "show") == 0) {
			WaitForSingleObject(gMutex, INFINITE);
			gHisManager->print();
			ReleaseMutex(gMutex);
		}	
	}
	else if (argc == 3) {
		if (strcmp(argv[1], "restart") == 0) {
			int id = atoi(argv[2]);
			checkStopCondition.push_back(new bool(false));
			restart(id, thrTable, checkStopCondition[checkStopCondition.size() - 1]);
		}
		else if (strcmp(argv[1], "rm") == 0) {
			if (strcmp(argv[2], "all") == 0) {
				WaitForSingleObject(gMutex, INFINITE);
				gHisManager->removeAll();
				ReleaseMutex(gMutex);
			}
			else {
				int id = atoi(argv[2]);
				WaitForSingleObject(gMutex, INFINITE);
				gHisManager->remove(id);
				ReleaseMutex(gMutex);
			}
		}
	}
}

int main(int argc, char **argv)
{
	SetConsoleTitle(SERVER_IP);
	system("chcp 65001");
	system("cls");
	FILE *log = nullptr;
	char fileName[19];

	strcpy_s(fileName, 19, "FTPSession");
	strcat_s(fileName, 19, SESSION_ID);
	strcat_s(fileName, 19, "Log.txt");
	freopen_s(&log, fileName, "w+t", stderr);
	char input[MAX_PATH + 2];

	if (argc > 4) {
		gSession = new FTPSession(SERVER_IP, USERNAME, PASSWORD);
	}
	else {
		gSession = new FTPSession(SERVER_IP, USERNAME, "");
	}
	string ipServer;
	ipServer.assign(SERVER_IP);
	gHisManager = new HistoryManager(ipServer);

	ThreadTable *thrTable = new ThreadTable();
	vector<bool *> checkStopCondition;
	
	system("cls");
	while (1)
	{
		gSession->printCurrentDirectory();
		cout << ">";
		
		cin.sync();
		cin.getline(input, MAX_PATH + 2, '\n');
		if (input[0] == '\0') {
			continue;
		}

		normalizeStr(input);
		parseCommand(&argc, &argv, input);
		tolowerStr(argv[0]);

		if (strcmp(argv[0], "cd") == 0) {
			cd(argc, argv[1]);
		}
		else if (strcmp(argv[0], "ls") == 0) {
			ls(argc);
		}
		else if (strcmp(argv[0], "size") == 0) {
			size(argc, argv[1]);
		}
		else if (strcmp(argv[0], "ps") == 0) {
			listDownload(argc, thrTable, checkStopCondition);
		}
		else if (strcmp(argv[0], "download") == 0) {
			checkStopCondition.push_back(new bool(false));
			download(argc, thrTable, argv[1], argv[2], checkStopCondition[checkStopCondition.size() - 1]);
		}
		else if (strcmp(argv[0], "pause") == 0) {
			pause(argc, thrTable, atoi(argv[1]));
		}
		else if (strcmp(argv[0], "resume") == 0) {
			pause(argc, thrTable, atoi(argv[1]));
		}
		else if (strcmp(argv[0], "stop") == 0) {
			int id = atoi(argv[1]);
			stop(argc, checkStopCondition[id]);
			WaitForSingleObject(thrTable->getHandle(id), INFINITE);
			thrTable->remove(id);
			checkStopCondition.erase(checkStopCondition.begin() + id);
		}
		else if (strcmp(argv[0], "history") == 0) {
			history(argc, argv, thrTable, checkStopCondition);
		}
		else if (strcmp(argv[0], "exit") == 0) {
			if (argc == 1) {
				break;
			}
			else {
				cout << "Invalid arguments." << endl;
			}
		}
		else {
			cout << "\'" << argv[0] << "\'" << "is not recognized as an internal or external command, operable program or batch file." << endl;
		}
	}

	int nThread = thrTable->getSize();
	HANDLE *thrArray = new HANDLE[nThread];

	for (int i = 0; i < nThread; i++) {
		thrArray[i] = thrTable->getHandle(i);
	}
	
	WaitForMultipleObjects(thrTable->getSize(), thrArray, TRUE, INFINITE);
	gHisManager->save(ipServer);
	

	for (int i = 0; i < argc; i++) {
		delete[] argv[i];
	}

	delete thrTable;
	delete[] argv;

	if (thrArray != nullptr) {
		delete[] thrArray;
	}
	
	delete gSession;
	delete gHisManager;
	fclose(log);
	return 0;
}