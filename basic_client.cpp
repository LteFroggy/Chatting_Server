#include <thread>
#include <mutex>
#include <string>
#include <vector>
#include <iostream>
#include <ws2tcpip.h>
#include <WinSock2.h>

#pragma comment(lib, "ws2_32")

using namespace std;

#define PORT 13542
#define BUFFER_SIZE 1024
#define SERVER_IP "127.0.0.1"

/*
    Ŭ���̾�Ʈ�� ������ �����ϰ�, ������ ��û�Ѵ�.
*/

// �������� ���� �޼������ ���� ���� �ż����� �����ϴ� ����
vector<string> sMsg;
mutex msg_mutex;
string userName;

// ������ ���ӵǾ����� �˸��� ���� ���� �� �г��� ������ �����ϴ� �Լ�
int firstScreen(SOCKET client_socket) {
    system("cls");
    char cBuffer[BUFFER_SIZE];
    string server_select;

    cout << "\n## ä�� ������ ����Ǿ����ϴ�! ##\n";
    int valRead = recv(client_socket, cBuffer, BUFFER_SIZE, 0);
    cBuffer[valRead] = '\0';
    cout << cBuffer << endl;
    getline(cin, server_select);

    while (server_select != "1" && server_select != "2") {
        cout << "�߸��� ���� �Է��ϼ̽��ϴ�. �ٽ� �Է����ּ��� : " << endl;
        getline(cin, server_select);
    }

    // �� �����ߴٸ�, �����Ѵ�
    send(client_socket, server_select.c_str(), server_select.size(), 0);

    cout << "## �������� ����� �г����� �Է����ּ���(2�� �̻�, 8�� ����) : ";

    getline(cin, userName);
    while (userName.length() >= 8 || userName.length() <= 2) {
        cout << "## �̸��� �ʹ� ª�ų� ��ϴ�. �ٽ� �Է����ּ��� : ";
        getline(cin, userName);
    }

    userName[userName.length()] = '\0';

    // ���� �̸��� �����Ѵ�.
    return send(client_socket, userName.c_str(), userName.size(), 0);
}

// ä�� ȭ���� �������ִ� �Լ�. ȭ���� cls�� �� ���� ä���� ��� �����ְ� �� �Է�â�� �ؿ� �д�.
void chattingScreen(void) {
    system("cls");

    cout << "\n";
    msg_mutex.lock();
    for (auto v : sMsg) {
        cout << v << endl;
    }
    msg_mutex.unlock();
    cout << "�ѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤ�" << endl;
    cout << endl << userName << " : ";

    // ���ۿ� ���� �����ִٸ�(�Է��ϴ� �޼����� �־��ٸ�) �� �״�� �ֱ�
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
    if (code == 0) {
        cout << "�ѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤ�" << endl; 
        cout << "�������� ������ ����Ǿ����ϴ� (" << code << ")" << endl;
        cout << "�����Ϸ��� ���͸� �����ּ���" << endl;
    }

    else if (code == 1) {
        cout << "�ѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤ�" << endl; 
        cout << "�������� ������ ���������ϴ�. (" << code << ")" << endl;
        cout << "���͸� ���� �������ּ���" << endl;
    }
}

// �޼����� �����ϱ� ���� �Լ�
void send_to_server(SOCKET client_socket) {
    string message;

    while (true) {
        message = "";
        // cout << userName << " : ";
        getline(cin, message);

        if (message == "/����" || message == "/exit") {
            endedScreen(0);
            break;
        }

        else if (send(client_socket, message.c_str(), message.length(), 0) == SOCKET_ERROR) {
            // cout << "�޼��� ���� ����" << endl;
            endedScreen(1);
            break;
        }

        msg_mutex.lock();
        sMsg.push_back(userName + " : " + message);
        msg_mutex.unlock();

        chattingScreen();
    }

    // ���⼭ socket�� ������, �ڿ������� recv�Լ��� ������ ����� ���� ������.
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
            // cout << "�������� ��� �����" << endl;
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
        cout << "Ŭ���̾�Ʈ ���� ���� ����" << endl;
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
    
    // ó�� �����Ͽ� ä�� �� �������� �����Ѵ�.
    int sendResult = firstScreen(clientSocket);
    if (sendResult == SOCKET_ERROR) {
        cout << "���ῡ �����Ͽ����ϴ�" << endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    // �г��� ������ �Ϸ�������, ������ ȯ�� �޼����� �޴´�.
    // char cBuffer[BUFFER_SIZE];
    // recv(clientSocket, cBuffer, BUFFER_SIZE, 0);
    // sMsg.push_back(cBuffer);
    // cout << cBuffer << endl;

    // ������ �׻� �޼����� ���� ���� �ְ�, ���� ���� �־�� �Ѵ�.
    // ���� 2���� �����带 ����Ѵ�.
    thread send_thread(send_to_server, clientSocket);
    thread receive_thread(recv_from_server, clientSocket);

    send_thread.join();
    receive_thread.join();

    system("pause");
    WSACleanup();
    return 0;
}