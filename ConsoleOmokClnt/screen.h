void currentCursor(int* _cur_pos); // ���� Ŀ�� ��ġ Ȯ��
void gotoxy(int x, int y); // x, y ��ġ�� Ŀ�� �̵�
void printNick(string p1, string p2); // ����ȭ�� �г��� ���
void printCurPlayer(int player, int next_player); // ����ȭ�� ���� �÷��̾� ȭ��ǥ ���
void printOmok(); // ����ȭ��
void printLobby(); // �κ�ȭ��
int omok(int row, int col, int _player, int __board[15][15]); // ���� ��� Ȯ��
void OmokPlay(int* game, int _player, int _next_player, int* cur_player, int board[15][15]\
				, SOCKET _client_sock, SQL &__sql); // �����÷���
int printRoomList(SOCKET _client_sock); // �� �̸� ���
void printStartMenu(); // ����ȭ��
void printLogin(); // �α���ȭ��
void printJoin(); // ȸ������ȭ��
void printMainMenu(); // ���θ޴�ȭ��
void printMyProfile(); // ������ȭ��
void printSetting(); // ����ȭ��

