#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <ws2tcpip.h>
#include <WinSock2.h>
#include <sw/redis++/redis.h>

#pragma comment(lib, "ws2_32")

using namespace std;
using namespace sw::redis;

#define PORT 13542
#define BUFFER_SIZE 1024


/* �̸� ���ǵ� �Լ� ��� */
SOCKET makeSocket();

int main() {
    SOCKET serverSocket = makeSocket();

    /* Redis Test! */
    cout << "�ڵ�� �� �����" << endl;
    
    auto redis = Redis("tcp://127.0.0.1:6379");
    redis.set("myKey", "Hello, Redis++!");
    cout << "Ű-�� ���� �Ϸ�" << endl;
    
    auto result = redis.get("myKey");
    if (result) {
        cout << "������ ����� " << *result << " �Դϴ�" << endl;
    }
    else {
        cout << "����� �޾ƿ��� ���߽��ϴ�" << endl;
    }
    

    //if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
    //    cerr << "Listen Failed : " << WSAGetLastError() << endl;
    //    closesocket(serverSocket);
    //    WSACleanup();
    //    system("pause");
    //    return 2;
    //}

    //cout << "Ŭ���̾�Ʈ ���� ��� ��" << endl;

    ///*
    //    ������ Accept�Ѵ�.

    //    accept�� ����ȭ�� ������� �����ϴµ�, �̴� ��û�� �������ϱ� ������ wait���°� �Ǵ� ���� �ǹ��Ѵ�.
    //    ������ �����ϸ� ����� ������ ���������.
    //    ���ڷδ� ����, accept�� Ŭ���̾�Ʈ�� �������� ����ü, �� ũ�Ⱑ ����.
    //*/
    //while (true) {
    //    // sockaddr�� socket�� �����, accept�� �����Ѵ�.
    //    SOCKADDR_IN tClntAddr = {};
    //    int iClntSize = sizeof(tClntAddr);
    //    SOCKET hClient = accept(serverSocket, (SOCKADDR*)&tClntAddr, &iClntSize);

    //    // �� ������� �ʾҴٸ�, ���� ������ �ٽ� ����
    //    if (hClient == INVALID_SOCKET) {
    //        closesocket(hClient);
    //        continue;
    //    }

    //    else {
    //        cout << "Ŭ���̾�Ʈ ���ӵ�" << endl;
    //        char buffer[BUFFER_SIZE];

    //        // �ϴ� �޼��� �ϳ��� �ް� �ٷ� ����������� ����
    //        int recv_size = recv(hClient, buffer, BUFFER_SIZE - 1, 0);
    //        // ���� �޼��� ũ�⸦ �߶���߸� ��Ȱ�� ��µȴ�
    //        buffer[recv_size] = '\0';

    //        cout << buffer << endl;
    //        break;
    //    }
    //}


    //WSACleanup();
    return 0;
}


/*
    ������ ������ �����ϰ�, ��ȯ�Ѵ�.
*/
SOCKET makeSocket() {
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
    SOCKET serverSocket;
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET) {
        cerr << "���� ���� ���� : " << WSAGetLastError() << endl;
        closesocket(serverSocket);
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
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    /*
        ������ sockaddr�� ���ε��ϰ�, listen���·� �����.

        bind(����, ���� ������� ����ü�� �ּ�, ����ü�� ũ��)
        listen(����, �ִ� �������� ��)
    */
    bind(serverSocket, (SOCKADDR*)&server_addr, sizeof(server_addr));

    /* hiredis�� ���� ������ ���ܼ� �ּ�ó�� */
    //if (bind(serverSocket, (SOCKADDR*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
    //    cerr << "Bind Failed : " << WSAGetLastError() << endl;
    //    closesocket(serverSocket);
    //    WSACleanup();
    //    system("pause");
    //    return 1;
    //}
    
    return serverSocket;
}