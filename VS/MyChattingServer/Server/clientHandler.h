#pragma once
#include <ws2tcpip.h>
#include <WinSock2.h>

#include "Protocol.h"
#include "server.h"
#include "login.h"
class msg_format;
class Server;

using namespace std;

/*
*	ClientHandler는 Client가 들어올때마다 생성된다.
*/
class clientHandler {
private:
	string id;
	string nickname;
	SOCKET clientSocket;

public:
	// 기본 생성자
	clientHandler() {}
	// 생성자
	clientHandler(SOCKET clientSocket) : clientSocket(clientSocket) {}

	// 아이디 가져오기
	string getID() {
		return id;
	}

	// 닉네임 가져오기
	string getNickname() {
		return nickname;
	}

	// Thread의 시작을 위한 함수
	void startClientHandler();

	// 클라이언트에게 메세지를 보낸다
	void sendMessage(msg_format* msg);

	// 클라이언트에게 메세지를 받는다
	void receiveMessage();
};