#include <string>
#include <vector>
#include <iostream>
#include <ws2tcpip.h> // inet_pton 사용을 위한 헤더 추가
#include <WinSock2.h>

#pragma comment(lib, "ws2_32")

using namespace std;

#define PORT 13542
#define BUFFER_SIZE 1024
#define SERVER_IP "127.0.0.1"

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

int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);


    // 클라이언트의 소켓을 생성한다.
    SOCKET clientSocket;
    clientSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) {
        cout << "클라이언트 소켓 생성 실패" << endl;
        return -1;
    }

    SOCKADDR_IN serverAddr = {}; // 서버 주소 정보를 저장할 구조체 초기화
    serverAddr.sin_family = PF_INET; // IPv4 주소 체계를 사용
    serverAddr.sin_port = htons(PORT); // 포트 번호를 네트워크 바이트 순서로 변환 후 할당
    inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr);


    // 위의 정보를 이용해 서버와 연결한다.
    if (connect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cout << "Connection failed : " << WSAGetLastError() << endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    // 메세지를 하나 보내고 종료한다
    string message = "Hello Server! I'm Client";
    if (send(clientSocket, message.c_str(), message.length(), 0) == SOCKET_ERROR) {
        cout << "메세지 전송에 실패하였습니다" << endl;
    }
    else {
        cout << "메세지 전송 성공!" << endl;
    }

    WSACleanup();
    return 0;
}