#include <errno.h>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <string>
#include <unistd.h>

#include "include/log.hpp"
#include "include/msg.hpp"

#define MAXLINE 4096
#define KEEP_ALIVE_TIME 10
#define KEEP_ALIVE_INTERVAL 3
#define KEEP_ALIVE_PROBETIMES 3

int main(void)
{
    int sockFd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(5001);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    socklen_t len = sizeof(addr);
    int res = connect(sockFd, (sockaddr *)&addr, len);
    if (res < 0)
    {
        errorOutput(1, errno, "connect failed");
        return 1;
    }

    char recv[MAXLINE + 1];
    int n;
    fd_set readmask;
    fd_set allreads;
    timeval tv;
    int heartbeats = 0;
    tv.tv_sec = KEEP_ALIVE_TIME;
    tv.tv_usec = 0;
    Msg msg;
    FD_ZERO(&allreads);
    FD_SET(sockFd, &allreads);

    while (true)
    {
        readmask = allreads;
        int r = select(sockFd + 1, &readmask, nullptr, nullptr, &tv);
        if (r < 0) {
            errorOutput(1, errno, "select failed");
            return 1;
        }
        if (r == 0)
        {
            if (++heartbeats > KEEP_ALIVE_PROBETIMES)
            {
                errorOutput(0, errno, "connection dead");
                return 0;
            }
            std::cout << "sending heartbeat " << heartbeats << std::endl;
            msg.type = htonl(MSG_PING);
            
        }
    }

    return 0;
}
