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
	nameProgram.assign("");

	string username;
	string password;
	string serverIP;
	cout << "Server IP = " << endl;
	cin >> serverIP;
	cout << "User name (gõ \"enter\" để đăng nhập chế độ anonymous user): " << endl;
	cin >> username;
	if (username == "") {
		username.assign("anonymous");
	}
	cout << "Password: " << endl;
	cin >> password;
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
	//system("cls");
	FILE *log = nullptr;
	freopen_s(&log, "FTPClientProgramLog.txt", "w+t", stderr);
	int command = 0;
	bool flag = true;

	SessionTable *sessTable = new SessionTable();
	do {
		while (1) {
			cout << "1. Tạo 1 phiên làm việc với Server." << endl;
			cout << "2. Xem danh sách các phiên làm việc với Server." << endl;
			cout << "3. Hủy phiên làm việc." << endl;
			cout << "4. Kết thúc chương trình." << endl;

			if (inputCommand(command, 1, 4)) {
				break;
			}

			system("cls");
			cout << "Không có chức năng này. Bạn vui lòng nhập lại" << endl;
		}

		switch (command) {
		case 1: {
			createSession(sessTable->getNumberSession(), sessTable);
			cout << "Session " << sessTable->getNumberSession() - 1 << " đã được tạo thành công." << endl;
			break;
		}
		case 2: {
			sessTable->listSession();
			cout << "Nhập 1 phím bất kỳ để tiếp tục." << endl;
			system("pause");
			break;
		}
		case 3: {
			string id;
			cout << "Nhập số ID của phiên cần kết thúc (all: Để kết thúc tất cả session).";
			cin >> id;
			if(id == "all") {
				sessTable->closeAllSession();
				cout << "Tất cả session đã kết thúc thành công." << endl;
			} else {
				sessTable->closeSession(std::stoi(id));
				cout << "Session " << id <<" đã kết thúc thành công." << endl;
			}
			break;
		}
		case 4: {
			do {
				string option;
				cout << "Bạn có muốn kết thúc chương trình (Y/N): ";
				cin >> option;
				if (option == "y" || option == "Y") {
					flag = false;
					break;
				} else if (option == "n" || option == "N") {
					break;
				}
				cout << "Không có lệnh." << endl;
			} while(1);
		}
		}
	} while (flag);


	fclose(log);
	delete sessTable;
	return 0;
}
