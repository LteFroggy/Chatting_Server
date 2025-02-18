#pragma once

#include "socketUtils_client.h"

#define BUFFER_SIZE 256

struct Message {
	int messageCode;
	int bodyLength;
	char body[BUFFER_SIZE];
};

enum MessageCode {
	LOGIN_REQUEST = 1001,
	LOGIN_RESPONSE = 1002,
	REGIST_REQUEST = 1003,
	REGIST_RESPONSE = 1004,

	CHAT_MESSAGE = 2001,

	SERVER_MESSAGE = 3001,
};

class message {
	static void sendMessage(SOCKET client_socket, string msg, int code);
	static void receiveMessage(SOCKET client_socket, string result);
};