#undef UNICODE

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "UserPI.h"

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

using namespace std;

UserPI::UserPI(void) {

}


UserPI::UserPI(char* serverIP): serverIP(serverIP) {}


UserPI::~UserPI(void)
{
	DEBUG_DESTRUCTOR
	controlSocket.closeConnect();
	//WSACleanup();
}

bool UserPI::createChanel(void) {
	if(controlSocket.connectServer(serverIP, controlPort)){
		receiveResponse();
		return true;
	}
	
	return false;
}

void UserPI::sendCommand(char* command)
{
	controlSocket.sendMess(command);
}

int UserPI::receiveResponse(char* recvbuf)
{
	int iResult = controlSocket.receive(recvbuf, DEFAULT_BUFLEN);
	clog << "-> Line " << __LINE__ << " in file " << __FILE__ << ": " << endl << recvbuf << endl;
	return iResult;
}

void UserPI::receiveResponse(void)
{
	char recvbuf[DEFAULT_BUFLEN];
	controlSocket.receive(recvbuf, DEFAULT_BUFLEN);
	clog << "-> Line " << __LINE__ << " in file " << __FILE__ << ": " << endl << "Server response: "<< recvbuf << endl;
}

void UserPI::closeProgram(void) 
{
	cout << "The program have some errors. Please read log for more details!" << endl;
	system("pause");
	this->~UserPI();
	exit(1);
}
