#include "ThreadTable.h"
#include <iostream>
#include <cstdio>
#include <iomanip>

using namespace std;

ThreadTable::ThreadTable()
{

}

bool ThreadTable::addThreadInfo(ThreadInfo *inf)
{
	iTable.push_back(inf);
	return true;
}

bool ThreadTable::addThread(HANDLE h)
{
	hTable.push_back(h);
	return true;
}

bool ThreadTable::remove(int id)
{
	hTable.erase(hTable.begin() + id);
	iTable.erase(iTable.begin() + id);
	return true;
}

bool ThreadTable::removeAll(void)
{
	hTable.clear();
	iTable.clear();
	return true;
}

bool ThreadTable::checkTerminatedThread(int id)
{
	DWORD exitCode = 0;
	GetExitCodeThread(hTable[id], &exitCode);

	if (exitCode == STILL_ACTIVE) {
		return false;
	}

	return true;
}

void ThreadTable::print(vector<bool *> &checkStopCondition)
{
	int n = hTable.size();
	int c = n;

	for (int i = 0; i < n; i++) {
		if (*checkStopCondition[i] == true) {
			c--;
		}
	}

	if (c == 0) {
		cout << "Khong co luong nao dang download." << endl;
	}

	for (int i = 0; i < n; i++) {
		if (*checkStopCondition[i] == false) {
			cout << setw(7) << left << i;
			cout << iTable[i]->destination << endl;
		}
	}
}

int ThreadTable::getSize(void)
{
	return hTable.size();
}

HANDLE ThreadTable::getHandle(int id)
{
	return hTable[id];
}

ThreadInfo* ThreadTable::getThreadInfo(int id)
{
	return iTable[id];
}

bool ThreadTable::pause(int id)
{
	if (id < 0 || id >= this->getSize()) {
		printf("Invalid ID.\n");
		return false;
	}

	SuspendThread(hTable[id]);

	return true;
}

bool ThreadTable::resume(int id)
{
	if (id < 0 || id >= this->getSize()) {
		printf("Invalid ID.\n");
		return false;
	}

	ResumeThread(hTable[id]);

	return true;
}
