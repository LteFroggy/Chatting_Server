#include "Protocol.h"

// ====================================================================================================================================
// 특정 대상에게 메세지를 보내는 함수
// ====================================================================================================================================
void message::sendMessage(SOCKET client_socket, msg_format msg) {
    // 코드에 따라 적절한 인코딩 함수에 넣는다.
    if (msg.code == LOGIN_REQUEST) {

    }

    else if (msg.code == LOGIN_RESPONSE) {

    }
    send(client_socket, msg, msg, 0);
}

void message::receiveMessage(SOCKET client_socket, string msg) {
    // 일단 받은 string을 해독한다.
    msg_format msg = (msg.substr(0, 4), msg.substr(4, msg.length() - 4));
    // 앞의 4글자를 바탕으로 알맞은 디코딩 함수에 넣는다.
}

// ====================================================================================================================================
// 로그인을 위해 메세지를 인코딩한다.
// ====================================================================================================================================
string login::encodeMessage(msg_format msg) {
    loginResponse response;
    response.code = msg.code;
    response.result = static_cast<loginResult>(msg.body.substr(0, 1));
}

protocol::loginResponse login::decodeMessage(string msg) {

}

string regist::encodeMessage(msg_format msg) {

}

protocol::registResponse regist::decodeMessage(string msg) {

}