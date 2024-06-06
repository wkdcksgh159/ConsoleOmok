#pragma once
#pragma comment(lib, "ws2_32.lib")
//#pragma comment(lib, "mysqlcppconn.lib")

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>
#include <sstream>
#include <iostream>
#include <thread>
#include <mysql/jdbc.h>
#include <conio.h>
#include <vector>

using std::cout;
using std::cin;
using std::endl;
using std::string;
using std::vector;
using std::thread;

#define SERVER_IP "59.8.177.61"
#define SERVER_PORT 7777
#define MAX_SIZE 1024 // 버퍼 사이즈 지정

#define ENTER 13
#define BACKSPACE 8
#define UP 72
#define DOWN 80
#define LEFT 75
#define RIGHT 77
#define SPACE 32