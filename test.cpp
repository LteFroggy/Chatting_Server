#include <iostream>
#include <algorithm>
#include <vector>

using namespace std;

int main() {
    vector<int> vec = {1, 2, 3, 4, 5};

    for (auto v : vec) cout << v << " ";
    cout << endl;

    vec.erase(remove(vec.begin(), vec.end(), 3), vec.end());

    for (auto v : vec) {
        if (v == 2) continue;
        cout << v << " ";
    }
    cout << endl;

    return 0;
}