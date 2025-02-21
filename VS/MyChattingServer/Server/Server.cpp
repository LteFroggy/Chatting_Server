#include "server.h"

// ====================================================================================================================================
// 서버의 인스턴스를 받아오는 함수
// ====================================================================================================================================
Server* Server::getInstance() {
	if (instance == nullptr) {
		lock_guard<mutex> lock(mtx);
		instance = new Server()
	}
	return instance;
}


// ====================================================================================================================================
// 서버의 소켓을 세팅한다
// ====================================================================================================================================
void Server::openSocket() {
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
        throw ServerException("WSAStartup Failed");
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

    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET) {
        cerr << "소켓 생성 실패 : " << WSAGetLastError() << endl;
        closesocket(serverSocket);
        WSACleanup();
        system("pause");
        ServerException("Socket open failed");
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
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    /*
        소켓을 sockaddr에 바인딩하고, listen상태로 만든다.

        bind(소켓, 소켓 구성요소 구조체의 주소, 구조체의 크기)
        listen(소켓, 최대 접속중인 수)
    */
    bind(serverSocket, (SOCKADDR*)&server_addr, sizeof(server_addr));

    /* hiredis로 인해 오류가 생겨서 주석처리 */
    //if (bind(serverSocket, (SOCKADDR*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
    //    cerr << "Bind Failed : " << WSAGetLastError() << endl;
    //    closesocket(serverSocket);
    //    WSACleanup();
    //    system("pause");
    //    return 1;
    //}
}


// ====================================================================================================================================
// User의 List를 확인한다.
// ====================================================================================================================================
unordered_map<string, clientHandler> Server::getUserList() {
	return userMap;
}


// ====================================================================================================================================
// 특정 유저의 소켓 번호를 가져온다! (게임 내 귓속말 기능 구현을 위함)
// ====================================================================================================================================
clientHandler& Server::getUserSocketNum(string nickname) {
    auto it = userMap.find(nickname);
    if (it != userMap.end()) {
        return it->second;
    }
}