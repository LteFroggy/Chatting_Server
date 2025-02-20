#pragma once

#include <mutex>
#include <string>
#include <iostream>
#include <ws2tcpip.h>
#include <WinSock2.h>
#include <unordered_map>

#include "clientHandler.h"

using namespace std;

#define PORT 13542
#define BUFFER_SIZE 1024

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

/*
*	서버 클래스는 하나의 인스턴스만 가지며, 유저 목록과 소켓을 가진다.
*/
class Server {
private :
	// 로그인 완료한 유저가 저장될 맵
	unordered_map<string, clientHandler> userMap;
	SOCKET serverSocket;
	// 서버가 저장될 저장공간
	static Server* instance;
	static mutex mtx;

	// 생성자 호출 불가능하다. getInstance만을 이용해 생성해야 함 
	Server() {}

public :
	// 해당 메소드를 이용해서만 Server의 호출이 가능하다.
	static Server* getInstace();

	// 서버의 소켓을 세팅하는 함수
	void openSocket();

	// 특정 유저를 userMap에 추가하는 함수
	void addToUserMap(SOCKET client_socket, clientHandler client);

	// BroadCast를 위해 모든 유저의 정보를 반환하는 함수
	unordered_map<string, clientHandler> getUserList();

	// 단일 유저의 정보를 알아내기 위해 유저의 정보를 찾아오는 함수
	clientHandler& getUserSocketNum(string nickname);
};