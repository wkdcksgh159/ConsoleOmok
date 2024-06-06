#pragma once
#pragma comment(lib, "ws2_32.lib")
//#pragma comment(lib, "libmysql.lib")
//#pragma comment(lib, "mysqlcppconn.lib")

#include <WinSock2.h>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <mysql/jdbc.h>

using std::cout;
using std::cin;
using std::endl;
using std::string;
using std::vector;
using std::thread;
using std::getline;

#define MAX_SIZE 1024 // ���� ������ ����
#define MAX_CLIENT 20 // �ִ� �ο�
#define SERVER_PORT 7777 // ��Ʈ ��ȣ

const string server = "tcp://127.0.0.1:3306"; // �����ͺ��̽� �ּ�
const string username = "root"; // �����ͺ��̽� �����
const string password = "root"; // �����ͺ��̽� ���� ��й�ȣ

// Ŭ���̾�Ʈ ����, �̸�, ������ �� ��ȣ, ��� ����
struct SOCKET_INFO {
    SOCKET sck = 0;
    string user = "";
    int room = -1;
    bool used = false;
};

// ���ϰ���
SOCKET_INFO sck_list[MAX_CLIENT];
SOCKET_INFO server_sock;
int client_count = 0;

// �� ����, ���� �̸�, �� �̸�, �� �ο�
struct ROOM_INFO {
    SOCKET sck[2] = { 0, };
    string user[2] = { "", "" };
    string room_title = { 0, };
    int room_count = 0;
};

// �����
vector<ROOM_INFO> room_list;
int room_limit = 9;

// SQL
sql::mysql::MySQL_Driver* driver;
sql::Connection* con;
sql::PreparedStatement* pstmt;
sql::ResultSet* result;
sql::Statement* stmt;
void startSql(); // �����ͺ��̽� ����

// �ý��۱���
void printMain(); // ����ȭ��

// ����
void serverInit(); // ���� ���� �ʱ�ȭ
void addClient(); // Ŭ���̾�Ʈ �߰�
void delClient(int idx); // Ŭ���̾�Ʈ ����
void printClients(); // ������ Ŭ���̾�Ʈ ��� ���

// IO
void sendMsg(const char* msg); // �޽��� ������
void sendMsgNotMe(const char* msg, int sender_idx); // �޽��� ������(�ڽ� ����)
void recvMsg(int idx); // �޽��� �ޱ�
void sendRoomList(); // �� ��� ������
void printRoomList(); // �� ��� ���
void delRoom(string user); // �� ����