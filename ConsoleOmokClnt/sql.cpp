#pragma once
#include "stdafx.h"
#include "sql.h"

// SQL �⺻ ������
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
	stmt->execute("set names euckr"); // ���� ����� ���ڵ����� euckr �� ����
	if (stmt) { delete stmt; stmt = nullptr; }
}

// SQL �Ҹ���
SQL::~SQL()
{
	delete result;
	delete pstmt;
	delete con;
	delete stmt;
}

// ��й�ȣ �Է�
string SQL::inputPw() 
{
	string i_pw;
	char input = ' ';
	while (1) {
		input = _getch(); // Ű���� �Է°� ��ȯ
		if (input == ENTER) break;
		if (input == BACKSPACE) { // BACKSPACE �Է½�
			if (!i_pw.empty()) {
				i_pw.pop_back(); // pw �� �Էµ� ������ ���� �����
				cout << "\b \b"; // ��µ� '*' �� ����
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

// �α���
int SQL::login() 
{
	cout << ">>���̵� : ";
	cin >> id;
	if (id == "0") return 0;
	cout << ">>��й�ȣ : ";
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
			cout << "\n���α��� ���Դϴ�. ��ø� ��ٷ��ּ���.\n";
			cout << "��" << id << "�� ȯ���մϴ�." << endl;
			return 1;
		}
		else cout << "���ش��ϴ� ������ �����ϴ�." << endl;
		pw.clear();
	}
	return 0;
}

// ȸ������
void SQL::join() 
{
	while (1)
	{
		cout << ">>���̵� : ";
		cin >> id;
		if (id == "0") return;
		pstmt = con->prepareStatement("SELECT id FROM user WHERE id=?");
		pstmt->setString(1, id);
		result = pstmt->executeQuery();

		if (result->next())
		{
			string db_id = result->getString(1);
			if (db_id == id) {
				cout << "���ߺ��� ���̵� �ֽ��ϴ�." << endl;
				continue;
			}
		}
		else {
			cout << "���ߺ�üũ �Ϸ�." << endl;
			break;
		}
	}
	while (1)
	{
		cout << ">>��й�ȣ�� �Է����ּ��� : ";
		pw = inputPw();
		if (pw == "0") return;
		cout << ">>�ٽ� �ѹ� �Է����ּ��� : ";
		string verify_pw = inputPw();
		if (verify_pw == "0") return;
		if (pw == verify_pw) {
			break;
		}
		else {
			cout << "����й�ȣ�� �ٸ��ϴ�." << endl;
			pw.clear();
		}
	}

	pstmt = con->prepareStatement("INSERT INTO user(id, pw) VALUE(?, ?)");
	pstmt->setString(1, id);
	pstmt->setString(2, pw);
	pstmt->execute();
	cout << "��ȸ�������� �Ϸ�Ǿ����ϴ�.\n";
	pw.clear();
	Sleep(500); // INSERT ���
}

// ���� ���� ���
void SQL::printProfile(sql::ResultSet* _result) 
{
	cout << "-------------------------------------" << endl;
	while (_result->next()) {
		cout << "���̸� : " << _result->getString("id") << endl;
		cout << "������ �޽��� : " << _result->getString("status") << endl;
		cout << "���¸� : " << _result->getInt("win") << endl;
		cout << "-------------------------------------" << endl;
	}
}

// �� ����
void SQL::myProfile() 
{
	pstmt = con->prepareStatement("SELECT id, status, win FROM user WHERE id = ?");
	pstmt->setString(1, id);
	result = pstmt->executeQuery();
	printProfile(result);
}

// ���¸޽��� ����
void SQL::updateStatus() 
{
	cout << ">>���¸޽��� �Է� : ";
	// ������ �Է��� ���� ���ڸ� ����
	cin.ignore();
	getline(cin, status);
	
	pstmt = con->prepareStatement("UPDATE user SET status = ? WHERE id = ?");
	pstmt->setString(1, status);
	pstmt->setString(2, id);
	pstmt->executeQuery();
	printf("�����¸޽��� ������ �Ϸ�Ǿ����ϴ�.\n");
}

// ��й�ȣ ����
void SQL::modifyPw() 
{
	cout << ">>���� ��й�ȣ�� �Է����ּ���. : ";
	string existPw = inputPw();
	cout << '\n';

	pstmt = con->prepareStatement("SELECT pw FROM user WHERE id=?");
	pstmt->setString(1, id);
	result = pstmt->executeQuery();

	if (existPw == result->getString("pw")) {
		while (1) {
			cout << ">>���ο� ��й�ȣ�� �Է����ּ��� : ";
			pw = inputPw();
			cout << ">>�ٽ� �ѹ� �Է����ּ��� : ";
			string verifyPw = inputPw();
			
			if (pw == verifyPw) {
				pstmt = con->prepareStatement("UPDATE user SET pw = ? WHERE id = ?");
				pstmt->setString(1, pw);
				pstmt->setString(2, id);
				pstmt->executeQuery();
				printf("�����ο� ��й�ȣ�� ����Ǿ����ϴ�.\n");
				break;
			}
			else cout << "����й�ȣ�� �ٸ��ϴ�.\n";
		}
	}
	else cout << "����й�ȣ�� �ٸ��ϴ�.\n";
}

// ȸ�� Ż��
int SQL::deleteUser() 
{
	cout << ">>���� ��й�ȣ�� �Է����ּ��� : ";
	string existPw = inputPw();

	pstmt = con->prepareStatement("SELECT pw FROM user WHERE id=?");
	pstmt->setString(1, id);
	result = pstmt->executeQuery();

	if (existPw == result->getString("pw")) {
		cout << "���� �����Ͻðڽ��ϱ�? ������ ���Ŀ� �ǵ��� �� �����ϴ�. 1. ����ϱ�, 2. �׸��α� : ";
		char lastCheck;
		cin >> lastCheck;
		if (lastCheck == '1') {
			pstmt = con->prepareStatement("DELETE FROM user WHERE id = ?");
			pstmt->setString(1, id);
			result = pstmt->executeQuery();
			cout << "���׵��� �����߽��ϴ�. �� �̿����ּ���.\n";
			return 1;
		}
		else if (lastCheck == '2') {
			cout << "�� �����ϼ̽��ϴ�..\n";
		}
		else cout << "���߸��� �Է��Դϴ�." << endl;
	}
	else cout << "����й�ȣ�� �ٸ��ϴ�.\n";
}

// ���̵� ��ȯ
string SQL::getId() 
{
	return id;
}

// ä���� ���
void SQL::printChat(sql::ResultSet* _result) 
{
	while (_result->next()) {
		cout << "[" << _result->getString("time") << "]" << 
		_result->getString("chatname") << ": " << _result->getString("recv") << '\n';
	}
}

// ���� ä���� �ҷ���
void SQL::beforeChat() 
{
	pstmt = con->prepareStatement("SELECT DATE_FORMAT(time, '%H:%i:%s'), chatname, recv FROM chatting ORDER BY time DESC LIMIT 5");
	result = pstmt->executeQuery();

	printChat(result);
}

// �Ű������� ���޵� id �� DB�� �����ϴ��� Ȯ��
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

// �¸� �� ����(����)
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