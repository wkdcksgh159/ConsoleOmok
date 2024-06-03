#pragma once
#include "stdafx.h"
#include "sql.h"
#include "screen.h"

// ���� Ŀ�� ��ġ Ȯ��
void currentCursor(int* _cur_pos)
{
	CONSOLE_SCREEN_BUFFER_INFO cur;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cur);
	_cur_pos[0] = cur.dwCursorPosition.X;
	_cur_pos[1] = cur.dwCursorPosition.Y;
}

// x, y ��ġ�� Ŀ�� �̵�
void gotoxy(int x, int y) 
{
	COORD Cur;
	Cur.X = x;
	Cur.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Cur);
}

// ������ �г��� ���
void printNick(string p1, string p2) {
	gotoxy(49, 1);
	cout << p1;
	gotoxy(49, 2);
	cout << p2;
}

// ���� ���� Ȯ��
void printCurPlayer(int _player, int _next_player) {
	gotoxy(47, _next_player);
	cout << " ";
	gotoxy(47, _player);
	//cout << "��";
	cout << ">";
}

// ������ ��� 15x15
void printOmok() { 
	system("cls");
	cout << "                  [O M O K]\n";
	cout << "[ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ]\n";
	cout << "[ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ]\n";
	cout << "[ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ]\n";
	cout << "[ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ]\n";
	cout << "[ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ]-\n";
	cout << "[ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ]\n";
	cout << "[ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ]\n";
	cout << "[ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ]\n";
	cout << "[ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ]\n";
	cout << "[ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ]-\n";
	cout << "[ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ]\n";
	cout << "[ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ]\n";
	cout << "[ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ]\n";
	cout << "[ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ]\n";
	cout << "[ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ][ ]-\n";
	cout << ">> �̵� : ����Ű, Ȯ�� : space\n";
}

// �κ�ȭ��
void printLobby() {
	system("cls");
	cout << "                  [R O O M]\n";
	cout << "[01][                                          ]\n";
	cout << "[02][                                          ]\n";
	cout << "[03][                                          ]\n";
	cout << "[04][                                          ]\n";
	cout << "[05][                                          ]\n";
	cout << "[06][                                          ]\n";
	cout << "[07][                                          ]\n";
	cout << "[08][                                          ]\n";
	cout << "[09][                                          ]\n";
	cout << ">> �� ���� :/����, �� ���� :/����, ���� :/����\n";
}

// ���� ��� Ȯ��
int omok(int row, int col, int _player, int __board[15][15]) {
	// �������� �̵�
	int front_dr[] = { 1, 1, 1, 0 };
	int front_dc[] = { -1, 0, 1, -1 };
	// �Ĺ����� �̵�
	int back_dr[] = { -1, -1, -1, 0 };
	int back_dc[] = { 1, 0, -1, 1 };

	// ����, ����, �밢�� 4���� üũ
	for (int i = 0; i < 4; i++) {
		// ������ġ���� �յڷ� üũ
		int front_nr = row + front_dr[i];
		int front_nc = col + front_dc[i];
		int back_nr = row + back_dr[i];
		int back_nc = col + back_dc[i];
		// ���� ī��Ʈ
		int cnt = 1;
		// �������� �̵��ϸ鼭 �÷��̾��� ���� �ִ� ��� ī��Ʈ ����
		while (__board[front_nr][front_nc] == _player) {
			cnt++;
			front_nr += front_dr[i];
			front_nc += front_dc[i];
		}
		// �Ĺ����� �̵��ϸ鼭 �÷��̾��� ���� �ִ� ��� ī��Ʈ ����
		while (__board[back_nr][back_nc] == _player) {
			cnt++;
			back_nr += back_dr[i];
			back_nc += back_dc[i];
		}
		// ���� �ϼ��� 1 ����(6�� �Ұ�)
		if (cnt == 5) return 1;
	}
	// ���� �̿ϼ��� 0 ����
	return 0;
}

// ���� ����
void OmokPlay(int *game, int _player, int _next_player, int *cur_player\
			, int _board[15][15], SOCKET _client_sock, SQL &__sql) {
	// �÷��̾� 1 ���� Ȯ��, ������ġ�� Ŀ�� �̵�
	printCurPlayer(1, 2);
	// Ŀ�� ��ġ �ʱ�ȭ
	gotoxy(1, 1);
	int cur_x = 1;
	int cur_y = 1;
	// ��, ��, ��, �� 
	int end_line[4] = { cur_y, cur_y + 14, cur_x, cur_x + 14 * 3 };
	while (*game) {
		// ���� ���ʰ� �ƴ� ��� Ű���� �Է� �Ұ�
		while (*cur_player != _player);
		// Ű���� �Է�
		if (_kbhit()) {
			char c = _getch();
			if (c == -32) {
				c = _getch();
			}
			switch (c) {
			case UP:
				if (cur_y - 1 >= end_line[0]) cur_y = cur_y - 1;
				break;
			case DOWN:
				if (cur_y + 1 <= end_line[1]) cur_y = cur_y + 1;
				break;
			case LEFT:
				if (cur_x - 3 >= end_line[2]) cur_x = cur_x - 3;
				break;
			case RIGHT:
				if (cur_x + 3 <= end_line[3]) cur_x = cur_x + 3;
				break;
			// ���� Ŀ�� ��ġ�� ���� ����
			case SPACE:
				// ���� �迭 �Է�, ���� ��ġ�� �̹� ���� �ִ� ��� �Ұ�
				int _row = cur_y - 1;
				int _col = (cur_x - 1) / 3;
				if (_board[_row][_col] == 0) {
					_board[_row][_col] = _player;
				}
				else {
					break;
				}
				// ���� ��ġ�� player ��ȣ ���
				cout << _player;
				// ���� ����
				printCurPlayer(_next_player, _player);
				// "�������Ῡ��|�÷��̾�|x|y|" ���� ���� ���ڿ��� ������ ����
				string msg;
				msg = std::to_string((*game)) + '|' + std::to_string(_player) + '|' + \
					std::to_string(cur_x) + '|' + std::to_string(cur_y) + "|";
				send(_client_sock, msg.c_str(), MAX_SIZE, 0);
				// ���� �ϼ� Ȯ��, �̿ϼ��� ���� ����
				if (omok(_row, _col, _player, _board)) {
					// ���� �ϼ��� ���� �¸� �� ����, ���� ���� Ȯ���� ������ ����
					__sql.updateWin();
					send(_client_sock, "0|", MAX_SIZE, 0);
				}
				else {
					*cur_player = _next_player;
				}
				break;
			}
			// ����� x, y ��ǥ�� Ŀ�� �̵�
			gotoxy(cur_x, cur_y);
		}
	}
}

// �� �̸� ���
int printRoomList(SOCKET _client_sock) {
	// ���� Ŀ�� ��ġ ����
	int cur_pos[2] = { 0, };
	currentCursor(cur_pos);
	char _buf[MAX_SIZE] = { };
	// ������ ���� �� ��� ����
	if (recv(_client_sock, _buf, MAX_SIZE, 0)) {
		std::stringstream ss(_buf);
		string room;
		int idx = 0;
		// ���Ź��� �� ����� ���
		while (getline(ss, room, '|')) {
			gotoxy(5, idx + 1);
			cout << room;
			idx++;
		}
		// ó�� Ŀ�� ��ġ�� �̵�
		gotoxy(cur_pos[0], cur_pos[1]);
		// �� ���� ����
		return idx;
	}
	return 0;
}

// ����ȭ��
void printStartMenu() { 
	system("cls");
	cout << " ----------------------------------- \n";
	cout << "|              [OMOK]               |\n";
	cout << "|                                   |\n";
	cout << "|            1. �α���              |\n";
	cout << "|                                   |\n";
	cout << "|            2. ȸ������            |\n";
	cout << "|                                   |\n";
	cout << "|            0. ����                |\n";
	cout << "|                                   |\n";
	cout << " ----------------------------------- \n";
}

// �α���ȭ��
void printLogin() { 
	system("cls");
	cout << " ----------------------------------- \n";
	cout << "|             [�α���]              |\n";
	cout << "|                                   |\n";
	cout << "|           >> ���̵� �Է�          |\n";
	cout << "|                                   |\n";
	cout << "|           >> ��й�ȣ �Է�        |\n";
	cout << "|                                   |\n";
	cout << "|           0. �ڷΰ���             |\n";
	cout << "|                                   |\n";
	cout << " ----------------------------------- \n";
}

// ȸ������ȭ��
void printJoin() { 
	system("cls");
	cout << " ----------------------------------- \n";
	cout << "|             [ȸ������]            |\n";
	cout << "|                                   |\n";
	cout << "|           >> ���̵� �Է�          |\n";
	cout << "|                                   |\n";
	cout << "|           >> ��й�ȣ �Է�        |\n";
	cout << "|                                   |\n";
	cout << "|           >> ��й�ȣ �Է� Ȯ��   |\n";
	cout << "|                                   |\n";
	cout << "|           0. �ڷΰ���             |\n";
	cout << "|                                   |\n";
	cout << " ----------------------------------- \n";
}

// ���θ޴�ȭ��
void printMainMenu() { 
	system("cls");
	cout << " ----------------------------------- \n";
	cout << "|              [OMOK]               |\n";
	cout << "|                                   |\n";
	cout << "|            1. ������              |\n";
	cout << "|                                   |\n";
	cout << "|            2. �κ� ����           |\n";
	cout << "|                                   |\n";
	cout << "|            3. ����                |\n";
	cout << "|                                   |\n";
	cout << "|            0. ����                |\n";
	cout << "|                                   |\n";
	cout << " ----------------------------------- \n";
}

// ������ȭ��
void printMyProfile() { 
	system("cls");
	cout << " ----------------------------------- \n";
	cout << "|              [������]             |\n";
	cout << "|                                   |\n";
	cout << "|            1. �� ������           |\n";
	cout << "|                                   |\n";
	cout << "|            2. ���¸޽��� ����     |\n";
	cout << "|                                   |\n";
	cout << "|            0. �ڷΰ���            |\n";
	cout << "|                                   |\n";
	cout << " ----------------------------------- \n";
}

// ����ȭ��
void printSetting() { 
	system("cls");
	cout << " ----------------------------------- \n";
	cout << "|              [����]               |\n";
	cout << "|                                   |\n";
	cout << "|            1. ��й�ȣ ����       |\n";
	cout << "|                                   |\n";
	cout << "|            2. ȸ�� Ż��           |\n";
	cout << "|                                   |\n";
	cout << "|            0. �ڷΰ���            |\n";
	cout << "|                                   |\n";
	cout << " ----------------------------------- \n";
}