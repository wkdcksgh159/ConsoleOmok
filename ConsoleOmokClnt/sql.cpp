#pragma once
#include "stdafx.h"
#include "sql.h"

// SQL 기본 생성자
SQL::SQL() 
{
	try {
		driver = sql::mysql::get_driver_instance();
		con = driver->connect(server, username, password);
		con->setSchema("omok"); // omok DB
	}
	catch (sql::SQLException& e) {
		cout << "Could not connect to server. Error message: " << e.what() << endl;
		exit(1);
	}

	stmt = con->createStatement();
	stmt->execute("set names euckr"); // 쿼리 결과값 인코딩셋을 euckr 로 설정
	if (stmt) { delete stmt; stmt = nullptr; }
}

// SQL 소멸자
SQL::~SQL()
{
	delete result;
	delete pstmt;
	delete con;
	delete stmt;
}

// 비밀번호 입력
string SQL::inputPw() 
{
	string i_pw;
	char input = ' ';
	while (1) {
		input = _getch(); // 키보드 입력값 반환
		if (input == ENTER) break;
		if (input == BACKSPACE) { // BACKSPACE 입력시
			if (!i_pw.empty()) {
				i_pw.pop_back(); // pw 에 입력된 마지막 값을 지우고
				cout << "\b \b"; // 출력된 '*' 를 지움
			}
		}
		else {
			i_pw.push_back(input);
			cout << '*';
		}
	}
	cout << '\n';
	return i_pw;
}

// 로그인
int SQL::login() 
{
	cout << ">>아이디 : ";
	cin >> id;
	if (id == "0") return 0;
	cout << ">>비밀번호 : ";
	pw = inputPw();
	if (pw == "0") return 0;
	cout << endl;

	pstmt = con->prepareStatement("SELECT id, pw FROM user WHERE id=?");
	pstmt->setString(1, id);
	result = pstmt->executeQuery();

	if (result->next()) {
		string db_id = result->getString(1);
		string db_pw = result->getString(2);

		if (db_id == id && db_pw == pw) {
			cout << "\n▶로그인 중입니다. 잠시만 기다려주세요.\n";
			cout << "▶" << id << "님 환영합니다." << endl;
			return 1;
		}
		else cout << "▶해당하는 정보가 없습니다." << endl;
		pw.clear();
	}
	return 0;
}

// 회원가입
void SQL::join() 
{
	while (1)
	{
		cout << ">>아이디 : ";
		cin >> id;
		if (id == "0") return;
		pstmt = con->prepareStatement("SELECT id FROM user WHERE id=?");
		pstmt->setString(1, id);
		result = pstmt->executeQuery();

		if (result->next())
		{
			string db_id = result->getString(1);
			if (db_id == id) {
				cout << "▶중복된 아이디가 있습니다." << endl;
				continue;
			}
		}
		else {
			cout << "▶중복체크 완료." << endl;
			break;
		}
	}
	while (1)
	{
		cout << ">>비밀번호를 입력해주세요 : ";
		pw = inputPw();
		if (pw == "0") return;
		cout << ">>다시 한번 입력해주세요 : ";
		string verify_pw = inputPw();
		if (verify_pw == "0") return;
		if (pw == verify_pw) {
			break;
		}
		else {
			cout << "▶비밀번호가 다릅니다." << endl;
			pw.clear();
		}
	}

	pstmt = con->prepareStatement("INSERT INTO user(id, pw) VALUE(?, ?)");
	pstmt->setString(1, id);
	pstmt->setString(2, pw);
	pstmt->execute();
	cout << "▶회원가입이 완료되었습니다.\n";
	pw.clear();
	Sleep(500); // INSERT 대기
}

// 유저 정보 출력
void SQL::printProfile(sql::ResultSet* _result) 
{
	cout << "-------------------------------------" << endl;
	while (_result->next()) {
		cout << "▷이름 : " << _result->getString("id") << endl;
		cout << "▷상태 메시지 : " << _result->getString("status") << endl;
		cout << "▷승리 : " << _result->getInt("win") << endl;
		cout << "-------------------------------------" << endl;
	}
}

// 내 정보
void SQL::myProfile() 
{
	pstmt = con->prepareStatement("SELECT id, status, win FROM user WHERE id = ?");
	pstmt->setString(1, id);
	result = pstmt->executeQuery();
	printProfile(result);
}

// 상태메시지 수정
void SQL::updateStatus() 
{
	cout << ">>상태메시지 입력 : ";
	// 마지막 입력의 개행 문자를 제거
	cin.ignore();
	getline(cin, status);
	
	pstmt = con->prepareStatement("UPDATE user SET status = ? WHERE id = ?");
	pstmt->setString(1, status);
	pstmt->setString(2, id);
	pstmt->executeQuery();
	printf("▶상태메시지 수정이 완료되었습니다.\n");
}

// 비밀번호 변경
void SQL::modifyPw() 
{
	cout << ">>기존 비밀번호를 입력해주세요. : ";
	string existPw = inputPw();
	cout << '\n';

	pstmt = con->prepareStatement("SELECT pw FROM user WHERE id=?");
	pstmt->setString(1, id);
	result = pstmt->executeQuery();

	if (existPw == result->getString("pw")) {
		while (1) {
			cout << ">>새로운 비밀번호를 입력해주세요 : ";
			pw = inputPw();
			cout << ">>다시 한번 입력해주세요 : ";
			string verifyPw = inputPw();
			
			if (pw == verifyPw) {
				pstmt = con->prepareStatement("UPDATE user SET pw = ? WHERE id = ?");
				pstmt->setString(1, pw);
				pstmt->setString(2, id);
				pstmt->executeQuery();
				printf("▶새로운 비밀번호로 변경되었습니다.\n");
				break;
			}
			else cout << "▶비밀번호가 다릅니다.\n";
		}
	}
	else cout << "▶비밀번호가 다릅니다.\n";
}

// 회원 탈퇴
int SQL::deleteUser() 
{
	cout << ">>기존 비밀번호를 입력해주세요 : ";
	string existPw = inputPw();

	pstmt = con->prepareStatement("SELECT pw FROM user WHERE id=?");
	pstmt->setString(1, id);
	result = pstmt->executeQuery();

	if (existPw == result->getString("pw")) {
		cout << "정말 삭제하시겠습니까? 삭제한 이후엔 되돌릴 수 없습니다. 1. 계속하기, 2. 그만두기 : ";
		char lastCheck;
		cin >> lastCheck;
		if (lastCheck == '1') {
			pstmt = con->prepareStatement("DELETE FROM user WHERE id = ?");
			pstmt->setString(1, id);
			result = pstmt->executeQuery();
			cout << "▶그동안 감사했습니다. 또 이용해주세요.\n";
			return 1;
		}
		else if (lastCheck == '2') {
			cout << "잘 생각하셨습니다..\n";
		}
		else cout << "▶잘못된 입력입니다." << endl;
	}
	else cout << "▶비밀번호가 다릅니다.\n";
}

// 아이디 반환
string SQL::getId() 
{
	return id;
}

// 채팅을 출력
void SQL::printChat(sql::ResultSet* _result) 
{
	while (_result->next()) {
		cout << "[" << _result->getString("time") << "]" << 
		_result->getString("chatname") << ": " << _result->getString("recv") << '\n';
	}
}

// 이전 채팅을 불러옴
void SQL::beforeChat() 
{
	pstmt = con->prepareStatement("SELECT DATE_FORMAT(time, '%H:%i:%s'), chatname, recv FROM chatting ORDER BY time DESC LIMIT 5");
	result = pstmt->executeQuery();

	printChat(result);
}

// 매개변수로 전달된 id 가 DB에 존재하는지 확인
int SQL::matchId(string _id)
{
	pstmt = con->prepareStatement("SELECT id FROM user WHERE id=?");
	pstmt->setString(1, _id);
	result = pstmt->executeQuery();
	if (result->next())
	{
		string db_id = result->getString(1);
		if (db_id == _id) {
			return 1;
		}
	}
	return 0;
}

// 승리 수 변경(증가)
void SQL::updateWin() 
{
	pstmt = con->prepareStatement("SELECT win FROM user WHERE id=?");
	pstmt->setString(1, id);
	result = pstmt->executeQuery();
	if (result->next())
	{
		win = result->getInt(1);
		int temp = win + 1;
		pstmt = con->prepareStatement("UPDATE user SET win = ? WHERE id = ?");
		pstmt->setInt(1, temp);
		pstmt->setString(2, id);
		pstmt->executeQuery();
	}
}