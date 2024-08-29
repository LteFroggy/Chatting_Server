#include <iostream>
#include <ws2tcpip.h>
#include <WinSock2.h>
// #include <sys/socket.h>

#pragma comment(lib, "ws2_32")

using namespace std;

#define PORT 13542
#define PACKET_SIZE 1024
#define SERVER_IP "127.0.0.1"

/*
    클라이언트는 소켓을 생성하고, 연결을 요청한다.
*/

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

    cout << "Connected To Server!" << endl;
    
    char cBuffer[PACKET_SIZE];
    char cMsg[] = "Client Hello!";
    int sendResult = send(clientSocket, cMsg, strlen(cMsg), 0);
    if (sendResult == SOCKET_ERROR) {
        cout << "Send Failed : " << WSAGetLastError() << endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    recv(clientSocket, cBuffer, PACKET_SIZE, 0);
    cout << "Received From Server : " << cBuffer << endl;

    closesocket(clientSocket);
    system("pause");

    WSACleanup();
    return 0;
}