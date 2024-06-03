#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <winsock.h>
#include <WinSock2.h>

#define BUF_SIZE 100
#define READ 3
#define WRITE 5

typedef struct { // socket info
	SOCKET hClntSock; // 클라이언트 소켓 핸들
	SOCKADDR_IN clntAdr; // 클라이언트 주소
} PER_HANDLE_DATA, *LPPER_HANDLE_DATA;

typedef struct { // buffer info
	OVERLAPPED overlapped; // 비동기 IO 이벤트 시그널 확인을 위한 구조체
	WSABUF wsaBuf; // 버퍼, 버퍼길이를 담은 구조체
	char buffer[BUF_SIZE];
	int rwMode; // READ or WRITE, 완료된 입출력을 구분하기 위한 변수
} PER_IO_DATA, *LPPER_IO_DATA;

unsigned int __stdcall EchoThreadMain(LPVOID pComPort);
void ErrorHandling(const char* message);

int main(int argc, char* argv[]) {
	WSADATA wsaData; // 윈도우 소켓 라이브러리
	HANDLE hComPort; // CP 오브젝트 핸들
	SYSTEM_INFO sysInfo; // 시스템(PC) 정보
	LPPER_IO_DATA ioInfo; // IO 이벤트 핸들 정보
	LPPER_HANDLE_DATA handleInfo; // 소켓 핸들 정보
	
	SOCKET hServSock; // 서버 소켓 핸들
	SOCKADDR_IN servAdr; // 서버 주소
	int recvBytes, i, flags = 0;
	
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		ErrorHandling("WSAStartup() error!");
	
	// CP 오브젝트 생성
	hComPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	
	// 호스트 정보를 얻고, 코어 수 만큼 IO 스레드 생성
	GetSystemInfo(&sysInfo);
	for (i = 0; i < sysInfo.dwNumberOfProcessors; i++)
		_beginthreadex(NULL, 0, EchoThreadMain, (LPVOID)hComPort, 0, NULL);
	
	// 서버 주소 입력
	hServSock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	memset(&servAdr, 0, sizeof(servAdr));
	servAdr.sin_family = AF_INET;
	servAdr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAdr.sin_port = htons(atoi(argv[1]));
	
	// 서버 소켓 주소 바인딩, 클라이언트 연결 요청 대기(최대 5 대기)
	bind(hServSock, (SOCKADDR*)&servAdr, sizeof(servAdr));
	listen(hServSock, 5);
	
	// 클라이언트 소켓 생성, CP 오브젝트 연결, IO 이벤트 핸들
	while (1) {
		SOCKET hClntSock; // 클라이언트 소켓 핸들
		SOCKADDR_IN clntAdr; // 클라이언트 주소
		int addrLen = sizeof(clntAdr);
		
		// 클라이언트 연결 및 핸들 정보 입력
		hClntSock = accept(hServSock, (SOCKADDR*)&clntAdr, &addrLen);
		handleInfo = (LPPER_HANDLE_DATA)malloc(sizeof(PER_HANDLE_DATA));
		handleInfo->hClntSock = hClntSock;
		memcpy(&(handleInfo->clntAdr), &clntAdr, addrLen);
		
		// 이전에 생성한 CP 오브젝트와 클라이언트 소켓을 연결
		CreateIoCompletionPort((HANDLE)hClntSock, hComPort, (DWORD)handleInfo, 0);
		
		// IO 이벤트 핸들 초기화 및 정보 입력
		ioInfo = (LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));
		memset(&(ioInfo->overlapped), 0, sizeof(OVERLAPPED));
		ioInfo->wsaBuf.len = BUF_SIZE;
		ioInfo->wsaBuf.buf = ioInfo->buffer;
		ioInfo->rwMode = READ;
		
		// 클라이언트->서버 데이터 수신
		WSARecv(handleInfo->hClntSock, &(ioInfo->wsaBuf), 
			1, &recvBytes, &flags, &(ioInfo->overlapped), NULL);
	}
	return 0;
}

// IO 스레드
unsigned int __stdcall EchoThreadMain(LPVOID pComPort) {
	HANDLE hComPort = (HANDLE)pComPort; // completion port
	SOCKET sock; // 소켓 핸들
	DWORD bytesTrans; // 
	LPPER_HANDLE_DATA handleInfo; // 소켓 정보(소켓핸들, 소켓주소)
	LPPER_IO_DATA ioInfo; // 버퍼 핸들(이벤트시그널, 버퍼, 버퍼길이, RW 확인
	DWORD flags = 0;
	
	while (1) {
		// 입출력 완료 대기 후 handleInfo, ioInfo 의 정보를 얻음(입출력이 없으면 진행되지 않음 "INFINITE")
		GetQueuedCompletionStatus(hComPort, &bytesTrans, (LPDWORD)&handleInfo, 
			(LPOVERLAPPED*)&ioInfo, INFINITE);
		sock = handleInfo->hClntSock; // 소켓정보 입력
		
		// 입력(클라이언트->서버)
		if (ioInfo->rwMode == READ) {
			puts("message received!");
			if (bytesTrans == 0) { // EOF 전송 시
				closesocket(sock);
				free(handleInfo); free(ioInfo);
				continue;
			}
			
			// 서버가 수신한 메시지를 클라이언트에게 재전송
			memset(&(ioInfo->overlapped), 0, sizeof(OVERLAPPED));
			ioInfo->wsaBuf.len = bytesTrans;
			ioInfo->rwMode = WRITE;
			WSASend(sock, &(ioInfo->wsaBuf), 
				1, NULL, 0, &(ioInfo->overlapped), NULL);
				
			// 클라이언트가 전송하는 메시지 수신 대기
			ioInfo = (LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));
			memset(&(ioInfo->overlapped), 0, sizeof(OVERLAPPED));
			ioInfo->wsaBuf.len = BUF_SIZE;
			ioInfo->wsaBuf.buf = ioInfo->buffer;
			ioInfo->rwMode = READ;
			WSARecv(sock, &(ioInfo->wsaBuf), 
				1, NULL, &flags, &(ioInfo->overlapped), NULL);
		}
		else {
			puts("message sent!");
			free(ioInfo);
		}
	}
	return 0;
}

void ErrorHandling(char *message) {
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}