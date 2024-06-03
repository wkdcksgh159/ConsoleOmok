#pragma once
#include "stdafx.h"
#include "sql.h"
#include "screen.h"

// 현재 커서 위치 확인
void currentCursor(int* _cur_pos)
{
	CONSOLE_SCREEN_BUFFER_INFO cur;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cur);
	_cur_pos[0] = cur.dwCursorPosition.X;
	_cur_pos[1] = cur.dwCursorPosition.Y;
}

// x, y 위치로 커서 이동
void gotoxy(int x, int y) 
{
	COORD Cur;
	Cur.X = x;
	Cur.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Cur);
}

// 오목판 닉네임 출력
void printNick(string p1, string p2) {
	gotoxy(49, 1);
	cout << p1;
	gotoxy(49, 2);
	cout << p2;
}

// 현재 차례 확인
void printCurPlayer(int _player, int _next_player) {
	gotoxy(47, _next_player);
	cout << " ";
	gotoxy(47, _player);
	//cout << "▶";
	cout << ">";
}

// 오목판 출력 15x15
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
	cout << ">> 이동 : 방향키, 확인 : space\n";
}

// 로비화면
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
	cout << ">> 방 생성 :/생성, 방 입장 :/입장, 종료 :/종료\n";
}

// 오목 결과 확인
int omok(int row, int col, int _player, int __board[15][15]) {
	// 전방으로 이동
	int front_dr[] = { 1, 1, 1, 0 };
	int front_dc[] = { -1, 0, 1, -1 };
	// 후방으로 이동
	int back_dr[] = { -1, -1, -1, 0 };
	int back_dc[] = { 1, 0, -1, 1 };

	// 가로, 세로, 대각선 4방향 체크
	for (int i = 0; i < 4; i++) {
		// 현재위치에서 앞뒤로 체크
		int front_nr = row + front_dr[i];
		int front_nc = col + front_dc[i];
		int back_nr = row + back_dr[i];
		int back_nc = col + back_dc[i];
		// 오목 카운트
		int cnt = 1;
		// 전방으로 이동하면서 플레이어의 돌이 있는 경우 카운트 증가
		while (__board[front_nr][front_nc] == _player) {
			cnt++;
			front_nr += front_dr[i];
			front_nc += front_dc[i];
		}
		// 후방으로 이동하면서 플레이어의 돌이 있는 경우 카운트 증가
		while (__board[back_nr][back_nc] == _player) {
			cnt++;
			back_nr += back_dr[i];
			back_nc += back_dc[i];
		}
		// 오목 완성시 1 리턴(6목 불가)
		if (cnt == 5) return 1;
	}
	// 오목 미완성시 0 리턴
	return 0;
}

// 오목 시작
void OmokPlay(int *game, int _player, int _next_player, int *cur_player\
			, int _board[15][15], SOCKET _client_sock, SQL &__sql) {
	// 플레이어 1 차례 확인, 시작위치로 커서 이동
	printCurPlayer(1, 2);
	// 커서 위치 초기화
	gotoxy(1, 1);
	int cur_x = 1;
	int cur_y = 1;
	// 상, 하, 좌, 우 
	int end_line[4] = { cur_y, cur_y + 14, cur_x, cur_x + 14 * 3 };
	while (*game) {
		// 본인 차례가 아닌 경우 키보드 입력 불가
		while (*cur_player != _player);
		// 키보드 입력
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
			// 현재 커서 위치에 돌을 놓음
			case SPACE:
				// 오목 배열 입력, 현재 위치에 이미 돌이 있는 경우 불가
				int _row = cur_y - 1;
				int _col = (cur_x - 1) / 3;
				if (_board[_row][_col] == 0) {
					_board[_row][_col] = _player;
				}
				else {
					break;
				}
				// 현재 위치에 player 번호 출력
				cout << _player;
				// 차례 변경
				printCurPlayer(_next_player, _player);
				// "게임종료여부|플레이어|x|y|" 값을 담은 문자열을 서버로 전송
				string msg;
				msg = std::to_string((*game)) + '|' + std::to_string(_player) + '|' + \
					std::to_string(cur_x) + '|' + std::to_string(cur_y) + "|";
				send(_client_sock, msg.c_str(), MAX_SIZE, 0);
				// 오목 완성 확인, 미완성시 차례 변경
				if (omok(_row, _col, _player, _board)) {
					// 오목 완성시 유저 승리 수 증가, 게임 종료 확인을 서버로 전송
					__sql.updateWin();
					send(_client_sock, "0|", MAX_SIZE, 0);
				}
				else {
					*cur_player = _next_player;
				}
				break;
			}
			// 변경된 x, y 좌표로 커서 이동
			gotoxy(cur_x, cur_y);
		}
	}
}

// 방 이름 출력
int printRoomList(SOCKET _client_sock) {
	// 현재 커서 위치 저장
	int cur_pos[2] = { 0, };
	currentCursor(cur_pos);
	char _buf[MAX_SIZE] = { };
	// 서버로 부터 방 목록 수신
	if (recv(_client_sock, _buf, MAX_SIZE, 0)) {
		std::stringstream ss(_buf);
		string room;
		int idx = 0;
		// 수신받은 방 목록을 출력
		while (getline(ss, room, '|')) {
			gotoxy(5, idx + 1);
			cout << room;
			idx++;
		}
		// 처음 커서 위치로 이동
		gotoxy(cur_pos[0], cur_pos[1]);
		// 방 개수 리턴
		return idx;
	}
	return 0;
}

// 시작화면
void printStartMenu() { 
	system("cls");
	cout << " ----------------------------------- \n";
	cout << "|              [OMOK]               |\n";
	cout << "|                                   |\n";
	cout << "|            1. 로그인              |\n";
	cout << "|                                   |\n";
	cout << "|            2. 회원가입            |\n";
	cout << "|                                   |\n";
	cout << "|            0. 종료                |\n";
	cout << "|                                   |\n";
	cout << " ----------------------------------- \n";
}

// 로그인화면
void printLogin() { 
	system("cls");
	cout << " ----------------------------------- \n";
	cout << "|             [로그인]              |\n";
	cout << "|                                   |\n";
	cout << "|           >> 아이디 입력          |\n";
	cout << "|                                   |\n";
	cout << "|           >> 비밀번호 입력        |\n";
	cout << "|                                   |\n";
	cout << "|           0. 뒤로가기             |\n";
	cout << "|                                   |\n";
	cout << " ----------------------------------- \n";
}

// 회원가입화면
void printJoin() { 
	system("cls");
	cout << " ----------------------------------- \n";
	cout << "|             [회원가입]            |\n";
	cout << "|                                   |\n";
	cout << "|           >> 아이디 입력          |\n";
	cout << "|                                   |\n";
	cout << "|           >> 비밀번호 입력        |\n";
	cout << "|                                   |\n";
	cout << "|           >> 비밀번호 입력 확인   |\n";
	cout << "|                                   |\n";
	cout << "|           0. 뒤로가기             |\n";
	cout << "|                                   |\n";
	cout << " ----------------------------------- \n";
}

// 메인메뉴화면
void printMainMenu() { 
	system("cls");
	cout << " ----------------------------------- \n";
	cout << "|              [OMOK]               |\n";
	cout << "|                                   |\n";
	cout << "|            1. 내정보              |\n";
	cout << "|                                   |\n";
	cout << "|            2. 로비 입장           |\n";
	cout << "|                                   |\n";
	cout << "|            3. 설정                |\n";
	cout << "|                                   |\n";
	cout << "|            0. 종료                |\n";
	cout << "|                                   |\n";
	cout << " ----------------------------------- \n";
}

// 내정보화면
void printMyProfile() { 
	system("cls");
	cout << " ----------------------------------- \n";
	cout << "|              [내정보]             |\n";
	cout << "|                                   |\n";
	cout << "|            1. 내 프로필           |\n";
	cout << "|                                   |\n";
	cout << "|            2. 상태메시지 설정     |\n";
	cout << "|                                   |\n";
	cout << "|            0. 뒤로가기            |\n";
	cout << "|                                   |\n";
	cout << " ----------------------------------- \n";
}

// 설정화면
void printSetting() { 
	system("cls");
	cout << " ----------------------------------- \n";
	cout << "|              [설정]               |\n";
	cout << "|                                   |\n";
	cout << "|            1. 비밀번호 변경       |\n";
	cout << "|                                   |\n";
	cout << "|            2. 회원 탈퇴           |\n";
	cout << "|                                   |\n";
	cout << "|            0. 뒤로가기            |\n";
	cout << "|                                   |\n";
	cout << " ----------------------------------- \n";
}