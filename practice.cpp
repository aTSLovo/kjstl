#include <bits/stdc++.h>
using namespace std;
using namespace placeholders;
// using std::cin;
// using std::string;
// using std::cout;

bool check_size(const int &x, string::size_type sz) {
    return x > sz;
}
int main() {
    vector<string> words = { "aaaa","bbbbb","cccccc","ddddddd" };
    vector<int> a = {1, 3, 6, 7, 8};
	string::size_type sz= 6;
    
    auto wc = find_if(a.begin(), a.end(), bind(check_size, _1, words[0].size()));
    cout << *wc << endl;
    // cout << count_if(words.begin(), words.end(), bind(check_size, _1, sz)) << endl;
    return 0;
}