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

// ������ ��� Client�� �����ϱ� ���� ����
vector<vector<SOCKET>> server_clients(2);

// Client vector�� ��Ʈ���� �� ����ϱ� ���� mutex
vector<mutex> client_mutex(2);

/*
    ������ �⺻������ socket�� ���� Ŭ���̾�Ʈ�� ������ ��ٸ���.

    �׷���, Accept�� �ϳ��� Client�� ���� �� �����Ƿ� ���ο� �����带 �����ؼ� Ŭ���̾�Ʈ ��û�� ó���Ѵ�.
*/

// ��� �����ڿ��� �޼��� ������
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

// Ư�� �����ڿ��� �޼��� ������
void sendMsg(SOCKET target, string msg) {
    send(target, msg.c_str(), msg.length(), 0);
}

// ���ο� Ŭ���̾�Ʈ�� ó���ϱ� ���� �Լ�
void handle_client(SOCKET client_socket) {
    char cBuffer[BUFFER_SIZE];
    string client_name = "";
    int server(0);

    // ���� � ä�ο� �������� ���ϰ� �ϵ��� �ϱ� ���� ������ �ο����� �����Ѵ�
    sendMsg(client_socket, "���� 1 �ο��� : " + to_string(server_clients[0].size()) + "\n���� 2 �ο��� : " + to_string(server_clients[1].size()) + "\n������ ������ �����ϼ��� (1 or 2)");
        
    int valRead = recv(client_socket, cBuffer, BUFFER_SIZE, 0);
    // cout << "�Է� ������ ���� �Ϸ�" << endl;
    cBuffer[valRead] = '\0';

    if (cBuffer == "1") server = 0;
    if (cBuffer == "2") server = 1;

    // �� ����� ���� ������ �����Ų��.
    client_mutex[server].lock();
    server_clients[server].push_back(client_socket);
    client_mutex[server].unlock();

    // cout << "������ ���������" << endl;

    while (true) {
        valRead = recv(client_socket, cBuffer, BUFFER_SIZE, 0);

        // ���� �г����� ���� ���¶��, �г����� ������ ���̴� �����ϰ� ȯ�� �޼��� ���
        if (valRead > 0 && client_name == "") {
            cBuffer[valRead] = '\0';
            client_name = cBuffer;
            
            client_mutex[server].lock();
            int users_count = server_clients[server].size();
            client_mutex[server].unlock();
            
            cout << "\n<����> " + client_name + " ���� " << server + 1 << "������ �����ϼ̽��ϴ�" << endl;
            cout << "       ���� " << server + 1 << "���� ������ ���� " + to_string(users_count) + "���Դϴ�" << endl << endl;
            sendMsg(client_socket, "<����> ä�� ������ ���� ���� ȯ���մϴ�.\n       ä���� �����Ϸ��� \"/����\" Ȥ�� \"/exit\" �̶� �Է����ּ���\n");
            broadCastMessage("\n<����> " + client_name + " ���� ä�ÿ� �����ϼ̽��ϴ�\n       ���� ������ ���� " + to_string(users_count) + "���Դϴ�\n", server, INVALID_SOCKET);
        }

        // recv�� ����� 0�̶��, ���������� Ŭ���̾�Ʈ �ܿ��� ���� �� ���̴�.
        else if (valRead == 0) {
            client_mutex[server].lock();
            int users_count = server_clients[server].size();
            client_mutex[server].unlock();

            broadCastMessage("\n<����> " + client_name + " ���� ä���� �����̽��ϴ�", server, INVALID_SOCKET);
            broadCastMessage("       ���� ������ ���� " + to_string(users_count - 1) + " ���Դϴ�.", server, INVALID_SOCKET);
            cout << "\n<����> " + client_name + " ���� ä���� �����̽��ϴ�" << endl;
            cout << "       ���� " << server + 1 << "���� ������ ���� " + to_string(users_count- 1) + " ���Դϴ�." << endl;
            
            break;
        }

        // recv�� ����� 0�̻��̶��, �޼����� ������ ���̴� ��� Ŭ���̾�Ʈ���� �����Ѵ�.
        else if (valRead > 0) {
            cBuffer[valRead] = '\0';
            cout << client_name << " : " << cBuffer << endl;
            broadCastMessage(client_name + " : " + string(cBuffer), server, client_socket);
        }

        // recv�� �������, ������ ���� ���̴� ������ ���������� ǥ���Ѵ�.
        else {
            // ������ �����Ϸ��� �г����� ���� �ʰ� �����ϸ� ����� �޼��� ����
            if (client_name == "") {}

            // ������ ������ ������ �����ϸ� �޼��� ���
            else {
                client_mutex[server].lock();
                int users_count = server_clients[server].size();
                client_mutex[server].unlock();  

                // ������ ������ ä�� ���
                broadCastMessage("\n<����> " + client_name + " ���� ������ ���ǵǾ����ϴ�.", server, INVALID_SOCKET);
                broadCastMessage("       ���� ������ ���� " + to_string(users_count - 1) + " ���Դϴ�.", server, INVALID_SOCKET);

                cout << "\n<����> " + client_name + " ���� ������ ���ǵǾ����ϴ�." << endl;
                cout << "       ���� ������ ���� " + to_string(users_count - 1) + " ���Դϴ�." << endl;
            }
            break;
        }
    }

    // ������ ����ȴٸ�, ���Ϳ��� �����Ѵ�.
    client_mutex[server].lock();
    server_clients[server].erase(remove(server_clients[server].begin(), server_clients[server].end(), client_socket), server_clients[server].end());
    client_mutex[server].unlock();
    closesocket(client_socket);
}


int main() {
    WSADATA wsaData;
    /*
        WinSock �ʱ�ȭ�� ���� �ڵ�

        WSADATA�� Windows�� ���� �ʱ�ȭ ���� ������ ���� ����ü�̴�.
        WSAStartup(���� ����, WSADATRA ����ü �ּ�)
            ���� ���� ȣ���ؾ� �ϴ� �Լ���, � ������ Windows�� �˷��ִ� ��.
            ���� �������� 2.2�� ����� ���̰�, WORD type���� ����.
            WORD�� unsigned intŸ���ε�, 2.2�� double���̹Ƿ� MAKEWORD�� �̿��� 2.2�� ����� �����.
            2��°���� WSADATA�� �����͸� ����� �ִ´�.
    */
    int iResult;
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        cerr << "WSAStartup Failed : " << iResult << endl;
        return -1;
    }

    /*
        ���� ���� �����ϱ�

        socket(int domain, int type, int protocol)

        domain
            AF_INET : IPv4 ���
            AF_INET6 : IPv6 ���

        type
            SOCK_STREAM : ����������, �ŷڼ��ִ� ��� ����, ������� �����ϸ� ��谡 ����. (TCP)
            SOCK_DGRAM : �񿬰�������, �ŷڼ����� �ӵ��� �߿���ϸ�, ��Ŷ ������ �����͸� �����Ѵ�. (UDP)

        protocol -> �⺻������ Domain, Type�� ���� �ڵ����� �����ǳ�, ����ϰ� ���� �� ���
            IPROTO_TCP : TCP�� ����ϰڴٰ� ����
            IRPOTO_UDP : UDP�� ����ϰڴٴ� �ǹ�
    */
    SOCKET server_socket;
    server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_socket == INVALID_SOCKET) {
        cerr << "���� ���� ���� : " << WSAGetLastError() << endl;
        closesocket(server_socket);
        WSACleanup();
        system("pause");
        return -1;
    }

    /*
        ������ ������Ҹ� ���� ����ü ���� �� �� �Ҵ�

        ���� �����  ������ ������Ҹ� �����Ͽ� �ּ������� �����.
        �ּҴ� IP�� PORT�� �� ���� ��Ұ� �ִ�.

        SOCKADDR_IN�� Windows���Ͽ��� ������ ������ ���� �� ���� �ּҸ� �����ϴ� ���� ���ȴ�.
        ��, �ּ� ������ ��Ƶα� ���� ����ü�̴�.

        sin_family : �ݵ�� AF_INET�̾�� �Ѵ�.
        sin_port : ��Ʈ ��ȣ�� �����Ѵ�. 2byte������ ǥ���� �����ؾ� �ϸ�, �⺻ ��Ʈ��ȣ�� ������ �ٸ� ��ȣ�� ��� �Ѵ�.
            htons, htonl : Host to Network�� ���ڷ�, �� �Լ��� ����ϸ� �򿣵��(�����͸� �޸𸮿� �տ������� ����) ������� �����͸� ��ȯ�� ����Ѵ�.
        sin_addr : IP�ּ� �����ϱ�
            s_addr : IPv4�� �ǹ��Ѵ�.
            INADDR_ANY�� ���� �������� ��ǻ���� IP �ּҸ� �ǹ��Ѵ�.

    */
    SOCKADDR_IN server_addr = {};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = PORT;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    
    /*  
        ������ sockaddr�� ���ε��ϰ�, listen���·� �����.
        
        bind(����, ���� ������� ����ü�� �ּ�, ����ü�� ũ��)
        listen(����, �ִ� �������� ��)
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

    cout << "Ŭ���̾�Ʈ ���� ��� ��" << endl;

    /*
        ������ Accept�Ѵ�.

        accept�� ����ȭ�� ������� �����ϴµ�, �̴� ��û�� �������ϱ� ������ wait���°� �Ǵ� ���� �ǹ��Ѵ�.
        ������ �����ϸ� ����� ������ ���������.
        ���ڷδ� ����, accept�� Ŭ���̾�Ʈ�� �������� ����ü, �� ũ�Ⱑ ����.
    */
    while (true) {
        // sockaddr�� socket�� �����, accept�� �����Ѵ�.
        SOCKADDR_IN tClntAddr = {};
        int iClntSize = sizeof(tClntAddr);
        SOCKET hClient = accept(server_socket, (SOCKADDR*)&tClntAddr, &iClntSize);

        // �� ������� �ʾҴٸ�, ���� ������ �ٽ� ����
        if (hClient == INVALID_SOCKET) {
            closesocket(hClient);
            continue;
        }

        thread client_thread(handle_client, hClient);
        // ���� ������ while���� �ޱ� ������ detach�ؼ� �˾Ƽ� ����ǵ��� �Ѵ�.
        client_thread.detach();
    }

    /*
        ���۸� �����ϰ� �޼����� �ް�, �޼����� �� ������

        recv���� ���� ����, �޼����� ���� ����, �� ������ ũ��, flag�� �ִ´�.
        flag�� ������ Ȱ��ȭ���� ���� ���̹Ƿ� ���� ����

        send�� ���� ����, ���� �޼���, ����, flag�� �ִ´�.
    */

    WSACleanup();
    system("pause");
    return 0;
}