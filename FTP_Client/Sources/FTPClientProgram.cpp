#include "SessionTable.h"

#include <cstdio>
#include <iostream>
#include <regex>

#define NAME_CLIENT "FTP CLIENT."
#define MAX_CHARACTER_READ 4

using namespace std;

bool inputCommand(int &command, int start, int end) {
	char buffer[MAX_CHARACTER_READ];
	string condition("\\d{1,4}");
	regex correctFunction(condition);

	cin.sync();
	cin.getline(buffer, MAX_CHARACTER_READ, '\n');
	string input(buffer);

	if (regex_match(input, correctFunction)) {
		command = std::stoi(input);
		
		if (command >= start && command <= end) {
			return true;
		}

	}

	return false;
}

bool createSession(int ID, SessionTable *sessTable) {
	string nameProgram;
	nameProgram.assign("FTP_Session.exe");

	string username;
	string password;
	string serverIP;
	cout << "Server IP = ";
	getline(cin, serverIP);

	cout << "User name (go \"enter\" de dang nhap che do anonymous user): ";
	getline(cin, username);
	if (username == "") {
		username.assign("anonymous");
	}

	cout << "Password: ";
	getline(cin, password);
	
	string arguments = nameProgram + " " + std::to_string(ID) + " " + serverIP + " " + username + " " + password;
	char cArguments[1000], cServerIP[100];
	strcpy_s(cArguments, 1000, arguments.c_str());
	strcpy_s(cServerIP, 100, serverIP.c_str());
	sessTable->createSession(cArguments, cServerIP);
	return true;
}

int main(int argc, char** argv) {
	SetConsoleTitle(NAME_CLIENT);
	system("chcp 65001");
	
	FILE *log = nullptr;
	freopen_s(&log, "FTPClientProgramLog.txt", "w+t", stderr);
	int command = 0;
	bool flag = true;

	SessionTable *sessTable = new SessionTable();
	do {
		while (1) {
			for (int i = 0; i < sessTable->getNumberSession(); i++) {
				if (sessTable->checkTerminatedProcess(i)) {
					sessTable->closeSession(i);
				}
			}

			system("cls");
			cout << "1. Tao mot phien lam viec voi Server." << endl;
			cout << "2. Xem danh sach cac phien lam viec." << endl;
			cout << "3. Huy phien lam viec." << endl;
			cout << "4. Dong chuong trinh." << endl;

			if (inputCommand(command, 1, 4)) {
				break;
			}

			system("cls");
			cout << "Khong co chuc nang nay. Ban vui long nhap lai: " << endl;
			system("pause");
		}

		switch (command) {
		case 1: {
			createSession(sessTable->getNumberSession(), sessTable);
			cout << "Session " << sessTable->getNumberSession() - 1 << " da duoc tao thanh cong." << endl;
			system("pause");
			break;
		}
		case 2: {
			sessTable->listSession();
			system("pause");
			break;
		}
		case 3: {
			if (sessTable->isEmpty()) {
				cout << "Khong co bat ky phien nao dang hoat dong." << endl;
				system("pause");
				break;
			}

			string id;
			cout << "Nhap so id cua phien can dong (all: De dong tat ca phien dang lam viec): ";
			cin >> id;
			if(id == "all") {
				sessTable->closeAllSession();
				cout << "Tat ca phien da duoc dong thanh cong." << endl;
				system("pause");
			} else {
				sessTable->closeSession(std::stoi(id));
				cout << "Phien " << id <<" da dong thanh cong." << endl;
				system("pause");
			}
			break;
		}
		case 4: {
			do {
				string option;
				cout << "Ban co muon ket thuc chuong trinh (Y/N): ";
				cin >> option;
				if (option == "y" || option == "Y") {
					flag = false;
					break;
				} else if (option == "n" || option == "N") {
					getchar();
					break;
				}
				cout << "Khong co lenh." << endl;
			} while(1);
		}
		}
	} while (flag);


	fclose(log);
	delete sessTable;
	return 0;
}
