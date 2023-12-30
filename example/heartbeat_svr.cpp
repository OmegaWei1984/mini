#include <errno.h>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <string>
#include <unistd.h>

#include "log.hpp"
#include "msg.hpp"

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        errorOutput(1, 0, "usage:\n\theart_beat <sleep time>");
    }
    int sleeptime = atoi(argv[1]);
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(5001);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    int r = bind(fd, (sockaddr *)&addr, sizeof(addr));
    if (r < 0)
    {
        errorOutput(1, errno, "bind failed");
        return 1;
    }
    r = listen(fd, 1024);
    if (r < 0)
    {
        errorOutput(1, errno, "listen failed");
        return 1;
    }

    sockaddr_in cliAddr;
    socklen_t len = sizeof(cliAddr);
    int connFd = accept(fd, (sockaddr *)&cliAddr, &len);
    if (connFd < 0)
    {
        errorOutput(1, errno, "accept failed");
        return 1;
    }

    Msg msg;
    int counter = 0;
    while (true)
    {
        int n = read(connFd, (char *)&msg, sizeof(msg));
        if (n < 0)
        {
            errorOutput(1, errno, "read error");
            return 1;
        }
        else if (n == 0)
        {
            errorOutput(1, 0, "client close");
            return 1;
        }
        std::cout << "recv " << n << " bytes" << std::endl;
        counter++;
        switch (ntohs(msg.type))
        {
        case MSG_TYPE1:
            std::cout << "msg type: " << MSG_TYPE1 << std::endl;
            break;
        case MSG_TYPE2:
            std::cout << "msg type: " << MSG_TYPE2 << std::endl;
            break;
        case MSG_PING:
        {
            Msg pong;
            pong.type = MSG_PONG;
            sleep(sleeptime);
            ssize_t rr = send(connFd, (char *)&pong, sizeof(pong), 0);
            if (rr < 0)
            {
                errorOutput(1, errno, "send failed");
            }
            std::cout << "msg type: " << MSG_PING << ", pong to client" << std::endl; 
            break;
        }
        default:
            errorOutput(1, 0, "unknow message type: " + ntohl(msg.type));
            break;
        }
    }

    return 0;
}