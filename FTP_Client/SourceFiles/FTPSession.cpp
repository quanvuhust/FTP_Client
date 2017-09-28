#include "FTPSession.h"
#include "UserDTP.h"
#include <string>
#include <iostream>

using namespace std;

FTPSession::FTPSession(char *serverIP,char *username, char* password): serverIP(serverIP), username(username), password(password)
{
	userPI = new UserPI(serverIP);
	creatControlChanel(userPI);
}

FTPSession::~FTPSession(void)
{
	delete userPI;
}


void FTPSession::getIPServer(char* serverIP)
{
	this->serverIP = serverIP;
}

void FTPSession::login(UserPI *pUserPI, char* username, char* password) {
	int lenUsername = 6 + (int)strlen(username);
	int lenPassword = 6 + (int)strlen(password);
	char *userCommand = new char[lenUsername];
	char *passCommand = new char[lenPassword];
	char *recvbuf = new char[DEFAULT_BUFLEN];

	//Tạo câu lệnh USER và PASS
	strcpy_s(userCommand, lenUsername, "USER ");
	strcat_s(userCommand, lenUsername, username);
	strcpy_s(passCommand, lenPassword, "PASS ");
	strcat_s(passCommand, lenPassword, password);
	if (userCommand[9] == '\0') {
		clog << "-> Line " << __LINE__ << " in file " << __FILE__ << ": " << endl << "YES" << endl;
	}

	//Gửi username và password

	pUserPI->sendCommand(userCommand);
	pUserPI->receiveResponse();//Thành công trả về mã 331
	pUserPI->sendCommand(passCommand);
	pUserPI->receiveResponse();//Thành công trả về mã 230

	delete[] userCommand;
	delete[] passCommand;
	delete[] recvbuf;
}

void FTPSession::list(void) {
	char dataPort[NUMBER_DIGIT_PORT];
	/*Thiết lập data chanel*/
	establishDataChanel(userPI, dataPort);
	UserDTP *pUserDTP = new UserDTP(serverIP, dataPort);
	pUserDTP->connectServer();
	setTransferMode(userPI, BINARY);

	userPI->sendCommand("MLSD");
	userPI->receiveResponse();

	string buffer;
	pUserDTP->download(buffer);
	cout << buffer << endl;

	delete pUserDTP;
}

void FTPSession::setTransferMode(UserPI *pUserPI, const int type) 
{
	int lenCmd = 8;
	char *command = new char[lenCmd];
	strcpy_s(command, lenCmd, "TYPE ");

	if (type == ASCII) {
		strcat_s(command, lenCmd, "A");
	}
	else if (type == BINARY) {
		strcat_s(command, lenCmd, "I");
	}

	pUserPI->sendCommand(command);
	pUserPI->receiveResponse();
}

void FTPSession::changeDirectory(char *newPath) {
	if (newPath[0] == '\0') {
		return;
	}
	char *recvbuf = new char[DEFAULT_BUFLEN];
	int lenCmd = 5 + (int)strlen(newPath);
	char *command = new char[lenCmd];
	strcpy_s(command, lenCmd, "CWD ");
	strcat_s(command, lenCmd, newPath);

	userPI->sendCommand(command);
	userPI->receiveResponse(recvbuf);

	recvbuf[3] = '\0';
	if (strcmp(recvbuf, "250") == 0) {
		printf("%s\n", &recvbuf[4]);
		getchar();
	}
	else if (strcmp(recvbuf, "550") == 0) {
		printf("%s\n", &recvbuf[4]);
		getchar();
	}
	delete[] recvbuf;
	delete[] command;
}

void FTPSession::printCurrentDirectory(void) {
	char *recvbuf = new char[DEFAULT_BUFLEN];

	userPI->sendCommand("PWD");
	userPI->receiveResponse(recvbuf);

	printf("%s\n", &recvbuf[4]);
	getchar();

	delete[] recvbuf;
}

long FTPSession::getFileSize(char *fileName) {
	char *recvbuf = new char[DEFAULT_BUFLEN];
	int lenCmd = 6 + (int)strlen(fileName);
	char *command = new char[lenCmd];

	strcpy_s(command, lenCmd, "SIZE ");
	strcat_s(command, lenCmd, fileName);

	userPI->sendCommand(command);
	userPI->receiveResponse(recvbuf);

	long result = atol(&recvbuf[4]);
	delete[] recvbuf;
	delete[] command;

	return result;
}

bool FTPSession::creatControlChanel(UserPI *pUserPI)
{
	if (!pUserPI->createChanel()) {
		return false;
	}
	login(pUserPI, username, password);
	return true;
}


void FTPSession::establishDataChanel(UserPI *pUserPI, char dataPort[])
{
	//Khởi tạo data chanel ở chế độ Passive mode
	char *recvbuf = new char[DEFAULT_BUFLEN];
	pUserPI->sendCommand("PASV");
	int lenResp = pUserPI->receiveResponse(recvbuf);//Thành công trả về mã 227

	recvbuf[3] = '\0';
	if (strcmp(recvbuf, "227") == 0) {
		int first = 0, second = 0, i = 0;
		recvbuf[lenResp - 1] = '\0';
		for (i = lenResp - 2; i > -1; i--) {
			if (recvbuf[i] == ',') {
				recvbuf[i] = '\0';
				second = atoi(&recvbuf[i + 1]);
				break;
			}
		}

		for (; i > -1; i--) {
			if (recvbuf[i] == ',') {
				first = atoi(&recvbuf[i + 1]);
				break;
			}
		}

		snprintf(dataPort, NUMBER_DIGIT_PORT, "%d", (first << 8) + second);
	}
	else {
		clog << "-> Line " << __LINE__ << " in file " << __FILE__ << ": " << endl << "Cannot creat data chanel." << endl;
	}

	delete[] recvbuf;
}

bool FTPSession::download(char *destination, char *source)
{
	char dataPort[NUMBER_DIGIT_PORT];
	UserPI *pUserPI = new UserPI(serverIP);

	if(!creatControlChanel(pUserPI)){
		return false;
	}
	
	setTransferMode(pUserPI, BINARY);
	establishDataChanel(pUserPI, dataPort);
	UserDTP *pUserDTP = new UserDTP(serverIP, dataPort);
	pUserDTP->connectServer();
	
	int lenCmd = 6 + (int)strlen(source);
	char *command = new char[lenCmd];

	strcpy_s(command, lenCmd, "RETR ");
	strcat_s(command, lenCmd, source);

	pUserPI->sendCommand(command);
	pUserPI->receiveResponse();

	delete[] command;

	pUserDTP->download(destination);

	delete pUserPI;
	delete pUserDTP;
	return true;
}


bool FTPSession::download(char *destination, char *source, int startOffset, int size)
{
	char dataPort[NUMBER_DIGIT_PORT];
	UserPI *pUserPI = new UserPI(serverIP);

	if(!creatControlChanel(pUserPI)){
		return false;
	}

	establishDataChanel(pUserPI, dataPort);
	UserDTP *pUserDTP = new UserDTP(serverIP, dataPort);
	pUserDTP->connectServer();
	
	
	int lenCmd = 6 + (int)strlen(source);
	char *command = new char[lenCmd];

	strcpy_s(command, lenCmd, "REST ");
	snprintf(&command[5], lenCmd, "%d", startOffset);

	pUserPI->sendCommand(command);
	pUserPI->receiveResponse();

	strcpy_s(command, lenCmd, "RETR ");
	strcat_s(command, lenCmd, source);

	pUserPI->sendCommand(command);
	pUserPI->receiveResponse();

	delete[] command;
	
	pUserDTP->download(destination, startOffset, size);

	delete pUserPI;
	delete pUserDTP;
	return true;
}