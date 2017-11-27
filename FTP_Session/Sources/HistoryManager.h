#ifndef HISTORYMANAGER
#define HISTORYMANAGER 1

#include <string>
#include <vector>

using namespace std;

struct ThreadInfo
{
	bool *pCheckStop;
	int id;
	long long start = 0;
	long long offset = 0;
	long long totalSize = 0;
	string source;
	string destination;
};

struct HistoryRecord {
	long long start = 0;
	long long transByte = 0;
	bool completed = false;
	long long totalSize = 0;
	string source;
	string destination;
};

class HistoryManager {
private:
	vector<HistoryRecord> hisTable;
public:
	HistoryManager(void);
	HistoryManager(string ipServer);
	HistoryRecord getRecord(int id);
	void remove(int id);
	void removeAll(void);
	void print(void);
	void add(HistoryRecord record);
	bool restore(string ipServer);
	bool save(string ipServer);
};


#endif
