#pragma once

#include <exception>
#include <string>

using namespace std;

// 서버에서 발생하는 오류
class ServerException : exception {
private:
	std::string message;
public:
	// 생성자
	explicit ServerException(const string& msg) : message(msg) {}

	// 예외 메시지를 반환
	const char* what() const noexcept override {
		return message.c_str();
	}
};

// MySQL에서 생기는 오류를 확인하기 위함
class MySQLException : exception {
private:
	string msg;
public:
	MySQLException(string msg) : msg(msg) {}

	const char* what() const noexcept override {
		return msg.c_str();
	}
};

// 어떤 유저를 찾았을 때, 없는 경우에 생기는 Exception
class NoIDException : public exception {
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

/*
*	클라이언트와 통신을 처리하는 과정에서 발생하는 오류
*/
class CommunicateException : public exception {
private:
	string msg;
	// 생성자
public:
	explicit CommunicateException(const string& msg) : msg(msg) {}

	const char* what() const noexcept override {
		return msg.c_str();
	}
};
