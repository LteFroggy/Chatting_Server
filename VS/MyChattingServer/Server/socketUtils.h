#pragma once

#include <ws2tcpip.h>
#include <WinSock2.h>
#include <iostream>

using namespace std;

#define PORT 13542
#define BUFFER_SIZE 1024

#pragma comment(lib, "ws2_32")
#pragma once

#define PORT 13542
#define BUFFER_SIZE 1024

SOCKET makeSocket();