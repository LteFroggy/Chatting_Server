#pragma once

#include <string>
#include <jdbc/mysql_driver.h>
#include <jdbc/mysql_connection.h>  // MySQL 
#include <jdbc/cppconn/prepared_statement.h>
#include <jdbc/cppconn/resultset.h>
#include <sw/redis++/redis.h>

#include "clientHandler.h"

#define MYSQL_PORT 3306
#define MYSQL_ADDRESS "127.0.0.1"
#define REDIS_PORT 6379
#define REDIS_ADDRESS "127.0.0.1"

using namespace std;

/*
*	mysql과의 통신을 담당하는 싱글톤 클래스
*/
class mysql {
private:
	static mysql* instance;

public: 
	static mysql* getInstance() {
		if (instance == nullptr) {
			instance = new mysql();
			instance->makeConnection();
		}
		return instance;
	}

	void makeConnection();
	clientHandler getUserinfoByID(string id);
	clientHandler getUserinfoByNickname(string nickname);
	void registNewUser(string id, string pwd);
	bool isDuplicateID(string id);
	bool isDuplicateNickname(string nickname);
};

// Redis 관련 함수가 정의된 폴더
class redis {
public:
	static void makeConnection();
	static string getValue(string key);
	static void setValue(string key, string value);
};