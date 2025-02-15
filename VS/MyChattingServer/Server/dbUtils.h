#pragma once

#include <stdexcept>
#include <string>
#include <jdbc/mysql_driver.h>
#include <jdbc/mysql_connection.h>  // MySQL 
#include <jdbc/cppconn/prepared_statement.h>
#include <jdbc/cppconn/resultset.h>
#include <sw/redis++/redis.h>

// 통신을 위해 사용하게 될 conn
extern sql::Connection* mysql_conn;
extern sw::redis::Redis* redis_conn;

#define MYSQL_PORT 3306
#define MYSQL_ADDRESS "127.0.0.1"
#define REDIS_PORT 6379
#define REDIS_ADDRESS "127.0.0.1"

using namespace std;

// 어떤 유저를 찾았을 때, 없는 경우에 생기는 Exception
class NoIDException : exception {
private:
	std::string message;
public:
	// 생성자
	explicit NoIDException(const string& msg) : message(msg) {}

	// 예외 메시지를 반환
	const char* what() const noexcept override {
		return message.c_str();
	}
};

struct userInfo {
	string id;
	string pwd;
	string nickname;

	userInfo(string id, string pwd, string nickname) {
		this->id = id;
		this->pwd = pwd;
		this->nickname = nickname;
	}
};

// MySQL관련 함수가 정의된 폴더
class mysql {
public:
	static void makeConnection();
	static userInfo getUserinfoByID(string id);
	static userInfo getUserinfoByNickname(string nickname);
	static void registNewUser(string id, string pwd);
	static bool isDuplicateID(string id);
	static bool isDuplicateNickname(string nickname);
};

// Redis 관련 함수가 정의된 폴더
class redis {
public:
	static void makeConnection();
	static string getValue(string key);
	static void setValue(string key, string value);
};