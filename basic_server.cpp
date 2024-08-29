#pragma comment(lib, "ws2_32.lib")

#include <iostream>
#include <ws2tcpip.h>
#include <WinSock2.h>

using namespace std;

#define PORT 13542
#define PACKET_SIZE 1024

/*
    서버는 기본적으로 socket을 열고 클라이언트의 접속을 기다린다.
*/

int main() {
    WSADATA wsaData;
    /*
        WinSock 초기화를 위한 코드

        WSADATA는 Windows의 소켓 초기화 정보 저장을 위한 구조체이다.
        WSAStartup(소켓 버전, WSADATRA 구조체 주소)
            제일 먼저 호출해야 하는 함수로, 어떤 소켓을 Windows에 알려주는 것.
            소켓 버전에는 2.2를 사용할 것이고, WORD type으로 들어간다.
            WORD는 unsigned int타입인데, 2.2는 double값이므로 MAKEWORD를 이용해 2.2를 사용해 만든다.
            2번째에는 WSADATA의 포인터를 만들어 넣는다.
    */
    int iResult;
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        cerr << "WSAStartup Failed : " << iResult << endl;
        return -1;
    }

    
    /*
        소켓 새로 생성하기

        socket(int domain, int type, int protocol)

        domain
            AF_INET : IPv4 사용
            AF_INET6 : IPv6 사용

        type
            SOCK_STREAM : 연결지향형, 신뢰성있는 통신 지원, 순서대로 도착하며 경계가 없다. (TCP)
            SOCK_DGRAM : 비연결지향형, 신뢰성보단 속도를 중요시하며, 패킷 단위로 데이터를 전송한다. (UDP)

        protocol -> 기본적으로 Domain, Type에 의해 자동으로 결정되나, 명시하고 싶을 때 사용
            IPROTO_TCP : TCP를 사용하겠다고 선택
            IRPOTO_UDP : UDP를 사용하겠다는 의미
    */
    SOCKET server_socket;
    server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_socket == INVALID_SOCKET) {
        cerr << "소켓 생성 실패 : " << WSAGetLastError() << endl;
        closesocket(server_socket);
        WSACleanup();
        system("pause");
        return -1;
    }

    /*
        소켓의 구성요소를 담을 구조체 생성 및 값 할당

        위에 언급한  소켓의 구성요소를 지정하여 주소정보를 만든다.
        주소는 IP와 PORT의 두 가지 요소가 있다.

        SOCKADDR_IN은 Windows소켓에서 소켓을 연결할 로컬 및 원격 주소를 지정하는 데에 사용된다.
        즉, 주소 정보를 담아두기 위한 구조체이다.

        sin_family : 반드시 AF_INET이어야 한다.
        sin_port : 포트 번호를 설정한다. 2byte내에서 표현이 가능해야 하며, 기본 포트번호를 제외한 다른 번호를 써야 한다.
            htons, htonl : Host to Network의 약자로, 이 함수를 사용하면 빅엔디안(데이터를 메모리에 앞에서부터 저장) 방식으로 데이터를 변환해 사용한다.
        sin_addr : IP주소 설정하기
            s_addr : IPv4를 의미한다.
            INADDR_ANY는 현재 동작중인 컴퓨터의 IP 주소를 의미한다.

    */
    SOCKADDR_IN tListenAddr = {};
    tListenAddr.sin_family = AF_INET;
    tListenAddr.sin_port = PORT;
    tListenAddr.sin_addr.s_addr = INADDR_ANY;
    
    /*  
        소켓을 sockaddr에 바인딩하고, listen상태로 만든다.
        
        bind(소켓, 소켓 구성요소 구조체의 주소, 구조체의 크기)
        listen(소켓, 최대 접속중인 수)
    */
    if (bind(server_socket, (SOCKADDR*)&tListenAddr, sizeof(tListenAddr)) == SOCKET_ERROR) {
        cerr << "Bind Failed : " << WSAGetLastError() << endl;
        closesocket(server_socket);
        WSACleanup();
        system("pause");
        return 1;
    }

    if (listen(server_socket, SOMAXCONN) == SOCKET_ERROR) {
        cerr << "Listen Failed : " << WSAGetLastError() << endl;
        closesocket(server_socket);
        WSACleanup();
        system("pause");
        return 2;
    }

    cout << "Waiting for client" << endl;

    /*
        들어오면 Accept한다.

        accept는 동기화된 방식으로 동작하는데, 이는 요청을 마무리하기 전까지 wait상태가 되는 것을 의미한다.
        접속을 승인하면 연결된 소켓이 만들어진다.
        인자로는 소켓, accept된 클라이언트의 구조정보 구조체, 그 크기가 들어간다.
    */
    SOCKADDR_IN tClntAddr = {};
    int iClntSize = sizeof(tClntAddr);
    SOCKET hClient = accept(server_socket, (SOCKADDR*)&tClntAddr, &iClntSize);

    if (hClient == INVALID_SOCKET) {
        cerr << "Accept Failed : " << WSAGetLastError() << endl;
        closesocket(server_socket);
        closesocket(hClient);
        WSACleanup();
        system("pause");
        return 3;
    }

    /*
        버퍼를 생성하고 메세지를 받고, 메세지를 또 보낸다

        recv에는 받을 소켓, 메세지를 받을 공간, 그 공간의 크기, flag를 넣는다.
        flag는 별도로 활성화하지 않을 것이므로 쓰지 않음

        send는 보낼 소켓, 보낼 메세지, 길이, flag를 넣는다.
    */
    char cBuffer[PACKET_SIZE] = {};
    char cMsg[] = "Server Hello";
    
    recv(hClient, cBuffer, PACKET_SIZE, 0);
    send(hClient, cMsg, strlen(cMsg), 0);

    cout << "Received From Client : " << cBuffer << endl;

    // 소켓 닫기
    closesocket(server_socket);
    closesocket(hClient);

    WSACleanup();
    system("pause");
    return 0;
}