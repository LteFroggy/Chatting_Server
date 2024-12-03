#include <iostream>
#include <string>
#include <algorithm>
#include <vector>

using namespace std;

string firstScreen(void) {
    system("cls");
    string nickName;
    cout <<
    "\t 채팅 서버에 연결되었습니다!\n"
    "\t 서버에서 사용할 닉네임을 입력해주세요 : ";
    getline(cin, nickName);
    nickName[nickName.length()] = '\0';
    
    return nickName;
}


int main() {
    vector<int> vec = {1, 2, 3, 4, 5};

    for (auto v : vec) cout << v << " ";
    cout << endl;

    vec.erase(remove(vec.begin(), vec.end(), 3), vec.end());

    string nickName = firstScreen();
    cout << nickName << endl;

    return 0;
}