#include "dbUtils.h"

// MySQL과의 연결을 생성한다.

// 통신을 위해 사용하게 될 conn
sql::Connection* mysql_conn = nullptr;
sw::redis::Redis* redis_conn = nullptr;

void mysql::makeConnection() {
	try {
		sql::mysql::MySQL_Driver* driver;

		// 드라이버 초기화
		driver = sql::mysql::get_mysql_driver_instance(); 
		// 드라이버를 통해 특정 주소, 포트에 연결한다.
		mysql_conn = driver->connect("tcp://" + string(MYSQL_ADDRESS) + ":" + to_string(MYSQL_PORT), "root", "1372");
		 
		// 채팅 서버 database 설정
		mysql_conn->setSchema("chatserver");
	}

	catch (sql::SQLException& e) {
		cout << "SQLException : " << e.what() << endl;
		cout << "MySQL Error Code : " << e.getErrorCode() << endl;
		cout << "SQLState" << e.getSQLState() << endl;
	}
}

// MySQL에서 ID를 사용해 유저 정보 가져오기
userInfo mysql::mysql::getUserinfoByID(string id) {
	try {
		sql::PreparedStatement* pstmt;
		sql::ResultSet* result;

		string query = 
			"SELECT id, pwd, nickname "
			"FROM userinfo "
			"WHERE id = ?";

		// id기반으로 유저 찾기
		pstmt = mysql_conn->prepareStatement(query);
		pstmt->setString(1, id);

		result = pstmt->executeQuery();


		// tmp라는 구조체 만들어 데이터 할당하기
		result->next();
		userInfo tmp(result->getString("id"), result->getString("pwd"), result->getString("nickname"));

		// 리소스 정리
		delete pstmt;
		delete result;

		return tmp;
	}

	catch (sql::SQLException &e) {
		cout << "SQLException : " << e.what() << endl;
		cout << "MySQL Error Code : " << e.getErrorCode() << endl;
		cout << "SQLState" << e.getSQLState() << endl;
	}
}

// MySQL에서 Nickname을 사용해 유저 정보를 가져온다
userInfo mysql::mysql::getUserinfoByNickname(string nickname) {
	try {
		sql::PreparedStatement* pstmt;
		sql::ResultSet* result;

		string query =
			"SELECT id, pwd, nickname "
			"FROM userinfo "
			"WHERE nickname = ?";

		// 닉네임 기반으로 유저 찾기
		pstmt = mysql_conn->prepareStatement(query);
		pstmt->setString(1, nickname);
		result = pstmt->executeQuery();

		// tmp라는 구조체 만들어 데이터 할당하기
		userInfo tmp(result->getString("id"), result->getString("pwd"), result->getString("nickname"));

		// 리소스 정리
		delete pstmt;
		delete result;

		return tmp;
	}

	catch (sql::SQLException &e) {
		cout << "SQLException : " << e.what() << endl;
		cout << "MySQL Error Code : " << e.getErrorCode() << endl;
		cout << "SQLState" << e.getSQLState() << endl;
	}
}

// 새 유저 정보를 등록한다!
void mysql::registNewUser(string id, string pwd) {
	try {
		sql::PreparedStatement* pstmt;
		sql::ResultSet* result;

		string query = "INSERT INTO userinfo (id, pwd) VALUES (?, ?)";
		pstmt = mysql_conn->prepareStatement(query);
		pstmt->setString(1, id);
		pstmt->setString(2, pwd);

		result = pstmt->executeQuery();

		delete pstmt;
		delete result;
	}

	catch (sql::SQLException& e) {
		cout << "SQLException : " << e.what() << endl;
		cout << "MySQL Error Code : " << e.getErrorCode() << endl;
		cout << "SQLState" << e.getSQLState() << endl;
	}
}

// 회원가입 전 id 중복체크를 수행한다.
bool mysql::isDuplicateID(string id) {
	try {
		sql::PreparedStatement* pstmt;
		sql::ResultSet* result;

		string query =
			"SELECT COUNT(*)"
			"FROM userinfo"
			"WHERE id = ?";
		pstmt = mysql_conn->prepareStatement(query);
		pstmt->setString(1, id);

		result = pstmt->executeQuery();
		result->next();

		if (result->getInt(1) == 0)
			return false;
		else
			return true;
	}

	catch (sql::SQLException& e) {
		cout << "SQLException : " << e.what() << endl;
		cout << "MySQL Error Code : " << e.getErrorCode() << endl;
		cout << "SQLState" << e.getSQLState() << endl;
	}
	
}

bool mysql::isDuplicateNickname(string nickname) {
	try {
		sql::PreparedStatement* pstmt;
		sql::ResultSet* result;

		string query =
			"SELECT COUNT(*)"
			"FROM userinfo"
			"WHERE nickname = ?";
		pstmt = mysql_conn->prepareStatement(query);
		pstmt->setString(1, nickname);

		result = pstmt->executeQuery();
		result->next();

		if (result->getInt(1) == 0)
			return false;
		else
			return true;
	}

	catch (sql::SQLException& e) {
		cout << "SQLException : " << e.what() << endl;
		cout << "MySQL Error Code : " << e.getErrorCode() << endl;
		cout << "SQLState" << e.getSQLState() << endl;
	}
}

// Redis와의 연결을 생성한다.
void redis::makeConnection() {
	redis_conn = new sw::redis::Redis("tcp://" + string(REDIS_ADDRESS) + ":" + to_string(REDIS_PORT));
}

string redis::getValue(string key) {
	return *(redis_conn->get(key));
}

void redis::setValue(string key, string value) {
	redis_conn->set(key, value);
}
	