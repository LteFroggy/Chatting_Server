#include <string>
#include <vector>
#include <iostream>
#include <ws2tcpip.h> // inet_pton ����� ���� ��� �߰�
#include <WinSock2.h>

#pragma comment(lib, "ws2_32")

using namespace std;

#define PORT 13542
#define BUFFER_SIZE 1024
#define SERVER_IP "127.0.0.1"

/*
    Ŭ���̾�Ʈ�� ������ �����ϰ�, ������ ��û�Ѵ�.

    ���� �Ŀ��� �پ��� �޼����� ������, �޼����� ����� ���� ������ �׿� �ɸ´� ��û�� ó���Ѵ�.
    ��û ó�� �ÿ��� �� ������ �����ϴ� ��Ȳ���� Ȯ���ؾ� ��.

    ### ä�� ��� ����

    Client���� ������ �޼����� � ����� �����ϱ� ���� �޼��������� �����ϱ� ����, ����� ���� �����Ѵ�
    - login �� ��login id/pw�� �� ���� �������� ���۵Ǹ�, ���� �޼����� ������� DB��ȸ�� ���� �α����� �����Ѵ�
    - join �� ��join id/pw���� ���� �������� ���۵ǰ�, ������ �̸� ������� DB�� ����Ѵ�. ID�ߺ�Ȯ���� �ʿ�
    - chat �� ��chat msg�� �� ���� �������� ���۵Ǹ�, �޼����� ����
    - gamechat �� ��gamechat msg�� �� ���� �������� ���۵Ǹ�, �����ձ� ������ �����Ѵ�
    - whisper �� ��whisper target msg���� ���� �������� ���۵ǰ�, Ÿ�ٿ��� �ӼӸ��� �ش�
    - quit 
*/

int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);


    // Ŭ���̾�Ʈ�� ������ �����Ѵ�.
    SOCKET clientSocket;
    clientSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) {
        cout << "Ŭ���̾�Ʈ ���� ���� ����" << endl;
        return -1;
    }

    SOCKADDR_IN serverAddr = {}; // ���� �ּ� ������ ������ ����ü �ʱ�ȭ
    serverAddr.sin_family = PF_INET; // IPv4 �ּ� ü�踦 ���
    serverAddr.sin_port = htons(PORT); // ��Ʈ ��ȣ�� ��Ʈ��ũ ����Ʈ ������ ��ȯ �� �Ҵ�
    inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr);


    // ���� ������ �̿��� ������ �����Ѵ�.
    if (connect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cout << "Connection failed : " << WSAGetLastError() << endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    // �޼����� �ϳ� ������ �����Ѵ�
    string message = "Hello Server! I'm Client";
    if (send(clientSocket, message.c_str(), message.length(), 0) == SOCKET_ERROR) {
        cout << "�޼��� ���ۿ� �����Ͽ����ϴ�" << endl;
    }
    else {
        cout << "�޼��� ���� ����!" << endl;
    }

    WSACleanup();
    return 0;
}