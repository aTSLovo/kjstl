#include <sys/types.h>
#include <unistd.h>
#include <iostream>
int main() {
    std::cout << "父进程pid: " << getpid() << '\n';
    pid_t pid = fork();
    std::cout << "子进程pid: " << pid << '\n';
    return 0;
}