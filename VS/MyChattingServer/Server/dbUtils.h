#pragma once

#include <stdexcept>
#include <string>
#include <jdbc/mysql_driver.h>
#include <jdbc/mysql_connection.h>  // MySQL 
#include <jdbc/cppconn/prepared_statement.h>
#include <jdbc/cppconn/resultset.h>
#include <sw/redis++/redis.h>

// 통신을 위해 사용하게 될 conn
extern sql::Connection *mysql_conn;
extern sw::redis::Redis *redis_conn;

#define MYSQL_PORT 3306
#define MYSQL_ADDRESS "127.0.0.1"
#define REDIS_PORT 6379
#define REDIS_ADDRESS "127.0.0.1"

using namespace std;

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