#include <mutex>
#include <string>
#include <thread>
#include <vector>
#include <iostream>
#include <algorithm>
#include <ws2tcpip.h>
#include <WinSock2.h>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

#define PORT 13542
#define BUFFER_SIZE 1024

// 접속한 모든 Client를 저장하기 위한 벡터
vector<vector<SOCKET>> server_clients(2);

// Client vector를 컨트롤할 때 사용하기 위한 mutex
vector<mutex> client_mutex(2);

/*
    서버는 기본적으로 socket을 열고 클라이언트의 접속을 기다린다.

    그런데, Accept는 하나의 Client만 받을 수 있으므로 새로운 스레드를 생성해서 클라이언트 요청을 처리한다.
*/

// 모든 접속자에게 메세지 보내기
void broadCastMessage(string msg, int server, SOCKET except) {
    client_mutex[server].lock();
    for (auto v : server_clients[server]) {
        if (v == except) {
            continue;
        }
        send(v, msg.c_str(), msg.length(), 0);
    }
    client_mutex[server].unlock();

    return;
}

// 특정 접속자에게 메세지 보내기
void sendMsg(SOCKET target, string msg) {
    send(target, msg.c_str(), msg.length(), 0);
}

// 새로운 클라이언트를 처리하기 위한 함수
void handle_client(SOCKET client_socket) {
    char cBuffer[BUFFER_SIZE];
    string client_name = "";
    int server(0);

    // 먼저 어떤 채널에 입장할지 정하게 하도록 하기 위해 서버별 인원수를 제공한다
    sendMsg(client_socket, "서버 1 인원수 : " + to_string(server_clients[0].size()) + "\n서버 2 인원수 : " + to_string(server_clients[1].size()) + "\n접속할 서버를 선택하세요 (1 or 2)");
        
    int valRead = recv(client_socket, cBuffer, BUFFER_SIZE, 0);
    // cout << "입력 서버값 수신 완료" << endl;
    cBuffer[valRead] = '\0';

    if (cBuffer == "1") server = 0;
    if (cBuffer == "2") server = 1;

    // 그 결과에 따라 서버에 입장시킨다.
    client_mutex[server].lock();
    server_clients[server].push_back(client_socket);
    client_mutex[server].unlock();

    // cout << "서버에 입장시켰음" << endl;

    while (true) {
        valRead = recv(client_socket, cBuffer, BUFFER_SIZE, 0);

        // 아직 닉네임이 없는 상태라면, 닉네임을 설정한 것이니 적용하고 환영 메세지 출력
        if (valRead > 0 && client_name == "") {
            cBuffer[valRead] = '\0';
            client_name = cBuffer;
            
            client_mutex[server].lock();
            int users_count = server_clients[server].size();
            client_mutex[server].unlock();
            
            cout << "\n<공지> " + client_name + " 님이 " << server + 1 << "서버에 입장하셨습니다" << endl;
            cout << "       현재 " << server + 1 << "서버 접속자 수는 " + to_string(users_count) + "명입니다" << endl << endl;
            sendMsg(client_socket, "<공지> 채팅 서버에 오신 것을 환영합니다.\n       채팅을 종료하려면 \"/퇴장\" 혹은 \"/exit\" 이라 입력해주세요\n");
            broadCastMessage("\n<공지> " + client_name + " 님이 채팅에 입장하셨습니다\n       현재 접속자 수는 " + to_string(users_count) + "명입니다\n", server, INVALID_SOCKET);
        }

        // recv의 결과가 0이라면, 정상적으로 클라이언트 단에서 종료 된 것이다.
        else if (valRead == 0) {
            client_mutex[server].lock();
            int users_count = server_clients[server].size();
            client_mutex[server].unlock();

            broadCastMessage("\n<공지> " + client_name + " 님이 채팅을 떠나셨습니다", server, INVALID_SOCKET);
            broadCastMessage("       현재 접속자 수는 " + to_string(users_count - 1) + " 명입니다.", server, INVALID_SOCKET);
            cout << "\n<공지> " + client_name + " 님이 채팅을 떠나셨습니다" << endl;
            cout << "       현재 " << server + 1 << "서버 접속자 수는 " + to_string(users_count- 1) + " 명입니다." << endl;
            
            break;
        }

        // recv의 결과가 0이상이라면, 메세지를 수신한 것이니 모든 클라이언트에게 전송한다.
        else if (valRead > 0) {
            cBuffer[valRead] = '\0';
            cout << client_name << " : " << cBuffer << endl;
            broadCastMessage(client_name + " : " + string(cBuffer), server, client_socket);
        }

        // recv가 음수라면, 에러가 생긴 것이니 연결이 끊어졌음을 표시한다.
        else {
            // 유저가 접속하려다 닉네임을 짓지 않고 종료하면 생기는 메세지 방지
            if (client_name == "") {}

            // 실제로 유저가 접속을 종료하면 메세지 출력
            else {
                client_mutex[server].lock();
                int users_count = server_clients[server].size();
                client_mutex[server].unlock();  

                // 접속이 끊어진 채팅 출력
                broadCastMessage("\n<공지> " + client_name + " 님의 연결이 유실되었습니다.", server, INVALID_SOCKET);
                broadCastMessage("       현재 접속자 수는 " + to_string(users_count - 1) + " 명입니다.", server, INVALID_SOCKET);

                cout << "\n<공지> " + client_name + " 님의 연결이 유실되었습니다." << endl;
                cout << "       현재 접속자 수는 " + to_string(users_count - 1) + " 명입니다." << endl;
            }
            break;
        }
    }

    // 연결이 종료된다면, 벡터에서 삭제한다.
    client_mutex[server].lock();
    server_clients[server].erase(remove(server_clients[server].begin(), server_clients[server].end(), client_socket), server_clients[server].end());
    client_mutex[server].unlock();
    closesocket(client_socket);
}


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
    SOCKADDR_IN server_addr = {};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = PORT;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    
    /*  
        소켓을 sockaddr에 바인딩하고, listen상태로 만든다.
        
        bind(소켓, 소켓 구성요소 구조체의 주소, 구조체의 크기)
        listen(소켓, 최대 접속중인 수)
    */
    if (bind(server_socket, (SOCKADDR*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
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
        SOCKET hClient = accept(server_socket, (SOCKADDR*)&tClntAddr, &iClntSize);

        // 잘 연결되지 않았다면, 위의 루프를 다시 진행
        if (hClient == INVALID_SOCKET) {
            closesocket(hClient);
            continue;
        }

        thread client_thread(handle_client, hClient);
        // 많은 유저를 while에서 받기 때문에 detach해서 알아서 실행되도록 한다.
        client_thread.detach();
    }

    /*
        버퍼를 생성하고 메세지를 받고, 메세지를 또 보낸다

        recv에는 받을 소켓, 메세지를 받을 공간, 그 공간의 크기, flag를 넣는다.
        flag는 별도로 활성화하지 않을 것이므로 쓰지 않음

        send는 보낼 소켓, 보낼 메세지, 길이, flag를 넣는다.
    */

    WSACleanup();
    system("pause");
    return 0;
}