#pragma once
#include "stdafx.h"
#include "sql.h"
#include "screen.h"

// Ŭ���̾�Ʈ ����, �г��� ���� ����
SOCKET client_sock;
string my_nick;

SQL _sql;

int chat_recv(); // �޽��� ���� �ޱ�(����->Ŭ���̾�Ʈ)
int room_count; // �� ����
int player; // �÷��̾� ��ȣ
int next_player; // ���� �÷��̾� ��ȣ
volatile int cur_player; // ���� �÷��̾�(���Ӽ���)
volatile int game; // ���� ���� ����(���� :0, ���� :1)
int board[15][15]; // ������

int main()
{
	bool loginSuccess = false;

	// ����ȭ�� ������
	while (!loginSuccess) {
		printStartMenu(); // ����ȭ��
		char startIn = 0;
		cout << "�� ";
		cin >> startIn;
		switch (startIn) {
		case '1': // 1. �α���
			printLogin();
			if (_sql.login() == 1) {
				loginSuccess = true;
				break; // �α��μ���(����ȭ�� ����)
			}
			continue;
		case '2': // 2. ȸ������
			printJoin();
			_sql.join();
			continue;
		case '0': // 0. �ý��� ����
			return -1;
		default: // 3~9. ���Է�
			cout << "���߸��� �Է��Դϴ�. �ٽ� �Է����ּ���." << endl;
			continue;
		}
	}

	// ����ȭ�� ������
	while (1) {
		printMainMenu(); // ����ȭ��
		char mainIn = 0;
		cout << "�� ";
		cin >> mainIn;
		bool backButton = false;
		int code = -1;

		switch (mainIn) {
		case '1': // ������ ������
			printMyProfile();
			backButton = false;
			while (!backButton) {
				char informationIn = 0;
				cout << "�� ";
				cin >> informationIn;
				switch (informationIn) {
				case '1':
					_sql.myProfile();
					break;
				case '2':
					_sql.updateStatus();
					continue;
				case '0':
					backButton = true;
					break;
				default:
					cout << "���߸��� �Է��Դϴ�. �ٽ� �Է����ּ���." << endl;
					continue;
				}
			}
			break;
		case '2': // ä�ù� ������
			printLobby();
			WSADATA wsa;
			code = WSAStartup(MAKEWORD(2, 2), &wsa);
			if (!code) {
				cout << "< ä�ù濡 �����մϴ�. >" << endl;
				my_nick = _sql.getId(); // �г���(���̵�) �ҷ�����
				closesocket(client_sock);
				client_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

				SOCKADDR_IN client_addr = {};
				client_addr.sin_family = AF_INET;
				client_addr.sin_port = htons(SERVER_PORT);
				InetPton(AF_INET, TEXT(SERVER_IP), &client_addr.sin_addr);

				// ���� ���� �� �г��� ����
				cout << "Connecting..." << endl;
				while (1) {
					if (!connect(client_sock, (SOCKADDR*)&client_addr, sizeof(client_addr))) {
						send(client_sock, my_nick.c_str(), my_nick.length(), 0);
						break;
					}
				}

				// �������� ä�� �޽����� ���޹ޱ� ���� ������ ����
				thread th2(chat_recv);
				
				// ������ �Է��� ���� ���ڸ� ����
				cin.ignore();
				// �������� ä�� �޽����� ������
				while (1) {
					string text;
					std::getline(cin, text);
					send(client_sock, text.c_str(), MAX_SIZE, 0);
					if (text == "/����") {
						closesocket(client_sock);
						break;
					}
					else if (text == "/����") {
						// �� ����, �Էµ� �� �̸� ������ ����
						cout << "�� �̸��� �Է����ּ��� :";
						string room_name;
						std::getline(cin, room_name);
						send(client_sock, room_name.c_str(), MAX_SIZE, 0);
						// �� ���� ���
						Sleep(500);

						// �÷��̾� ���� ����
						player = 1;
						next_player = 2;
						cur_player = player;

						// ���ӽ���
						while (!game); // ���� ���� ���
						OmokPlay(&game, player, next_player, &cur_player, board, client_sock, _sql);

						// ���� ���� �޽��� ���, ���� ���� ����
						gotoxy(0, 17);
						cout << "������ ����Ǿ����ϴ�.\n5�� �� ���θ޴��� �̵��մϴ�...";
						Sleep(5000);
						closesocket(client_sock);
						break;
					}
					else if (text == "/����") {
						// ������ �� ��ȣ �Է�
						cout << "������ �� ��ȣ�� �Է����ּ��� :";
						char num;
						cin >> num;
						send(client_sock, &num, MAX_SIZE, 0);
						if (num - '0' > room_count || num - '0' <= 0) {
							cout << "�� ��ȣ�� ��Ȯ���� �ʽ��ϴ�. (���� �� ���� :" << room_count << ")" << endl;
							continue;
						}
						// �� ���� ���
						Sleep(500);

						// �÷��̾� ���� ����
						player = 2;
						next_player = 1;
						cur_player = next_player;

						// ���ӽ���
						while (!game); // ���� ���� ���
						OmokPlay(&game, player, next_player, &cur_player, board, client_sock, _sql);

						// ���� ���� �޽��� ���, ���� ���� ����
						gotoxy(0, 17);
						cout << "������ ����Ǿ����ϴ�.\n5�� �� ���θ޴��� �̵��մϴ�...";
						Sleep(5000);
						closesocket(client_sock);
						break;
					}
				}
				th2.join();
			}
			break;
		case '3': // ������ ����
			printSetting();
			backButton = false;
			while (!backButton) {
				char settingIn = 0;
				cout << "�� ";
				cin >> settingIn;
				switch (settingIn) {
				case '1': // ��й�ȣ ����
					_sql.modifyPw();
					continue;
				case '2': // ȸ��Ż��
					if (_sql.deleteUser() == 1) return -1;
					else backButton = true;
				case '0': // �ڷΰ���
					backButton = true;
					break;
				default:
					cout << "���߸��� �Է��Դϴ�. �ٽ� �Է����ּ���." << endl;
				}
			}
			break;
		case '0': // ����
			cout << "���α׷��� �����մϴ�." << endl;
			return 0;
		default:
			cout << "���߸��� �Է��Դϴ�. �ٽ� �Է����ּ���." << endl;
		}
	}
	return 0;
}

// �޽��� ���� �ޱ�(����->Ŭ���̾�Ʈ)
int chat_recv() { 
	char buf[MAX_SIZE] = { };
	string msg;

	while (1) {
		ZeroMemory(&buf, MAX_SIZE);
		if (recv(client_sock, buf, MAX_SIZE, 0) > 0) {
			msg = buf;
			// "room_list" ����, �� ��� ���
			if (msg == "room_list") {
				if (!game) { // �������� �ƴ� ��쿡�� ���(ä�� �κ�)
					room_count = printRoomList(client_sock);
				}
			}
			// ������ �濡 �����ϰ�, �г����� ���޹����� ���� ����
			else if (_sql.matchId(buf)) {
				// ������ ���ӽ��� �޽��� ����, game ���� ����(����)
				send(client_sock, "omok_play", MAX_SIZE, 0);
				game = 1; 
				// ������ �迭 �ʱ�ȭ, ������, �г��� ���
				ZeroMemory(board, sizeof(board));
				printOmok();
				if (player == 1) printNick(my_nick, buf);
				else printNick(buf, my_nick);
			} 
			// ���� ������, ���ʰ� ����ɶ� �� player ��ȣ�� ���޹���
			else if (buf[0] - '0' == player) {
				ZeroMemory(&buf, MAX_SIZE);
				// ������ ���� ���� ��ġ�� ���޹��� ("x|y|" ���·� ����)
				if (recv(client_sock, buf, MAX_SIZE, 0) > 0) {
					std::stringstream ss(buf);
					string num;
					int i = 0;
					int _pos[2] = { 0 };
					// '|' �� ���������� �߶�, x �� y ���� ����
					while (getline(ss, num, '|')) {
						_pos[i] = stoi(num);
						i++;
					}
					// ���޹��� ��ġ�� ������ ���� ���´�
					gotoxy(_pos[0], _pos[1]);
					cout << next_player;
					board[_pos[1] - 1][(_pos[0] - 1) / 3] = next_player;
				}
				// ��� ���ʰ� ������, �� ���ʸ� ����
				printCurPlayer(player, next_player);
				cur_player = player;
			} 
			// ���� ������ gameover ���޹���
			else if (msg == "gameover") {
				// game ���� ����(����)
				game = 0;
			}
			// �Ϲ� �޽��� ���
			else {
				cout << buf << endl;
			}
		}
		else {
			//cout << "Server Connection Off" << endl;
			return -1;
		}
	}
}