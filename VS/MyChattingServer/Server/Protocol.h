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

	// �޼��� �ص��� ���� split�ϴ� �����Լ�
	vector<string> split(string target, char deli = '|');
	// msg_format�� ������ ���� str�� �����
	virtual string encodeMessage();
	// ���۵� ����� �޼����� ���ڵ��Ѵ�
	virtual void decodeMessage();
	// ���۹��� �޼����� ó���ϰ�, �ʿ��ϴٸ� ��ȯ�ϴ� �޼����� �����.
	virtual optional<msg_format*> processMessage();
};