#include "Protocol.h"

// ====================================================================================================================================
// Setter, Getter
// ====================================================================================================================================
messageCode msg_format::getCode() {
	return code;
}
void msg_format::setCode(messageCode input) {
	code = input;
}

string msg_format::getMsg() {
	return encodedMessage;
}

void msg_format::setMsg(string message) {
	encodedMessage = message;
}

// ====================================================================================================================================
 // 메세지 해독을 위해 split하는 내장함수
// ====================================================================================================================================
vector<string> msg_format::split(string target, char deli) {
	stringstream ss(target);
	vector<string> result;
	string tmp;

	while (getline(ss, tmp, deli)) {
		result.push_back(tmp);
	}

	return result;
}