#include "ClientSocket.h"
#include <iostream>

using namespace std;

ClientSocket::ClientSocket(void): clientSocket(INVALID_SOCKET)
{
	DEBUG_CONSTRUCTOR
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (iResult != 0) {
		clog << "-> Line " << __LINE__ << " in file " << __FILE__ << ": " << endl << "WSAStartup failed with error: " << iResult << endl;
		//closeProgram();
	}
}

ClientSocket::~ClientSocket(void)
{
	closeConnect();
	WSACleanup();
}

bool ClientSocket::connectServer(char* serverIP, char* port, int addressFamily, int socketType, int protocol) {
	clog << "Connecting to  " << serverIP << ": "<< port << endl;
	struct addrinfo *result = nullptr, *ptr = nullptr, hints;
	
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = addressFamily;
	hints.ai_socktype = socketType;
	hints.ai_protocol = protocol;

	// Resolve the server address and port
	int iResult = getaddrinfo(serverIP, port, &hints, &result);

	if (iResult != 0) {
		clog << "-> Line " << __LINE__ << " in file " << __FILE__ << ": " << endl << "getaddrinfo failed with error: " << iResult << endl;
		//closeProgram();
	}

	int counter = 0;
	do {
		for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
			// Create a SOCKET for connecting to server

			clientSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

			if (clientSocket == INVALID_SOCKET) {
				clog << "-> Line " << __LINE__ << " in file " << __FILE__ << ": " << endl << "socket failed with error: " << WSAGetLastError();
				//closeProgram();
			}

			// connect to server.
			iResult = connect(clientSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
			
			if (iResult == SOCKET_ERROR) {
				closesocket(clientSocket);
				clientSocket = INVALID_SOCKET;
				continue;
			}
			counter = 39;
			break;
		}
		
		counter++;
		if (counter == 40) {
			break;
		}
	} while (1);
	
	freeaddrinfo(result);

	if (clientSocket == INVALID_SOCKET) {
		clog << "-> Line " << __LINE__ << " in file " << __FILE__ << ": " << endl << "Unable to connect to server!\n";
		return false;
	}

	return true;
}

int ClientSocket::sendMess(char* sendbuf)
{
	int result = send(clientSocket, sendbuf, (int)strlen(sendbuf) + 1, 0);

	if (result == SOCKET_ERROR) {
		clog << "-> Line " << __LINE__ << " in file " << __FILE__ << ": " << endl << "send failed with error: " << WSAGetLastError() << endl;
	}

	return result;
}

int ClientSocket::sendData(char *sendbuf) {
	int iResult;

	iResult = send(clientSocket, sendbuf, 1, 0);

	if (iResult < 0)
	{
		clog << "-> Line " << __LINE__ << " in file " << __FILE__ << ": " << endl << "send failed with error: " << WSAGetLastError() << endl;
		//closeProgram();
	}

	return iResult;
}

int ClientSocket::receive(char *recvbuf, int lenRecvbuf)
{
	int iResult;

	iResult = recv(clientSocket, recvbuf, lenRecvbuf, 0);
	recvbuf[iResult] = '\0'; // Cẩn thận dòng này.

	if (iResult < 0)
	{
		clog << "-> Line " << __LINE__ << " in file " << __FILE__ << ": " << endl << "recv failed with error: " << WSAGetLastError() << endl;
		//closeProgram();
	}

	return iResult;
}


void ClientSocket::closeConnect()
{
	if (clientSocket != INVALID_SOCKET) {
		int iResult = shutdown(clientSocket, SD_SEND);
		if (iResult == SOCKET_ERROR) {
			clog << "-> Line " << __LINE__ << " in file " << __FILE__ << ": " << endl << "shutdown failed with error: " << WSAGetLastError() << endl;
			//closeProgram();
		}
	}
}