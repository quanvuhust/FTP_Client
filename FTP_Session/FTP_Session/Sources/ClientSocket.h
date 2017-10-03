#ifndef CLIENT_SOCKER_H
#define CLIENT_SOCKET_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

//#define NDEBUG

#ifndef NDEBUG
#define DEBUG_DESTRUCTOR clog << "-> Line " << __LINE__ << " in file " << __FILE__ << ": " << endl << "Destructor." << endl;
#define DEBUG_CONSTRUCTOR clog << "-> Line " << __LINE__ << " in file " << __FILE__ << ": " << endl << "Constructor." << endl;
#else
#define DEBUG_DESTRUCTOR
#define DEBUG_CONSTRUCTOR
#endif

#define DEFAULT_BUFLEN 0x10240

class ClientSocket{
private:
	SOCKET clientSocket;
public:
	ClientSocket(void);
	~ClientSocket(void);
	bool connectServer(char* serverIP, char* port, int addressFamily = AF_INET, int socketType = SOCK_STREAM, int protocol = IPPROTO_TCP);
	void sendMess(char* sendbuf);
	int receive(char *recvbuf, int lenRecvbuf  = DEFAULT_BUFLEN);
	void closeConnect(void);
};

#endif