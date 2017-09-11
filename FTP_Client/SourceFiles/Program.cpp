#include "../HeaderFiles/Program.h"

Program::Program(void) {
	freopen_s(&log, "FTPClientLog.txt", "w+t", stderr);
}

void Program::closeProgram(void) {
	fclose(log);
}