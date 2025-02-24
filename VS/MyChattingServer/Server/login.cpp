#include "login.h"


// ====================================================================================================================================
// Setter, Getter
// ====================================================================================================================================
void loginRequest::setId(string id) {
	id = id;
}

string loginRequest::getId() {
	return id;
}

void loginRequest::setPwd(string pwd) {
	pwd = pwd;
}

string loginRequest::getPwd() {
	return pwd;
}

// ====================================================================================================================================
// 현재 가진 정보를 바탕으로 메세지를 보내기 위한 string으로 만들어 반환한다.
// ====================================================================================================================================
string loginRequest::encodeMessage() {
	string msg = "";
	msg += this->getCode();
	msg += "|";
	msg += this->getId();
	msg += "|";
	msg += this->getPwd();

	return msg;
}

// ====================================================================================================================================
// string을 잘라서 순서대로 넣어서 디코딩한다.
// ====================================================================================================================================
void loginRequest::decodeMessage() {
	vector<string> result = msg_format::split(getMsg());
	
	setCode(static_cast<messageCode>(stoi(result[0])));
	setId(result[1]);
	setPwd(result[2]);
}

// ====================================================================================================================================
// 해당 메세지를 처리한다. 
// ====================================================================================================================================
optional<msg_format*> loginRequest::processMessage() {
	// 먼저 데이터베이스에서 아아디, 비밀번호를 검색한다.
	mysql* db = mysql::getInstance();
	try {
		userInfo info = db->getLoginInfo(id);

		// 아이디, 비밀번호를 확인하고, 일치하는지 체크하여 반환
		if (pwd == info.pwd) {
			return new loginResponse(LOGIN_RESPONSE, SUCCESS, info.nickname, "로그인에 성공하였습니다.");
		}

		// 일치하지 않는다면, wrong_pwd 반환
		else {
			return new loginResponse(LOGIN_RESPONSE, WRONG_PWD, "", "비밀번호가 일치하지 않습니다");
		}
	}
	catch (NoIDException e) {
		// 아이디, 비밀번호가 없다면 Response로 아이디 없음을 돌려보낸다.
		return new loginResponse(LOGIN_RESPONSE, NO_ID, "", "올바르지 않은 아이디입니다");
	}
	catch (MySQLException e) {
		// DB에서 오류가 났다면 에러 전송
		cerr << "MySQL 오류입니다." << endl;
		cout << e.what() << endl;
		return new loginResponse(LOGIN_RESPONSE, SERVER_ERROR, "", e.what());
	}
	
}