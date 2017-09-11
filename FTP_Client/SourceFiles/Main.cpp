#include "..\HeaderFiles\FTPClient.h"

int main(int argc, char** argv) {
	FTPClient *client = new FTPClient();
	client->execute(argc, argv);
	delete client;
	return 0;
}