#include "Protocol.h"

using namespace protocol


// ====================================================================================================================================
// 특정 대상에게 메세지를 보내는 함수
// ====================================================================================================================================
void protocol::sendMessage(SOCKET client_socket, msg_format msg) {
	// 앞의 4글자는 항상 코드가 된다.
    string toSend = to_string(msg.code) + msg.body;
    send(client_socket, msg.c_str(), msg.length(), 0);
}

void protocol::receiveMessage(SOCKET client_socket, string msg) {
    // 일단 받은 string을 해독한다.
    protocol::msg_format msg = (msg.substr(0, 4), msg.substr(4, msg.length() - 4));
    // 앞의 4글자를 바탕으로 알맞은 디코딩 함수에 넣는다.
    protocol::
}

string login::encodeMessage(msg_format msg) {

}

protocol::loginResponse login::decodeMessage(string msg) {

}

string regist::encodeMessage(msg_format msg) {

}

protocol::registResponse regist::decodeMessage(string msg) {

}