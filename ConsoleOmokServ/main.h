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

#define MAX_SIZE 1024 // 버퍼 사이즈 선언
#define MAX_CLIENT 20 // 최대 인원
#define SERVER_PORT 7777 // 포트 번호

const string server = "tcp://127.0.0.1:3306"; // 데이터베이스 주소
const string username = "root"; // 데이터베이스 사용자
const string password = "root"; // 데이터베이스 접속 비밀번호

// 클라이언트 소켓, 이름, 입장한 방 번호, 사용 여부
struct SOCKET_INFO {
    SOCKET sck = 0;
    string user = "";
    int room = -1;
    bool used = false;
};

// 소켓관리
SOCKET_INFO sck_list[MAX_CLIENT];
SOCKET_INFO server_sock;
int client_count = 0;

// 방 소켓, 유저 이름, 방 이름, 방 인원
struct ROOM_INFO {
    SOCKET sck[2] = { 0, };
    string user[2] = { "", "" };
    string room_title = { 0, };
    int room_count = 0;
};

// 방관리
vector<ROOM_INFO> room_list;
int room_limit = 9;

// SQL
sql::mysql::MySQL_Driver* driver;
sql::Connection* con;
sql::PreparedStatement* pstmt;
sql::ResultSet* result;
sql::Statement* stmt;
void startSql(); // 데이터베이스 연결

// 시스템구동
void printMain(); // 메인화면

// 소켓
void serverInit(); // 서버 소켓 초기화
void addClient(); // 클라이언트 추가
void delClient(int idx); // 클라이언트 제거
void printClients(); // 접속한 클라이언트 목록 출력

// IO
void sendMsg(const char* msg); // 메시지 보내기
void sendMsgNotMe(const char* msg, int sender_idx); // 메시지 보내기(자신 제외)
void recvMsg(int idx); // 메시지 받기
void sendRoomList(); // 방 목록 보내기
void printRoomList(); // 방 목록 출력
void delRoom(string user); // 방 제거