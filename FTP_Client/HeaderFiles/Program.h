#ifndef PROGRAM_H
#define PROGRAM_H

#include <cstdio>

class Program
{
public:
	Program(void);
	FILE *log = nullptr;
	void closeProgram(void);
};

#endif