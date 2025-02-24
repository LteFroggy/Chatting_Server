#pragma once
#include "clientHandler.h"
#include "exceptions.h"

#include <string>
#include <jdbc/mysql_driver.h>  // MySQL 
#include <jdbc/mysql_connection.h>  // MySQL 
#include <jdbc/cppconn/prepared_statement.h>  // MySQL
#include <jdbc/cppconn/resultset.h>  // MySQL
#include <sw/redis++/redis.h>  // Redis

#define MYSQL_PORT 3306
#define MYSQL_ADDRESS "127.0.0.1"
#define REDIS_PORT 6379
#define REDIS_ADDRESS "127.0.0.1"

using namespace std;

struct userInfo {
	string id;
	string pwd;
	string nickname;
	
	userInfo(string id, string pwd, string nickname) : id(id), pwd(pwd), nickname(nickname) {}
};

/*
*	mysql과의 통신을 담당하는 싱글톤 클래스
*/
class mysql {
private:
	static mysql* instance;
	sql::mysql::MySQL_Driver* driver;
	sql::Connection* connector;

	mysql() {}
	void makeConnection();
public: 

	static mysql* getInstance();
	// clientHandler getUserinfoByID(string id);
	// clientHandler getUserinfoByNickname(string nickname);
	userInfo getLoginInfo(string id);
	void registNewUser(string id, string pwd);
	bool isDuplicateID(string id);
	bool isDuplicateNickname(string nickname);
};

// Redis 관련 함수가 정의된 폴더
class redis {
private :
	static redis* instance;
	sw::redis::Redis* connector;

	redis() {}
	void makeConnection();
public:
	static redis* getInstance();
	string getValue(string key);
	void setValue(string key, string value);
};