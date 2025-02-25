#include "server.h"

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

#pragma comment(lib, "ws2_32.lib")

using namespace std;
using namespace sw::redis;

int main() {
    // 소켓 생성
    Server* myServer = Server::getInstance();
    try {
        myServer->openSocket();
    }
    catch (exception e) {
        cout << e.what() << endl;
    }

    /* Redis Test! */
    cout << "Redis Test!" << endl;
    redis* redisConn = redis::getInstance();
    redisConn->setValue("Myname", "Wonjong");
    cout << redisConn->getValue("Myname") << endl;

    cout << endl << endl;

    /* MySQL Test */
    cout << "MySQL TEST" << endl;
    mysql* sqlInstance = mysql::getInstance();
    userInfo tmp = sqlInstance->getLoginInfo("zac0328");
    cout << "유저의 닉네임은 " + tmp.nickname << endl;

    if (listen(myServer->getSocket(), SOMAXCONN) == SOCKET_ERROR) {
        cerr << "Listen Failed : " << WSAGetLastError() << endl;
        closesocket(myServer->getSocket());
        WSACleanup();
        system("pause");
        return 2;
    }

    cout << "클라이언트 접속 대기 중" << endl;

    /*
        들어오면 Accept한다.

        accept는 동기화된 방식으로 동작하는데, 이는 요청을 마무리하기 전까지 wait상태가 되는 것을 의미한다.
        접속을 승인하면 연결된 소켓이 만들어진다.
        인자로는 소켓, accept된 클라이언트의 구조정보 구조체, 그 크기가 들어간다.
    */
    while (true) {
        // sockaddr과 socket을 만들고, accept를 수행한다.
        SOCKADDR_IN tClntAddr = {};
        int iClntSize = sizeof(tClntAddr);
        SOCKET hClient = accept(myServer->getSocket(), (SOCKADDR*)&tClntAddr, &iClntSize);

        // 잘 연결되지 않았다면, 위의 루프를 다시 진행
        if (hClient == INVALID_SOCKET) {
            closesocket(hClient);
            continue;
        }

        // 클라이언트가 연결되면, 별도 스레드를 만들고 분리한다
        cout << "클라이언트 접속됨" << endl;

        thread client_thread(, hClient);
        client_thread.detach();
        
        /*
        // 일단 메세지 하나만 받고 바로 돌려보내기로 하자
        int recv_size = recv(hClient, buffer, BUFFER_SIZE - 1, 0);
        // 받은 메세지 크기를 잘라줘야만 원활히 출력된다
        buffer[recv_size] = '\0';

        cout << buffer << endl;
        break;
        */
    }


    WSACleanup();
    return 0;
}