#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <iostream>
#include <ws2tcpip.h> // inet_pton 사용을 위한 헤더 추가
#include <WinSock2.h>

#include "socketUtils_client.h"

using namespace std;

/*
    클라이언트는 소켓을 생성하고, 연결을 요청한다.

    연결 후에는 다양한 메세지를 보내고, 메세지의 헤더를 통해 서버는 그에 걸맞는 요청을 처리한다.
    요청 처리 시에는 꼭 권한이 존재하는 상황인지 확인해야 함.

    ### 채팅 헤더 관련

    Client에서 전송한 메세지가 어떤 기능을 수행하기 위한 메세지인지를 구분하기 위해, 헤더를 통해 구분한다
    - login → “login id/pw” 와 같은 형식으로 전송되며, 받은 메세지를 기반으로 DB조회를 통해 로그인을 수행한다
    - join → “join id/pw”와 같은 형식으로 전송되고, 서버는 이를 기반으로 DB에 등록한다. ID중복확인은 필요
    - chat → “chat msg” 와 같은 형식으로 전송되며, 메세지를 전송
    - gamechat → “gamechat msg” 와 같은 형식으로 전송되며, 끝말잇기 낱말을 전송한다
    - whisper → “whisper target msg”와 같은 형식으로 전송되고, 타겟에게 귓속말을 준다
    - quit
*/
mutex MSG_MUTEX;
vector<string> MESSAGE_LIST;
string STATE = "LOGIN";

void toLoginScreen();
void printScreen();

int main() {
    SOCKET clientSocket = socketUtils::connectToServer();

    if (clientSocket == -1) {
        cout << "서버와 연결에 실패하였습니다" << endl;
        WSACleanup();
        return 0;
    }

    // 로그인 화면으로 넘어간다
    toLoginScreen();

    WSACleanup();
    return 0;
}

// 처음엔 로그인 화면으로 들어간다
void toLoginScreen() {
    // 초기화면 메세지 출력
    MSG_MUTEX.lock();
    MESSAGE_LIST.push_back("서버에 접속되었습니다");
    MESSAGE_LIST.push_back("로그인 : 1");
    MESSAGE_LIST.push_back("회원가입 : 2");
    MSG_MUTEX.unlock();
    printScreen();

    // 유저에게 1 혹은 2를 입력받을때까지 입력 받기
    string userInput;
    while (true) {
        cout << "번호를 입력하세요 : ";
        getline(cin, userInput);

        if (userInput == "1" || userInput == "2") break;
        else cout << "올바르지 않은 입력입니다! 다시 입력해주세요" << endl;
    }

    // 1번이면, 로그인을 시도한다.
    if (userInput == "1") {
        
    }
}

void printScreen() {
    system("cls");
    for (auto v : MESSAGE_LIST) cout << v << endl;
    return;
}