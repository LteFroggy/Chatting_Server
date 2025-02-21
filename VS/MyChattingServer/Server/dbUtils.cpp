#include "dbUtils.h"

// ====================================================================================================================================
// MySQL class의 instance를 반환해주는 함수
// 값이 없었다면, connection까지 생성한다.
// ====================================================================================================================================
mysql* mysql::getInstance() {
	if (instance == nullptr) {
		instance = new mysql();
		instance->makeConnection();
	}
	return instance;
}

// ====================================================================================================================================
// MySQL과의 연결을 생성한다.
// ====================================================================================================================================
void mysql::makeConnection() {
	try {
		// 드라이버 초기화
		driver = sql::mysql::get_mysql_driver_instance();
		// 드라이버를 통해 특정 주소, 포트에 연결한다.
		connector = driver->connect("tcp://" + string(MYSQL_ADDRESS) + ":" + to_string(MYSQL_PORT), "root", "1372");

		// 채팅 서버 database 설정
		connector->setSchema("chatserver");
	}

	catch (sql::SQLException& e) {
		cout << "SQLException : " << e.what() << endl;
		cout << "MySQL Error Code : " << e.getErrorCode() << endl;
		cout << "SQLState" << e.getSQLState() << endl;
	}
}

// ====================================================================================================================================
// 로그인을 위해 아이디 바탕으로 유저정보를 반환한다.
// ====================================================================================================================================
userInfo mysql::getLoginInfo(string id) {
	// 아이디를 바탕으로 유저정보를 검색한다.
	sql::PreparedStatement* pstmt;
	sql::ResultSet* result;

	string query =
		"SELECT id, pwd, nickname"
		"FROM userinfo"
		"WHERE id = ?";

	pstmt = connector->prepareStatement(query);
	pstmt->setString(1, id);
	result = pstmt->executeQuery();

	// 쿼리 결과가 없으면, 아이디가 없는 것이다!
	if (!result->next()) {
		throw NoIDException("NOID");
	}

	// 결과가 존재하면, 구조체에 담아 반환한다.
	return userInfo(result->getString(1), result->getString(2), result->getString(3));
}

// ====================================================================================================================================
// 새로운 유저를 등록한다
// ====================================================================================================================================
void mysql::registNewUser(string id, string pwd) {
}

// ====================================================================================================================================
// 회원가입 시에 아이디 중복체크를 수행한다.
// ====================================================================================================================================
bool mysql::isDuplicateID(string id) {
	try {
		sql::PreparedStatement* pstmt;
		sql::ResultSet* result;

		string query =
			"SELECT COUNT(*)"
			"FROM userinfo"
			"WHERE id = ?";
		pstmt = connector->prepareStatement(query);
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

// ====================================================================================================================================
// 회원가입 시에 닉네임 중복체크를 수행한다.
// ====================================================================================================================================
bool mysql::isDuplicateNickname(string nickname) {
	try {
		sql::PreparedStatement* pstmt;
		sql::ResultSet* result;

		string query =
			"SELECT COUNT(*)"
			"FROM userinfo"
			"WHERE nickname = ?";
		pstmt = connector->prepareStatement(query);
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

// ====================================================================================================================================
// Redis Connector의 instance를 반환한다
// 없어서 생성해야 한다면, 연결까지 한번에 생성한다.
// ====================================================================================================================================
redis* redis::getInstance() {
	if (instance == nullptr) {
		instance = new redis();
		instance->makeConnection();
	}
	return instance;
}

// ====================================================================================================================================
// redis의 연결을 생성한다.
// ====================================================================================================================================
void redis::makeConnection() {
	if (instance == nullptr) {
		instance = new redis();
		connector = new sw::redis::Redis("tcp://" + string(REDIS_ADDRESS) + ":" + to_string(REDIS_PORT));
	}
}

// ====================================================================================================================================
// 특정 키에 대한 값을 받아온다
// ====================================================================================================================================
string redis::getValue(string key) {
	return *(connector->get(key));
}

// ====================================================================================================================================
// 특정 키의 값을 생성한다.
// ====================================================================================================================================
void redis::setValue(string key, string value) {
	connector->set(key, value);
}




/* DB는 유저 정보 검색할 필요가 아직 없어서 구현 안함
clientHandler mysql::getUserinfoByID(string id) {
	try {
		sql::PreparedStatement* pstmt;
		sql::ResultSet* result;

		string query =
			"SELECT id, pwd, nickname "
			"FROM userinfo "
			"WHERE id = ?";

		// id기반으로 유저 찾기
		pstmt = connector->prepareStatement(query);
		pstmt->setString(1, id);

		result = pstmt->executeQuery();

		// 결과가 없다면, exception 던지기
		if (result->next()) throw NoIDException("일치하는 아이디가 없습니다");

		// tmp라는 구조체 만들어 데이터 할당하고 반환
		clientHandler()
		userInfo tmp(result->getString("id"), result->getString("pwd"), result->getString("nickname"));

		// 리소스 정리
		delete pstmt;
		delete result;

		return tmp;
	}

	catch (sql::SQLException& e) {
		cout << "SQLException : " << e.what() << endl;
		cout << "MySQL Error Code : " << e.getErrorCode() << endl;
		cout << "SQLState" << e.getSQLState() << endl;
	}
clientHandler mysql::getUserinfoByNickname(string nickname) {
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

		// 결과가 없다면, exception 던지기
		if (result->next()) throw NoIDException("닉네임과 일치하는 아이디가 없습니다");

		// tmp라는 구조체 만들어 데이터 할당하고 반환
		userInfo tmp(result->getString("id"), result->getString("pwd"), result->getString("nickname"));

		// 리소스 정리
		delete pstmt;
		delete result;

		return tmp;
	}

	catch (sql::SQLException& e) {
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
*/