#include "socketUtils.h"

using namespace std;

// ====================================================================================================================================
// 서버 소켓을 만들고 반환한다.
// ====================================================================================================================================
SOCKET socketUtils::openSocket() {
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
    SOCKET serverSocket;
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET) {
        cerr << "소켓 생성 실패 : " << WSAGetLastError() << endl;
        closesocket(serverSocket);
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

    return serverSocket;
}

// ====================================================================================================================================
// 새로운 클라이언트를 처리하기 위한 함수
// ====================================================================================================================================
void socketUtils::handleClient(SOCKET client_socket) {
    // 수령한 메세지를 저장하기 위한 변수
    char cBuffer[BUFFER_SIZE] = "";

    while (true) {
        // 받은 메세지를 분리한다.
        int recv_size = recv(client_socket, cBuffer, BUFFER_SIZE - 1, 0);
        
        // 받은 메세지 크기를 잘라줘야만 원활히 출력된다
        cBuffer[recv_size] = '\0';

        // 메세지를 자른다.
        pair<string, string> msg_splited = utils::decodeMessage(cBuffer);

        // 메세지 종류에 따라 적당한 동작을 수행한다!
        if (msg_splited.first == "login") {
            socketUtils::user_login(client_socket, msg_splited.second);
        }
    }
}

// ====================================================================================================================================
// LOGIN
// login → “login id|pw” 와 같은 형식으로 전송되며, 받은 메세지를 기반으로 DB조회를 통해 로그인을 수행한다
// ====================================================================================================================================
void socketUtils::user_login(SOCKET client_socket, string msg) {
    // 일단 |기준으로 메세지를 나눈다
    string id, pwd;
    int split_pos = msg.find('|');

    // 나눠지지 않으면 오류 출력
    if (split_pos == string::npos) throw runtime_error("아이디, 비밀번호 분류 실패");
    
    id = msg.substr(0, split_pos);
    pwd = msg.substr(split_pos + 1, msg.length() - split_pos);

    try {
        // 아이디, 비밀번호를 찾아 중복체크를 수행한다.
        userInfo tmp = mysql::getUserinfoByID(id);

        // 아이디가 존재한다면, 비밀번호를 비교한다.
        if (id == tmp.id && pwd == tmp.pwd) {
            // 모두 동일하다면, 로그인을 성공시킨다.
            
        }
    }

    // 입력한 아이디가 없다면, 에러메세지 반환 후 값 받는 상태로 돌아간다.
    catch (NoIDException e) {
        sendMessage(client_socket, e.what());
        handleClient(client_socket);
    }


}

// ====================================================================================================================================
// 특정 클라이언트에게 메세지를 보내는 함수
// ====================================================================================================================================
void socketUtils::sendMessage(SOCKET client_socket, string msg) {
    send(client_socket, msg.c_str(), msg.length(), 0);
}


// ====================================================================================================================================
// 새 유저를 받아 sessionMap에 추가해주는 함수
// ====================================================================================================================================
void socketUtils::addNewSession(SOCKET client_socket) {
    // 새 유저가 들어오면, sessionMap에 값을 넣는다
    socketUtils::sessionMutex.lock();
    socketUtils::sessionMap.insert({ client_socket, "newUser" });
    socketUtils::sessionMutex.unlock();

    // 등록 완료했다면, handleClient에 넘긴다!
    handleClient(client_socket);
}

// ====================================================================================================================================
// 어떤 메세지를 받았을 때, 메세지의 prefix와 본문으로 분리해주는 함수
// ====================================================================================================================================
pair<string, string> utils::decodeMessage(string recv) {
    string prefix, message;

    // prefix는 첫 공백까지이므로, 첫 공백을 찾는다.
    int space_pos = recv.find(' ');
    // 공백이 없다면 오류를 발생시킨다.
    if (space_pos == string::npos) throw runtime_error("메세지 디코딩 실패");

    prefix = recv.substr(0, space_pos);
    message = recv.substr(space_pos + 1, recv.length() - space_pos);

    return { prefix, message };
}