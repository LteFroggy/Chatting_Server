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
 // �޼��� �ص��� ���� split�ϴ� �����Լ�
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