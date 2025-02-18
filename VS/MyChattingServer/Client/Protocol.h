#pragma once

#include "socketUtils_client.h"

#define BUFFER_SIZE 256

enum messageCode {
	LOGIN_REQUEST = 1001,
	LOGIN_RESPONSE = 1002,
	REGIST_REQUEST = 1003,
	REGIST_RESPONSE = 1004,

	CHAT_MESSAGE = 2001,

	SERVER_MESSAGE = 3001,
};

enum loginResult {
	SUCCESS = 0,
	NO_ID = 1,
	WRONG_PWD = 2,
	NO_NICKNAME = 3,
	SERVER_ERROR = 4,
}

enum registResult {
	SUCCESS = 0,
	DUPLICATE_ID = 1,
	DUPLICATE_NICKNAME = 2,
	SERVER_ERROR = 3,
}

struct msg_format {
	messageCode code;
	char body[BUFFER_SIZE];
};

struct loginResponse {
	messageCode code;
	loginResult result;
	string message;
};

struct registResponse {
	messageCode code;
	registResult result;
	string message;	
};

class message {
	static void sendMessage(SOCKET client_socket, msg_format msg);
	static void receiveMessage(SOCKET client_socket, msg_format msg);
};

class login {
	static string encodeMessage(msg_format msg);
	static loginResponse decodeMessage(string msg);
};

class regist {
	static string encodeMessage(msg_format msg);
	static registResponse decodeMessage(string msg);
}