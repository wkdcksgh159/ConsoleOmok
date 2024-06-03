void currentCursor(int* _cur_pos); // 현재 커서 위치 확인
void gotoxy(int x, int y); // x, y 위치로 커서 이동
void printNick(string p1, string p2); // 오목화면 닉네임 출력
void printCurPlayer(int player, int next_player); // 오목화면 현재 플레이어 화살표 출력
void printOmok(); // 오목화면
void printLobby(); // 로비화면
int omok(int row, int col, int _player, int __board[15][15]); // 오목 결과 확인
void OmokPlay(int* game, int _player, int _next_player, int* cur_player, int board[15][15]\
				, SOCKET _client_sock, SQL &__sql); // 오목플레이
int printRoomList(SOCKET _client_sock); // 방 이름 출력
void printStartMenu(); // 시작화면
void printLogin(); // 로그인화면
void printJoin(); // 회원가입화면
void printMainMenu(); // 메인메뉴화면
void printMyProfile(); // 내정보화면
void printSetting(); // 설정화면

