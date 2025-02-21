#include "clientHandler.h"

// 클라이언트에게 메세지를 보낸다
void clientHandler::sendMessage(msg_format* msg) {
	// 코드에 맞게 인코딩을 수행한다
	if (msg->getCode() == LOGIN_REQUEST) {
		string send = msg->encodeMessage();

	}
}

// 클라이언트에게 메세지를 받는다
void clientHandler::receiveMessage() {
	char buffer[BUFFER_SIZE];
	int recv_size = recv(clientSocket, buffer, BUFFER_SIZE - 1, 0);

	// 전송된 메세지의 크기에 맞게 문자열의 끝을 등록해주고, string형 변수에 바꿔 넣는다.
	buffer[recv_size - 1] = '\0';
	string msg = buffer;

	// 먼저 코드를 확인한다
	messageCode code = static_cast<messageCode>(stoi(msg.substr(0, 4)));
	msg_format* message;

	// 코드에 맞게 디코딩을 수행한다
	// 로그인 시도 시!
	if (code == LOGIN_REQUEST) {
		// loginRequest 객체를 만들어 할당
		message = new loginRequest(msg);

		message->decodeMessage();
		// 메세지를 프로세싱해서 사용한다.
		msg_format* response = message->processMessage().value_or(nullptr);

		// 프로세싱된 메세지를 이용해 답장한다
		sendMessage(response);
	}
}