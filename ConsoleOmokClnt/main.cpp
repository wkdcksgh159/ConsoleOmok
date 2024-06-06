#pragma once
#include "stdafx.h"
#include "sql.h"
#include "screen.h"

// 클라이언트 소켓, 닉네임 변수 선언
SOCKET client_sock;
string my_nick;

SQL _sql;

int chat_recv(); // 메시지 전달 받기(서버->클라이언트)
int room_count; // 방 개수
int player; // 플레이어 번호
int next_player; // 다음 플레이어 번호
volatile int cur_player; // 현재 플레이어(게임순서)
volatile int game; // 게임 실행 유무(종료 :0, 실행 :1)
int board[15][15]; // 오목판

int main()
{
	bool loginSuccess = false;

	// 시작화면 구현부
	while (!loginSuccess) {
		printStartMenu(); // 시작화면
		char startIn = 0;
		cout << "▶ ";
		cin >> startIn;
		switch (startIn) {
		case '1': // 1. 로그인
			printLogin();
			if (_sql.login() == 1) {
				loginSuccess = true;
				break; // 로그인성공(시작화면 종료)
			}
			continue;
		case '2': // 2. 회원가입
			printJoin();
			_sql.join();
			continue;
		case '0': // 0. 시스템 종료
			return -1;
		default: // 3~9. 재입력
			cout << "▶잘못된 입력입니다. 다시 입력해주세요." << endl;
			continue;
		}
	}

	// 메인화면 구현부
	while (1) {
		printMainMenu(); // 메인화면
		char mainIn = 0;
		cout << "▶ ";
		cin >> mainIn;
		bool backButton = false;
		int code = -1;

		switch (mainIn) {
		case '1': // 내정보 구현부
			printMyProfile();
			backButton = false;
			while (!backButton) {
				char informationIn = 0;
				cout << "▶ ";
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
					cout << "▶잘못된 입력입니다. 다시 입력해주세요." << endl;
					continue;
				}
			}
			break;
		case '2': // 채팅방 구현부
			printLobby();
			WSADATA wsa;
			code = WSAStartup(MAKEWORD(2, 2), &wsa);
			if (!code) {
				cout << "< 채팅방에 입장합니다. >" << endl;
				my_nick = _sql.getId(); // 닉네임(아이디) 불러오기
				closesocket(client_sock);
				client_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

				SOCKADDR_IN client_addr = {};
				client_addr.sin_family = AF_INET;
				client_addr.sin_port = htons(SERVER_PORT);
				InetPton(AF_INET, TEXT(SERVER_IP), &client_addr.sin_addr);

				// 서버 연결 및 닉네임 전송
				cout << "Connecting..." << endl;
				while (1) {
					if (!connect(client_sock, (SOCKADDR*)&client_addr, sizeof(client_addr))) {
						send(client_sock, my_nick.c_str(), my_nick.length(), 0);
						break;
					}
				}

				// 서버에게 채팅 메시지를 전달받기 위한 스레드 생성
				thread th2(chat_recv);
				
				// 마지막 입력의 개행 문자를 제거
				cin.ignore();
				// 서버에게 채팅 메시지를 보내기
				while (1) {
					string text;
					std::getline(cin, text);
					send(client_sock, text.c_str(), MAX_SIZE, 0);
					if (text == "/종료") {
						closesocket(client_sock);
						break;
					}
					else if (text == "/생성") {
						// 방 생성, 입력된 방 이름 서버로 전달
						cout << "방 이름을 입력해주세요 :";
						string room_name;
						std::getline(cin, room_name);
						send(client_sock, room_name.c_str(), MAX_SIZE, 0);
						// 방 생성 대기
						Sleep(500);

						// 플레이어 순서 지정
						player = 1;
						next_player = 2;
						cur_player = player;

						// 게임시작
						while (!game); // 게임 실행 대기
						OmokPlay(&game, player, next_player, &cur_player, board, client_sock, _sql);

						// 게임 종료 메시지 출력, 소켓 연결 종료
						gotoxy(0, 17);
						cout << "게임이 종료되었습니다.\n5초 후 메인메뉴로 이동합니다...";
						Sleep(5000);
						closesocket(client_sock);
						break;
					}
					else if (text == "/입장") {
						// 입장할 방 번호 입력
						cout << "입장할 방 번호를 입력해주세요 :";
						char num;
						cin >> num;
						send(client_sock, &num, MAX_SIZE, 0);
						if (num - '0' > room_count || num - '0' <= 0) {
							cout << "방 번호가 정확하지 않습니다. (현재 방 개수 :" << room_count << ")" << endl;
							continue;
						}
						// 방 입장 대기
						Sleep(500);

						// 플레이어 순서 지정
						player = 2;
						next_player = 1;
						cur_player = next_player;

						// 게임시작
						while (!game); // 게임 실행 대기
						OmokPlay(&game, player, next_player, &cur_player, board, client_sock, _sql);

						// 게임 종료 메시지 출력, 소켓 연결 종료
						gotoxy(0, 17);
						cout << "게임이 종료되었습니다.\n5초 후 메인메뉴로 이동합니다...";
						Sleep(5000);
						closesocket(client_sock);
						break;
					}
				}
				th2.join();
			}
			break;
		case '3': // 설정부 구현
			printSetting();
			backButton = false;
			while (!backButton) {
				char settingIn = 0;
				cout << "▶ ";
				cin >> settingIn;
				switch (settingIn) {
				case '1': // 비밀번호 변경
					_sql.modifyPw();
					continue;
				case '2': // 회원탈퇴
					if (_sql.deleteUser() == 1) return -1;
					else backButton = true;
				case '0': // 뒤로가기
					backButton = true;
					break;
				default:
					cout << "▶잘못된 입력입니다. 다시 입력해주세요." << endl;
				}
			}
			break;
		case '0': // 종료
			cout << "프로그램을 종료합니다." << endl;
			return 0;
		default:
			cout << "▶잘못된 입력입니다. 다시 입력해주세요." << endl;
		}
	}
	return 0;
}

// 메시지 전달 받기(서버->클라이언트)
int chat_recv() { 
	char buf[MAX_SIZE] = { };
	string msg;

	while (1) {
		ZeroMemory(&buf, MAX_SIZE);
		if (recv(client_sock, buf, MAX_SIZE, 0) > 0) {
			msg = buf;
			// "room_list" 실행, 방 목록 출력
			if (msg == "room_list") {
				if (!game) { // 게임중이 아닌 경우에만 출력(채팅 로비)
					room_count = printRoomList(client_sock);
				}
			}
			// 상대방이 방에 입장하고, 닉네임을 전달받으면 게임 실행
			else if (_sql.matchId(buf)) {
				// 서버로 게임실행 메시지 전달, game 변수 변경(실행)
				send(client_sock, "omok_play", MAX_SIZE, 0);
				game = 1; 
				// 오목판 배열 초기화, 오목판, 닉네임 출력
				ZeroMemory(board, sizeof(board));
				printOmok();
				if (player == 1) printNick(my_nick, buf);
				else printNick(buf, my_nick);
			} 
			// 게임 진행중, 차례가 변경될때 내 player 번호를 전달받음
			else if (buf[0] - '0' == player) {
				ZeroMemory(&buf, MAX_SIZE);
				// 상대방이 놓은 돌의 위치를 전달받음 ("x|y|" 형태로 전달)
				if (recv(client_sock, buf, MAX_SIZE, 0) > 0) {
					std::stringstream ss(buf);
					string num;
					int i = 0;
					int _pos[2] = { 0 };
					// '|' 를 기준점으로 잘라서, x 와 y 값을 저장
					while (getline(ss, num, '|')) {
						_pos[i] = stoi(num);
						i++;
					}
					// 전달받은 위치에 상대방의 돌을 놓는다
					gotoxy(_pos[0], _pos[1]);
					cout << next_player;
					board[_pos[1] - 1][(_pos[0] - 1) / 3] = next_player;
				}
				// 상대 차례가 끝나고, 내 차례를 시작
				printCurPlayer(player, next_player);
				cur_player = player;
			} 
			// 오목 성공시 gameover 전달받음
			else if (msg == "gameover") {
				// game 변수 변경(종료)
				game = 0;
			}
			// 일반 메시지 출력
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