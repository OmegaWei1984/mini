#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "log.hpp"

#define PATH "/tmp/udstest"

int main(void)
{
    int listenFd = socket(AF_LOCAL, SOCK_STREAM, 0);
    unlink(PATH);
    sockaddr_un srvaddr;
    srvaddr.sun_family = AF_LOCAL;
    strcpy(srvaddr.sun_path, PATH);
    int res = bind(listenFd, (sockaddr *) &srvaddr, sizeof(srvaddr));
    if (res < 0) {
        errorOutput(1, errno, "bind failed");
        return 1;
    }

    res = listen(listenFd, 1024);
    if (res < 0) {
        errorOutput(1, errno, "listen failed");
        return 1;
    }

    sockaddr_un cliAddr;
    socklen_t cliLen = sizeof(cliAddr);
    int connFd = accept(listenFd, (sockaddr *) &cliAddr, &cliLen);
    if (connFd < 0) {
        errorOutput(1, errno, "accept failed");
        return 1;
    }

    char buff[1024];

    while (true) {
        bzero(buff, sizeof(buff));

        if (read(connFd, &buff, 1024) == 0) {
            std::cout << "client quit" << std::endl;
            break;
        }

        std::cout << "recv: " << buff << std::endl;

        std::string sendBuff = buff;
        sendBuff = "hi, " + sendBuff + "\0";

        write(connFd, &sendBuff[0], sendBuff.size());
    }

    close(listenFd);
    close(connFd);

    return 0;
}