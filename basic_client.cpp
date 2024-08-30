#include <thread>
#include <mutex>
#include <string>
#include <vector>
#include <iostream>
#include <ws2tcpip.h>
#include <WinSock2.h>
// #include <sys/socket.h>

#pragma comment(lib, "ws2_32")

using namespace std;

#define PORT 13542
#define BUFFER_SIZE 1024
#define SERVER_IP "210.178.8.62"

/*
    클라이언트는 소켓을 생성하고, 연결을 요청한다.
*/

// 서버에서 받은 메세지들과 내가 보낸 매세지를 저장하는 벡터
vector<string> sMsg;
mutex msg_mutex;
string userName;


// 서버에 접속되었음과 닉네임 설정을 수행하는 함수
void firstScreen(void) {
    system("cls");
    cout << "\n"
    "## 채팅 서버에 연결되었습니다! ##\n"
    "## 서버에서 사용할 닉네임을 입력해주세요 : ";
    getline(cin, userName);
    while (userName.length() > 12 || userName.length() < 2) {
        cout << "## 이름이 너무 짧거나 깁니다. 다시 입력해주세요 : ";
        getline(cin, userName);
    }

    userName[userName.length()] = '\0';
}

// 채팅 화면을 구성해주는 함수. 화면을 cls한 후 받은 채팅을 모두 보여주고 내 입력창을 밑에 둔다.
void chattingScreen(void) {
    system("cls");

    cout << "\n";
    msg_mutex.lock();
    for (auto v : sMsg) {
        cout << v << endl;
    }
    msg_mutex.unlock();
    cout << "ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ" << endl;
    cout << endl << userName << " : ";

    // 버퍼에 값이 남아있다면(입력하던 메세지가 있었다면) 값 그대로 주기
    // string remain_input;
    // getline(cin, remain_input);
    // cout << remain_input;
}

void endedScreen(int code) {
    system("cls");

    cout << "\n";
    msg_mutex.lock();
    for (auto v : sMsg) {
        cout << v << endl;
    }
    msg_mutex.unlock();
    cout << "ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ" << endl;
    cout << "서버와의 연결이 종료되었습니다 (" << code << ")" << endl;
    cout << "종료하려면 엔터를 눌러주세요" << endl;
}

// 메세지를 전송하기 위한 함수
void send_to_server(SOCKET client_socket) {
    string message;
    while (true) {
        message = "";
        // cout << userName << " : ";
        getline(cin, message);

        if (message == "/퇴장") {
            endedScreen(0);
            break;
        }

        else if (send(client_socket, message.c_str(), message.length(), 0) == SOCKET_ERROR) {
            // cout << "메세지 전송 실패" << endl;
            endedScreen(1);
            break;
        }

        msg_mutex.lock();
        sMsg.push_back(userName + " : " + message);
        msg_mutex.unlock();

        chattingScreen();
    }

    // 여기서 socket을 닫으면, 자연스럽게 recv함수도 소켓이 사라져 같이 끝난다.
    closesocket(client_socket);
}

void recv_from_server(SOCKET client_socket) {
    char cBuffer[BUFFER_SIZE];
    int recv_result;

    while (true) {
        recv_result = recv(client_socket, cBuffer, BUFFER_SIZE, 0);
        if (recv_result > 0) {
            cBuffer[recv_result] = '\0';

            msg_mutex.lock();
            sMsg.push_back(cBuffer);
            msg_mutex.unlock();

            chattingScreen();
        }

        else if (recv_result == 0) {
            // cout << "Server closed" << endl;
            endedScreen(3);
            break;
        }

        else {
            // cout << "서버와의 통신 종료됨" << endl;
            endedScreen(4);
            break;
        }
    }
}

int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET clientSocket;
    clientSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) {
        cout << "클라이언트 소켓 생성 실패" << endl;
        return -1;
    }

    SOCKADDR_IN serverAddr = {};
    serverAddr.sin_family = PF_INET;
    serverAddr.sin_port = PORT;
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

    if (connect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cout << "Connection failed : " << WSAGetLastError() << endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }
    
    // 처음 접속하여 유저명을 정하는 부분
    firstScreen();
    int sendResult = send(clientSocket, userName.c_str(), userName.length(), 0);
    if (sendResult == SOCKET_ERROR) {
        cout << "연결에 실패하였습니다" << endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    // 닉네임 설정을 완료했으면, 서버의 환영 메세지를 받는다.
    // char cBuffer[BUFFER_SIZE];
    // recv(clientSocket, cBuffer, BUFFER_SIZE, 0);
    // sMsg.push_back(cBuffer);
    // cout << cBuffer << endl;

    // 서버로 항상 메세지를 보낼 수도 있고, 받을 수도 있어야 한다.
    // 따라서 2개의 스레드를 사용한다.
    thread send_thread(send_to_server, clientSocket);
    thread receive_thread(recv_from_server, clientSocket);

    send_thread.join();
    receive_thread.join();

    system("pause");
    WSACleanup();
    return 0;
}