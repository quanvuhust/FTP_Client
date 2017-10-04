#ifndef FTP_CLIENT_H
#define FTP_CLIENT_H

#include "UserPI.h"
#include "UserDTP.h"

class FTPSession
{
private:
	UserPI *userPI;
	char* username;
	char* password;
	char* serverIP;
	enum TYPE_TRANSFER { ASCII, BINARY };
public:
	FTPSession(char *serverIP,char *username, char* password);
	~FTPSession(void);
	void getIPServer(char* serverIP);
	void login(UserPI *pUserPI, char* username, char* password);
	void setTransferMode(UserPI *pUserPI, const int type);
	void list(void);
	void changeDirectory(char *newPath);
	void printCurrentDirectory(void);
	long getFileSize(char *fileName);

	bool creatControlChanel(UserPI *pUserPI);
	void establishDataChanel(UserPI *pUserPI, char dataPort[]);
	bool download(char *destination, char *source, ThreadInf *inf);
	bool download(char *destination, char *source, int startOffset, int size, ThreadInf *inf);
	bool restart(const ThreadInf inf);
	bool upload(char *destination, char *source, ThreadInf *inf);
};

#endif