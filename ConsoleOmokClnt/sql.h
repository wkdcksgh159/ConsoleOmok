// Mysql DB서버에 접근하기 위한 SQL 클래스 선언
class SQL
{
private:
	// Table 데이터 컬럼
	string id, pw, status;
	int win;

	// Mysql 클래스 선언(기본 생성자 실행)
	sql::mysql::MySQL_Driver* driver;
	sql::Connection* con;
	sql::PreparedStatement* pstmt;
	sql::ResultSet* result;
	sql::Statement* stmt;

	// Mysql 서버 주소, 계정, 비밀번호
	const string server = "tcp://59.8.177.61:3306";
	const string username = "guest";
	const string password = "guest";

public:
	SQL(); // SQL 기본 생성자
	~SQL(); // SQL 소멸자
	string inputPw(); // 비밀번호 입력
	int login(); // 로그인
	void join(); // 회원가입
	void printProfile(sql::ResultSet* _result); // 유저 정보 출력
	void myProfile(); // 내 정보
	void updateStatus(); // 상태메시지 수정
	void modifyPw(); // 비밀번호 변경
	int deleteUser(); // 회원 탈퇴
	string getId(); // 아이디 반환
	void printChat(sql::ResultSet* _result); // 채팅을 출력
	void beforeChat(); // 이전 채팅을 출력
	int matchId(string _id); // 상대 아이디를 확인
	void updateWin(); // 승리 수 변경(증가)
};