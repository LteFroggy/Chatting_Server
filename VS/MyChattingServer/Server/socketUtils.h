#pragma once

#include <ws2tcpip.h>
#include <WinSock2.h>
#include <iostream>
#include <mutex>
#include <unordered_map>

#include "dbUtils.h"

using namespace std;

#define PORT 13542
#define BUFFER_SIZE 1024

class socketUtils {
public :
	static unordered_map<int, string> sessionMap;
	static unordered_map<string, int> userMap;
	static mutex sessionMutex;
	static mutex userMutex;

	static SOCKET openSocket();

	static void addNewSession(SOCKET client_socket);
	static void addToUsermap(SOCKET client_socket, userInfo user);
	static void handleClient(SOCKET client_socket);

	static void user_login(SOCKET client_socket, string msg);
	static void sendMessage(SOCKET client_socket, string msg, int code);
};

class utils {
public :
	static pair<string, string> decodeMessage(string recv);
};