#ifndef FTPClient_H
#define FTPClient_H

//#define NDEBUG

#ifndef NDEBUG
#define DEBUG_DESTRUCTOR cout << "Destructor." << endl;
#define DEBUG_CONSTRUCTOR cout << "Constructor." << endl;
#else
#define DEBUG_DESTRUCTOR
#define DEBUG_CONSTRUCTOR
#endif

#define DEFAULT_BUFLEN 0x10240

class FTPClient
{
private:
	SOCKET ConnectSocket;
	struct addrinfo *result, *ptr, hints;
	WSADATA wsaData;
	int iResult;
	char* IP_ADDRESS;
	FILE *log;
public:
	FTPClient(void);
	~FTPClient(void);
	void GetIPServer(char* IP_ADDRESS);
	void Connect(void);
	void Login(char* username, char* password);

	friend void SendData(FTPClient *myFTPClient, char sendbuf[DEFAULT_BUFLEN]);
	friend void ReceiveData(FTPClient *myFTPClient);
	
	void CloseConnect(void);
};

#endif // !FTPClient_H