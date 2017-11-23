#include "HistoryManager.h"
#include <iostream>
#include <iomanip>
#include <Windows.h>

using namespace std;

HistoryManager::HistoryManager(void) {
	hisTable.resize(0);
}

HistoryManager::HistoryManager(string ipServer) {
	restore(ipServer);
}

HistoryRecord HistoryManager::getRecord(int id) {
	return hisTable[id];
}

void HistoryManager::remove(int id) {
	hisTable.erase(hisTable.begin() + id);
}

void HistoryManager::removeAll(void) {
	hisTable.clear();
}

void HistoryManager::print(void) {
	int n = hisTable.size();
	cout << left << setw(5) << "ID";
	cout << "\t" << setw(17) << "Completed percent";
	cout << "\t" << "File name" << endl;
	for (int i = 0; i < n; i++) {
		cout << " " << left << setw(5) << i;
		cout << "\t" << setw(17) << ((float)(hisTable[i].transByte + hisTable[i].start) / hisTable[i].totalSize * 100);
		cout << hisTable[i].source << endl;
	}
}

void HistoryManager::add(HistoryRecord record) {
	hisTable.push_back(record);
}

bool HistoryManager::restore(string ipServer) {
	FILE *f = nullptr;

	string hisFileName = ".\\history\\" + ipServer + "\\history.dat";
	fopen_s(&f, hisFileName.c_str(), "rb");

	if (f == nullptr) {
		cout << "Khong co lich su nao duoc luu lai." << endl;
		system("pause");
		return false;
	}

	int n = 0;
	fscanf_s(f, "%d", &n);

	for (int i = 0; i < n; i++) {
		HistoryRecord record;
		fscanf_s(f, "%lld ", &record.start);
		fscanf_s(f, "%lld ", &record.transByte);

		int tmp = 0;
		fscanf_s(f, "%d ", &tmp);
		if (tmp == 1) {
			record.completed = true;
		}
		else {
			record.completed = false;
		}
		
		fscanf_s(f, "%lld ", &record.totalSize);
		
		char buffer[255];
		fscanf_s(f, "%s", buffer, _countof(buffer));
		
		record.source.assign(buffer);
		fscanf_s(f, "%s\n", buffer, _countof(buffer));
		
		record.destination.assign(buffer);
		this->add(record);
	}

	fclose(f);
	cout << "Lich su da duoc restore thanh cong" << endl;
	system("pause");
	return true;
}

bool HistoryManager::save(string ipServer) {
	FILE *f = nullptr;

	string dirPath = ".\\history\\" + ipServer;
	
	if (!CreateDirectory(dirPath.c_str(), NULL))
	{
		int returnCode = GetLastError();
		if (returnCode == ERROR_ALREADY_EXISTS) {
			
		}
		else if(returnCode == ERROR_PATH_NOT_FOUND){
			printf("CreateDirectory failed \n");
			system("pause");
			return false;
		}	
	}

	string filePath =  dirPath + "\\history.dat";
	fopen_s(&f, filePath.c_str(), "w+t");

	int n = hisTable.size();
	fprintf(f, "%d\n", n);

	long long start = 0;
	long long transByte = 0;
	bool completed = false;
	long long totalSize = 0;
	string source;
	string destination;

	for (int i = 0; i < n; i++) {
		fprintf(f, "%lld ", hisTable[i].start);
		fprintf(f, "%lld ", hisTable[i].transByte);

		if (hisTable[i].completed == true) {
			fprintf(f, "1 ");
		}
		else {
			fprintf(f, "0 ");
		}
		
		fprintf(f, "%lld ", hisTable[i].totalSize);
		fprintf(f, "%s ", hisTable[i].source.c_str());
		fprintf(f, "%s\n", hisTable[i].destination.c_str());
	}

	fclose(f);
	cout << "Lich su da duoc luu thanh cong" << endl;
	system("pause");

	return true;
}