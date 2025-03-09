#pragma once

#include <optional>
#include <sstream>
#include <vector>

#include "server.h"

using namespace std;

#define BUFFER_SIZE 256

enum class messageCode {
	LOGIN_REQUEST = 1001,
	LOGIN_RESPONSE = 1002,
	REGIST_REQUEST = 1003,
	REGIST_RESPONSE = 1004,

	CHAT_MESSAGE = 2001,

	SERVER_MESSAGE = 3001,
};

class msg_format {
private :
	messageCode code;
	string encodedMessage = "";

public :
	msg_format(messageCode a) : code(a) {}
	msg_format(string encodedMessage) : encodedMessage(encodedMessage) {}

	messageCode getCode();
	void setCode(messageCode input);
	string getMsg();
	void setMsg(string message);

	// 메세지 해독을 위해 split하는 내장함수
	vector<string> split(string target, char deli = '|');
	// msg_format을 전송을 위한 str로 만든다
	virtual string encodeMessage();
	// 전송된 저장된 메세지를 디코딩한다
	virtual void decodeMessage();
	// 전송받은 메세지를 처리하고, 필요하다면 반환하는 메세지를 만든다.
	virtual optional<msg_format*> processMessage();
};