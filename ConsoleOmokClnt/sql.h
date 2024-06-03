// Mysql DB������ �����ϱ� ���� SQL Ŭ���� ����
class SQL
{
private:
	// Table ������ �÷�
	string id, pw, status;
	int win;

	// Mysql Ŭ���� ����(�⺻ ������ ����)
	sql::mysql::MySQL_Driver* driver;
	sql::Connection* con;
	sql::PreparedStatement* pstmt;
	sql::ResultSet* result;
	sql::Statement* stmt;

	// Mysql ���� �ּ�, ����, ��й�ȣ
	const string server = "tcp://59.8.177.61:3306";
	const string username = "guest";
	const string password = "guest";

public:
	SQL(); // SQL �⺻ ������
	~SQL(); // SQL �Ҹ���
	string inputPw(); // ��й�ȣ �Է�
	int login(); // �α���
	void join(); // ȸ������
	void printProfile(sql::ResultSet* _result); // ���� ���� ���
	void myProfile(); // �� ����
	void updateStatus(); // ���¸޽��� ����
	void modifyPw(); // ��й�ȣ ����
	int deleteUser(); // ȸ�� Ż��
	string getId(); // ���̵� ��ȯ
	void printChat(sql::ResultSet* _result); // ä���� ���
	void beforeChat(); // ���� ä���� ���
	int matchId(string _id); // ��� ���̵� Ȯ��
	void updateWin(); // �¸� �� ����(����)
};