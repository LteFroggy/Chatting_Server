#include <thread>
#include <string>
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

// 메세지를 전송하기 위한 함수
void send_to_server(SOCKET client_socket, string userName) {
    string message;
    cout << "채팅을 종료하려면 exit을 입력하세요" << endl;
    while (true) {
        message = "";
        // cout << userName << " : ";
        getline(cin, message);

        if (message == "exit") {
            cout << "채팅을 종료합니다" << endl;
            break;
        }

        else if (send(client_socket, message.c_str(), message.length(), 0) == SOCKET_ERROR) {
            cout << "메세지 전송 실패" << endl;
            break;
        }
    }

    // 여기서 socket을 닫으면, 자연스럽게 recv함수도 소켓이 사라져 같이 끝난다.
    closesocket(client_socket);
}

void recv_from_server(SOCKET client_socket) {
    char cBuffer[BUFFER_SIZE];
    int recv_result;

    while (true) {
        cBuffer[0] = '\0';
        recv_result = recv(client_socket, cBuffer, BUFFER_SIZE, 0);
        if (recv_result > 0) {
            cBuffer[recv_result] = '\0';
            cout << cBuffer << endl;
        }

        else if (recv_result == 0) {
            cout << "Server closed" << endl;
            break;
        }

        else {
            cout << "Recv 종료" << endl;
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
        cerr << "클라이언트 소켓 생성 실패" << endl;
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

    string userName;
    
    // 처음 접속하여 유저명을 정하는 부분
    while (true) {
        cout << "채팅에서 사용할 닉네임을 입력해주세요 : ";
        getline(cin, userName);
        // 유저명은 6자 이하로 설정해야 함
        if (userName.length() > 8) {
            cout << "너무 깁니다!" << endl;
            continue;
        }
        userName += '\0';
        int sendResult = send(clientSocket, userName.c_str(), userName.length(), 0);
        if (sendResult == SOCKET_ERROR) {
            cout << "연결에 실패하였습니다" << endl;
            closesocket(clientSocket);
            WSACleanup();
            return 1;
        }
        break;
    }
    
    // 닉네임 설정을 완료했으면, 서버의 환영 메세지를 받는다.
    char cBuffer[BUFFER_SIZE];
    recv(clientSocket, cBuffer, BUFFER_SIZE, 0);
    cout << cBuffer << endl;

    // 서버로 항상 메세지를 보낼 수도 있고, 받을 수도 있어야 한다.
    // 따라서 2개의 스레드를 사용한다.
    thread send_thread(send_to_server, clientSocket, userName);
    thread receive_thread(recv_from_server, clientSocket);

    send_thread.join();
    receive_thread.join();

    system("pause");
    WSACleanup();
    return 0;
}