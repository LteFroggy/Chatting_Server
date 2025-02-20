#include "clientHandler.h"

// 클라이언트에게 메세지를 보낸다
void clientHandler::sendMessage(msg_format* msg) {
	// 코드에 맞게 인코딩을 수행한다
	if (msg->getCode() == LOGIN_REQUEST) {
		string send = msg->encodeMessage();

	}
}

// 클라이언트에게 메세지를 받는다
void clientHandler::receiveMessage(string msg) {
	// 먼저 코드를 확인한다
	messageCode code = static_cast<messageCode>(stoi(msg.substr(0, 4)));
	msg_format* message;

	// 코드에 맞게 디코딩을 수행한다
	// 로그인 시도 시!
	if (code == LOGIN_REQUEST) {
		// loginRequest 객체를 만들어 할당
		message = new loginRequest(msg);

		message->decodeMessage();
	}
}