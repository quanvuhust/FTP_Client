#ifndef CONTROL_CHANEL_H
#define CONTROL_CHANEL_H

#include "ClientSocket.h"
#define NUMBER_DIGIT_PORT 8

class UserPI
{
private:
	ClientSocket controlSocket;
	char controlPort[NUMBER_DIGIT_PORT] = "21";
	char* serverIP;
public:
	UserPI(void);
	UserPI(char* serverIP);
	~UserPI(void);

	void closeProgram(void);
	bool createChanel(void);

	void sendCommand(char* command);
	void receiveResponse(void);
	int receiveResponse(char* recvbuf);
};

#endif