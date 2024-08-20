#include "main.h"
int main()
{
	system("color 06");

	// 데이터베이스 연결
	startSql();
	// 메인화면 출력
	printMain();

	WSADATA wsa;

	int code = WSAStartup(MAKEWORD(2, 2), &wsa);
	if (!code) {
		// 서버 소켓 초기화
		serverInit();
		thread th1[MAX_CLIENT];

		// 클라이언트 연결 요청 대기
		for (int i = 0; i < MAX_CLIENT; i++) {
			th1[i] = thread(addClient);
		}

		// 연결된 클라이언트에게 메시지 전달
		while (1) {
			string text, msg = "";
			getline(cin, text);
			const char* buf = text.c_str();
			msg = server_sock.user + " : " + buf;
			sendMsg(msg.c_str());
		}

		// 스레드 종료 대기
		for (int i = 0; i < MAX_CLIENT; i++) {
			th1[i].join();
		}

		closesocket(server_sock.sck);
	}
	else cout << "프로그램 종료. (Error code : " << code << ")";

	WSACleanup();

	delete result;
	delete pstmt;
	delete con;
	delete stmt;

	return 0;
}

// 데이터베이스 연결
void startSql() { 
	try {
		driver = sql::mysql::get_mysql_driver_instance();
		con = driver->connect(server, username, password);
		con->setSchema("omok"); // omok DB
	}
	catch (sql::SQLException& e) {
		cout << "Could not connect to server. Error message: " << e.what() << endl;
		exit(1);
	}

	stmt = con->createStatement();
	stmt->execute("set names euckr"); // euckr 인코딩
	if (stmt) { delete stmt; stmt = nullptr; }
}

// 메인화면
void printMain() { 
	system("cls");
	cout << " ----------------------------------- \n";
	cout << "|              [OMOK]               |\n";
	cout << "|                                   |\n";
	cout << "|           < SERVER ON >           |\n";
	cout << "|                                   |\n";
	cout << " ----------------------------------- \n";
}

// 서버 소켓 초기화
void serverInit() { 
	server_sock.sck = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN server_addr = {};
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	bind(server_sock.sck, (sockaddr*)&server_addr, sizeof(server_addr));
	listen(server_sock.sck, SOMAXCONN);

	server_sock.user = "server";
}

// 클라이언트 추가
void addClient() { 
	SOCKADDR_IN addr = {};
	int addrsize = sizeof(addr);
	char buf[MAX_SIZE] = { };
	ZeroMemory(&addr, addrsize);

	SOCKET_INFO new_client = {};
	new_client.sck = accept(server_sock.sck, (sockaddr*)&addr, &addrsize);

	// 클라이언트 유저 이름 전달받기
	recv(new_client.sck, buf, MAX_SIZE, 0);
	new_client.user = string(buf);
	string msg = "▶" + new_client.user + " 님이 입장했습니다.";
	cout << msg << "\n";

	int i;
	// 소켓 목록 추가
	for (i = 0; i < MAX_CLIENT; i++) {
		if (!sck_list[i].used) { // 사용 여부 확인
			sck_list[i] = new_client;
			sck_list[i].used = true;
			break;
		}
	}
	printClients();

	// recvMsg 스레드 추가
	thread th(recvMsg, i);
	th.detach();

	client_count++;
	cout << "▷현재 접속자 수 : " << client_count << "명" << endl;

	// 입장 확인, 방 목록 송신
	sendMsg(msg.c_str());
	sendRoomList();
}

// 클라이언트 제거
void delClient(int idx) {
	// 해당 유저가 만든 방 제거
	delRoom(sck_list[idx].user);
	// 새로운 addClient 스레드 실행
	thread th(addClient);
	// 기존 소켓 종료
	closesocket(sck_list[idx].sck);
	sck_list[idx].used = false;
	client_count--;
	cout << "▷현재 접속자 수 : " << client_count << "명" << endl;
	printClients();
	// addClient 스레드 종료 대기
	th.join();
}

// 접속한 클라이언트 목록 출력
void printClients() {
	cout << "▷현재 접속 : ";
	for (auto& client : sck_list) {
		if (client.used) cout << client.user << " ";
	}
	cout << endl;
}

// 메시지 보내기(전체)
void sendMsg(const char* msg) { 
	for (int i = 0; i < MAX_CLIENT; i++) {
		if (sck_list[i].used) {
			send(sck_list[i].sck, msg, MAX_SIZE, 0);
		}
	}
}

// 메시지 보내기(보낸사람 제외)
void sendMsgNotMe(const char* msg, int sender_idx) { 
	for (int i = 0; i < MAX_CLIENT; i++) {
		if (sck_list[i].used) {
			if (i != sender_idx) {
				send(sck_list[i].sck, msg, MAX_SIZE, 0);
			}
		}
	}
}

// 메시지 받기(index)
void recvMsg(int idx) {
	char buf[MAX_SIZE] = { };
	string msg = "";

	while (1) {
		ZeroMemory(&buf, MAX_SIZE);
		if (recv(sck_list[idx].sck, buf, MAX_SIZE, 0) > 0) {
			if (string(buf) == "/종료") {
				msg = "▶" + sck_list[idx].user + " 님이 퇴장했습니다.";
				cout << msg << '\n';
				delClient(idx);
				sendMsgNotMe(msg.c_str(), idx);
				return;
			}
			else if (string(buf) == "/생성") {
				ZeroMemory(&buf, MAX_SIZE);
				if (recv(sck_list[idx].sck, buf, MAX_SIZE, 0) > 0) {
					// 현재 방 개수가 최대치 미만인 경우 방 생성
					if (room_list.size() < room_limit) {
						ROOM_INFO new_room = { };
						new_room.sck[0] = sck_list[idx].sck;
						new_room.user[0] = sck_list[idx].user;
						new_room.room_title = buf;
						new_room.room_count = 1;
						room_list.push_back(new_room);
						sck_list[idx].room = room_list.size() - 1; // 방 번호 입력
						send(sck_list[idx].sck, "방 생성이 완료되었습니다.\n상대를 기다리는중...", MAX_SIZE, 0);
						sendRoomList();
						printRoomList();
					}
					else {
						send(sck_list[idx].sck, "방 생성이 불가합니다.", MAX_SIZE, 0);
					}
				}
				else {
					delClient(idx);
					return;
				}
			}
			else if (string(buf) == "/입장") {
				ZeroMemory(&buf, MAX_SIZE);
				if (recv(sck_list[idx].sck, buf, MAX_SIZE, 0) > 0) {
					int room_num = (buf[0] - '0') - 1;
					// 방이 존재하는 경우
					if (room_num < room_list.size()) {
						// 방에 인원이 1명인 경우
						if (room_list[room_num].room_count == 1) {
							room_list[room_num].sck[1] = sck_list[idx].sck;
							room_list[room_num].user[1] = sck_list[idx].user;
							room_list[room_num].room_count = 2;
							sck_list[idx].room = room_num; // 방 번호 입력
							printRoomList();
							// 서로에게 상대방의 닉네임을 전송
							send(room_list[room_num].sck[0], room_list[room_num].user[1].c_str(), MAX_SIZE, 0);
							send(room_list[room_num].sck[1], room_list[room_num].user[0].c_str(), MAX_SIZE, 0);
							
						}
						else {
							send(sck_list[idx].sck, "방 입장이 불가합니다.", MAX_SIZE, 0);
						}
					}
					else {
						send(sck_list[idx].sck, "방 입장이 불가합니다.", MAX_SIZE, 0);
					}
				}
				else {
					delClient(idx);
					return;
				}
			}
			else if (string(buf) == "omok_play") {
				int room_num = sck_list[idx].room;
				ZeroMemory(&buf, MAX_SIZE);
				// 게임시작
				while (1) {
					if (recv(sck_list[idx].sck, buf, MAX_SIZE, 0) > 0) {
						std::stringstream ss(buf);
						string num;
						int i = 0;
						int _pos[4] = { 0 }; // game, player, x, y;
						// 전달받은 문자열 "game|player|x|y|" 을 배열에 저장
						while (getline(ss, num, '|')) {
							_pos[i] = stoi(num);
							i++;
						}
						// 상대 플레이어에게 보낼 x, y 입력
						num = std::to_string(_pos[2]) + '|' + std::to_string(_pos[3]);
						// game 이 종료된 경우 
						if (_pos[0] == 0) {
							send(room_list[room_num].sck[0], "gameover", MAX_SIZE, 0);
							send(room_list[room_num].sck[1], "gameover", MAX_SIZE, 0);
							break;
						}
						// 다음 차례 player 에게 x, y 위치값 전송
						if (_pos[1] == 1) {
							send(room_list[room_num].sck[1], "2", MAX_SIZE, 0);
							send(room_list[room_num].sck[1], num.c_str(), MAX_SIZE, 0);
						}
						else if (_pos[1] == 2) {
							send(room_list[room_num].sck[0], "1", MAX_SIZE, 0);
							send(room_list[room_num].sck[0], num.c_str(), MAX_SIZE, 0);
						}
					}
					else {
						delClient(idx);
						return;
					}
				}
			}
			// 채팅 보내기
			else {
				pstmt = con->prepareStatement("INSERT INTO chatting(chatname, time, recv) VALUE(?, NOW(), ?)");
				pstmt->setString(1, sck_list[idx].user);
				pstmt->setString(2, string(buf));
				pstmt->execute();

				pstmt = con->prepareStatement("SELECT chatname, DATE_FORMAT(time,'%h:%i:%s'), recv FROM chatting ORDER BY time DESC LIMIT 1");
				result = pstmt->executeQuery();
				if (result->next()) {
					string chatname = result->getString(1);
					string time = result->getString(2);
					string recv = result->getString(3);

					// 현재 시간, 이름, 내용을 전송 ("[시:분:초]이름 :내용")
					msg = "[" + time + "]" + chatname + " :" + recv;

					cout << msg << endl;
					sendMsgNotMe(msg.c_str(), idx);
				}
			}
		}
		// 퇴장
		else {
			msg = sck_list[idx].user + " 님이 퇴장했습니다.";
			cout << msg << endl;
			delClient(idx);
			return;
		}
	}
}



// 방 목록 보내기
void sendRoomList() {
	string _room_list;
	for (int i = 0; i < room_list.size(); i++) {
		// 방 제목을 '|' 문자를 기준점으로 입력하여 문자열 생성
		_room_list += room_list[i].room_title + "|";
	}
	for (int i = 0; i < MAX_CLIENT; i++) {
		if (sck_list[i].used) {
			send(sck_list[i].sck, "room_list", MAX_SIZE, 0);
			send(sck_list[i].sck, _room_list.c_str(), MAX_SIZE, 0);
		}
	}
}

// 방 목록 출력
void printRoomList() {
	cout << "< room_list >" << endl;
	for (int i = 0; i < room_list.size(); i++) {
		cout << room_list[i].room_title << endl;
	}
	cout << "-------------" << endl;
}

// 방 제거
void delRoom(string _user) {
	for (int i = 0; i < room_list.size(); i++) {
		if (room_list[i].user[0] == _user) {
			room_list.erase(room_list.begin() + i);
		}
	}
	sendRoomList();
}
