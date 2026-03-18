#include <bits/stdc++.h>
using namespace std;
using namespace placeholders;
// using std::cin;
// using std::string;
// using std::cout;
template<class T>
T testArgs(T args) {
    std::cout << __PRETTY_FUNCTION__ << "\n";
    return args;
}
template<class T, class... Args>
T testArgs(T one, Args... two) {
    std::cout << __PRETTY_FUNCTION__ << "\n";
    return one + testArgs(two...);
}
int main() {
    std::cout << testArgs(1, 2, 3, 4, 5) << '\n';
    return 0;
}