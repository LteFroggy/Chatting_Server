#pragma once

#include "Protocol.h"
#include "dbUtils.h"

enum loginResult {
	SUCCESS = 0,
	NO_ID = 1,
	WRONG_PWD = 2,
	SERVER_ERROR = 3
};

class loginRequest : public msg_format {
private :
	string id;
	string pwd;

public :
	loginRequest(messageCode code, string id, string pwd) : msg_format(code), id(id), pwd(pwd) {}
	//loginRequest(string msg) : msg_format(msg) {}

	void setId(string id);
	string getId();
	void setPwd(string pwd);
	string getPwd();

	string encodeMessage() override;
	void decodeMessage() override;
	optional<msg_format*> processMessage() override;
};

class loginResponse : public msg_format {
private :
	loginResult result;
	string nickname;
	string msg;

public :
	loginResponse(messageCode code, loginResult result, string nickname, string msg) : msg_format(code), result(result), nickname(nickname), msg(msg) {}
};