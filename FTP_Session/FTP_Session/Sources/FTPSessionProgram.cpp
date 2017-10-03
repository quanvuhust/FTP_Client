#include "FTPClient.h"
#include <iostream>
#include <cstdio>

using namespace std;

#define SESSION_ID argv[2]
#define SERVER_IP argv[3]
#define USERNAME argv[4]
#define PASSWORD argv[5]

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
	int i, len = strlen(str), position;

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
	int countSpace = 0, len = strlen(input);
	char *p, *str = input;

	input[len] = SPACE;
	input[len + 1] = '\0';

	while ((p = strchr(str, SPACE)) != NULL) {
		countSpace++;
		str = p + 1;
	}

	*pArgc = countSpace;
	*pArgv = (char **) malloc(*pArgc * sizeof(char*));

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
	int len = strlen(str), i;

	for (i = 0; i < len; i++)
	{
		if (isupper(str[i]))
		{
			str[i] = tolower(str[i]);
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
	strcpy_s(fileName, 18, "FTPSession");
	strcat_s(fileName, 18, SESSION_ID);
	strcat_s(fileName, 18, "Log.txt");
	freopen_s(&log, fileName, "w+t", stderr);
	char input[MAX_PATH + 2];

	FTPSession *session = new FTPSession(SERVER_IP, USERNAME, PASSWORD);

	while (1)
	{
		session->printCurrentDirector();
		cout << ">";
		cin.sync();
		cin.getline(input, MAX_PATH + 2, '\n');
		if (input[0] == "\0") {
			continue;
		}
		normalizeStr(input);
		parseCommand(&argc, &argv, input);
		tolowerStr(argv[0]);
	}

	switch (command) {
	case 1: {
		break;
	}
	case 2: {
		break;
	}
	case 3: {
		break;
	}
	case 4: {
		break;
	}
	}
	delete session;
	fclose(log);
	return 0;
}
