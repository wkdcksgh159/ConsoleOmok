#include "main.h"
int main()
{
	system("color 06");

	// �����ͺ��̽� ����
	startSql();
	// ����ȭ�� ���
	printMain();

	WSADATA wsa;

	int code = WSAStartup(MAKEWORD(2, 2), &wsa);
	if (!code) {
		// ���� ���� �ʱ�ȭ
		serverInit();
		thread th1[MAX_CLIENT];

		// Ŭ���̾�Ʈ ���� ��û ���
		for (int i = 0; i < MAX_CLIENT; i++) {
			th1[i] = thread(addClient);
		}

		// ����� Ŭ���̾�Ʈ���� �޽��� ����
		while (1) {
			string text, msg = "";
			getline(cin, text);
			const char* buf = text.c_str();
			msg = server_sock.user + " : " + buf;
			sendMsg(msg.c_str());
		}

		// ������ ���� ���
		for (int i = 0; i < MAX_CLIENT; i++) {
			th1[i].join();
		}

		closesocket(server_sock.sck);
	}
	else cout << "���α׷� ����. (Error code : " << code << ")";

	WSACleanup();

	delete result;
	delete pstmt;
	delete con;
	delete stmt;

	return 0;
}

// �����ͺ��̽� ����
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
	stmt->execute("set names euckr"); // euckr ���ڵ�
	if (stmt) { delete stmt; stmt = nullptr; }
}

// ����ȭ��
void printMain() { 
	system("cls");
	cout << " ----------------------------------- \n";
	cout << "|              [OMOK]               |\n";
	cout << "|                                   |\n";
	cout << "|           < SERVER ON >           |\n";
	cout << "|                                   |\n";
	cout << " ----------------------------------- \n";
}

// ���� ���� �ʱ�ȭ
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

// Ŭ���̾�Ʈ �߰�
void addClient() { 
	SOCKADDR_IN addr = {};
	int addrsize = sizeof(addr);
	char buf[MAX_SIZE] = { };
	ZeroMemory(&addr, addrsize);

	SOCKET_INFO new_client = {};
	new_client.sck = accept(server_sock.sck, (sockaddr*)&addr, &addrsize);

	// Ŭ���̾�Ʈ ���� �̸� ���޹ޱ�
	recv(new_client.sck, buf, MAX_SIZE, 0);
	new_client.user = string(buf);
	string msg = "��" + new_client.user + " ���� �����߽��ϴ�.";
	cout << msg << "\n";

	int i;
	// ���� ��� �߰�
	for (i = 0; i < MAX_CLIENT; i++) {
		if (!sck_list[i].used) { // ��� ���� Ȯ��
			sck_list[i] = new_client;
			sck_list[i].used = true;
			break;
		}
	}
	printClients();

	// recvMsg ������ �߰�
	thread th(recvMsg, i);
	th.detach();

	client_count++;
	cout << "������ ������ �� : " << client_count << "��" << endl;

	// ���� Ȯ��, �� ��� �۽�
	sendMsg(msg.c_str());
	sendRoomList();
}

// Ŭ���̾�Ʈ ����
void delClient(int idx) {
	// �ش� ������ ���� �� ����
	delRoom(sck_list[idx].user);
	// ���ο� addClient ������ ����
	thread th(addClient);
	// ���� ���� ����
	closesocket(sck_list[idx].sck);
	sck_list[idx].used = false;
	client_count--;
	cout << "������ ������ �� : " << client_count << "��" << endl;
	printClients();
	// addClient ������ ���� ���
	th.join();
}

// ������ Ŭ���̾�Ʈ ��� ���
void printClients() {
	cout << "������ ���� : ";
	for (auto& client : sck_list) {
		if (client.used) cout << client.user << " ";
	}
	cout << endl;
}

// �޽��� ������(��ü)
void sendMsg(const char* msg) { 
	for (int i = 0; i < MAX_CLIENT; i++) {
		if (sck_list[i].used) {
			send(sck_list[i].sck, msg, MAX_SIZE, 0);
		}
	}
}

// �޽��� ������(������� ����)
void sendMsgNotMe(const char* msg, int sender_idx) { 
	for (int i = 0; i < MAX_CLIENT; i++) {
		if (sck_list[i].used) {
			if (i != sender_idx) {
				send(sck_list[i].sck, msg, MAX_SIZE, 0);
			}
		}
	}
}

// �޽��� �ޱ�(index)
void recvMsg(int idx) {
	char buf[MAX_SIZE] = { };
	string msg = "";

	while (1) {
		ZeroMemory(&buf, MAX_SIZE);
		if (recv(sck_list[idx].sck, buf, MAX_SIZE, 0) > 0) {
			if (string(buf) == "/����") {
				msg = "��" + sck_list[idx].user + " ���� �����߽��ϴ�.";
				cout << msg << '\n';
				delClient(idx);
				sendMsgNotMe(msg.c_str(), idx);
				return;
			}
			else if (string(buf) == "/����") {
				ZeroMemory(&buf, MAX_SIZE);
				if (recv(sck_list[idx].sck, buf, MAX_SIZE, 0) > 0) {
					// ���� �� ������ �ִ�ġ �̸��� ��� �� ����
					if (room_list.size() < room_limit) {
						ROOM_INFO new_room = { };
						new_room.sck[0] = sck_list[idx].sck;
						new_room.user[0] = sck_list[idx].user;
						new_room.room_title = buf;
						new_room.room_count = 1;
						room_list.push_back(new_room);
						sck_list[idx].room = room_list.size() - 1; // �� ��ȣ �Է�
						send(sck_list[idx].sck, "�� ������ �Ϸ�Ǿ����ϴ�.\n��븦 ��ٸ�����...", MAX_SIZE, 0);
						sendRoomList();
						printRoomList();
					}
					else {
						send(sck_list[idx].sck, "�� ������ �Ұ��մϴ�.", MAX_SIZE, 0);
					}
				}
				else {
					delClient(idx);
					return;
				}
			}
			else if (string(buf) == "/����") {
				ZeroMemory(&buf, MAX_SIZE);
				if (recv(sck_list[idx].sck, buf, MAX_SIZE, 0) > 0) {
					int room_num = (buf[0] - '0') - 1;
					// ���� �����ϴ� ���
					if (room_num < room_list.size()) {
						// �濡 �ο��� 1���� ���
						if (room_list[room_num].room_count == 1) {
							room_list[room_num].sck[1] = sck_list[idx].sck;
							room_list[room_num].user[1] = sck_list[idx].user;
							room_list[room_num].room_count = 2;
							sck_list[idx].room = room_num; // �� ��ȣ �Է�
							printRoomList();
							// ���ο��� ������ �г����� ����
							send(room_list[room_num].sck[0], room_list[room_num].user[1].c_str(), MAX_SIZE, 0);
							send(room_list[room_num].sck[1], room_list[room_num].user[0].c_str(), MAX_SIZE, 0);
							
						}
						else {
							send(sck_list[idx].sck, "�� ������ �Ұ��մϴ�.", MAX_SIZE, 0);
						}
					}
					else {
						send(sck_list[idx].sck, "�� ������ �Ұ��մϴ�.", MAX_SIZE, 0);
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
				// ���ӽ���
				while (1) {
					if (recv(sck_list[idx].sck, buf, MAX_SIZE, 0) > 0) {
						std::stringstream ss(buf);
						string num;
						int i = 0;
						int _pos[4] = { 0 }; // game, player, x, y;
						// ���޹��� ���ڿ� "game|player|x|y|" �� �迭�� ����
						while (getline(ss, num, '|')) {
							_pos[i] = stoi(num);
							i++;
						}
						// ��� �÷��̾�� ���� x, y �Է�
						num = std::to_string(_pos[2]) + '|' + std::to_string(_pos[3]);
						// game �� ����� ��� 
						if (_pos[0] == 0) {
							send(room_list[room_num].sck[0], "gameover", MAX_SIZE, 0);
							send(room_list[room_num].sck[1], "gameover", MAX_SIZE, 0);
							break;
						}
						// ���� ���� player ���� x, y ��ġ�� ����
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
			// ä�� ������
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

					// ���� �ð�, �̸�, ������ ���� ("[��:��:��]�̸� :����")
					msg = "[" + time + "]" + chatname + " :" + recv;

					cout << msg << endl;
					sendMsgNotMe(msg.c_str(), idx);
				}
			}
		}
		// ����
		else {
			msg = sck_list[idx].user + " ���� �����߽��ϴ�.";
			cout << msg << endl;
			delClient(idx);
			return;
		}
	}
}



// �� ��� ������
void sendRoomList() {
	string _room_list;
	for (int i = 0; i < room_list.size(); i++) {
		// �� ������ '|' ���ڸ� ���������� �Է��Ͽ� ���ڿ� ����
		_room_list += room_list[i].room_title + "|";
	}
	for (int i = 0; i < MAX_CLIENT; i++) {
		if (sck_list[i].used) {
			send(sck_list[i].sck, "room_list", MAX_SIZE, 0);
			send(sck_list[i].sck, _room_list.c_str(), MAX_SIZE, 0);
		}
	}
}

// �� ��� ���
void printRoomList() {
	cout << "< room_list >" << endl;
	for (int i = 0; i < room_list.size(); i++) {
		cout << room_list[i].room_title << endl;
	}
	cout << "-------------" << endl;
}

// �� ����
void delRoom(string _user) {
	for (int i = 0; i < room_list.size(); i++) {
		if (room_list[i].user[0] == _user) {
			room_list.erase(room_list.begin() + i);
		}
	}
	sendRoomList();
}
