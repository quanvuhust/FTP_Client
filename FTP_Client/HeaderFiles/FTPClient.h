#ifndef FTPClient_H
#define FTPClient_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "Program.h"

//#define NDEBUG

#ifndef NDEBUG
#define DEBUG_DESTRUCTOR clog << "-> Line " << __LINE__ << " in file " << __FILE__ << ": " << endl << "Destructor." << endl;
#define DEBUG_CONSTRUCTOR clog << "-> Line " << __LINE__ << " in file " << __FILE__ << ": " << endl << "Constructor." << endl;
#else
#define DEBUG_DESTRUCTOR
#define DEBUG_CONSTRUCTOR
#endif

#define DEFAULT_BUFLEN 0x10240
#define NUMBER_DIGIT_PORT 10

class DownloadInformation {
public:
	char *destination;
	int start;
	int numberBlocks;
	SOCKET dataSocket;
	class FTPClient *client;

	DownloadInformation(char *destination, int start, int numberBlocks, SOCKET dataSocket, class FTPClient *client) : destination(destination),
		start(start), numberBlocks(numberBlocks), dataSocket(dataSocket), client(client) {}
};


class FTPClient: public Program
{
private:
	SOCKET controlSocket;
	char controlPort[NUMBER_DIGIT_PORT] = "21";
	char* ipADDRESS;
	enum TYPE_TRANSFER{ASCII, BINARY};
	HANDLE mutex;
public:
	static const int BLOCK_SIZE = 32;
	FTPClient(void);
	~FTPClient(void);
	int execute(int argc, char **argv);
	void closeProgram(void);
	void getIPServer(char* ipADDRESS);
	void connectServer(char *port, char *ipServer, SOCKET &connectSocket);
	void login(char* username, char* password);
	void establishDataChanel(SOCKET &dataSocket, char dataPort[]);
	void setTransferMode(const int type);
	void list(void);
	void changeDirectory(char *newPath);
	void printCurrentDirectory(void);
	long getFileSize(char *fileName);
	void download(char *destination, char *source, const int numberThread);
	void upload(char *destination, char *source);

	void sendCommand(char* sendbuf);
	int receiveResponse(char* recvbuf);
	int sendAndReceive(char *sendbuf, char *recvbuf);
	void downloadSegment(HANDLE *h, DownloadInformation **inf, int index, char *source);
	friend void downloadThread(const DownloadInformation * const inf);
	
	void closeConnect(SOCKET socket);
};

#endif