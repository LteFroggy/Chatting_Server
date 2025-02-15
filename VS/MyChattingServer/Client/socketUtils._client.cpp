#include "socketUtils_client.h"

using namespace std;

// 서버 소켓에 연결한다.
SOCKET socketUtils::connectToServer() {
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
        return -1;
    }

    return clientSocket;
}