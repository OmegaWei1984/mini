#include <errno.h>
#include <iostream>
#include <netinet/in.h>
#include <signal.h>
#include <string>
#include <sys/socket.h>

#include "log.hpp"

size_t readn(int fd, void *buffer, size_t length)
{
    size_t count;
    ssize_t nread;
    char *ptr = (char *)buffer;
    count = length;
    while (count > 0)
    {
        nread = read(fd, ptr, count);

        if (nread < 0)
        {
            if (errno == EINTR)
                continue;
            else
                return (-1);
        }
        else if (nread == 0)
            break;

        count -= nread;
        ptr += nread;
    }
    return (length - count);
}

size_t read_message(int fd, char *buffer, size_t length)
{
    u_int32_t msg_length;
    u_int32_t msg_type;
    int rc;

    rc = readn(fd, (char *)&msg_length, sizeof(u_int32_t));
    if (rc != sizeof(u_int32_t))
        return rc < 0 ? -1 : 0;
    msg_length = ntohl(msg_length);

    rc = readn(fd, (char *)&msg_type, sizeof(msg_type));
    if (rc != sizeof(u_int32_t))
        return rc < 0 ? -1 : 0;

    if (msg_length > length)
    {
        return -1;
    }

    rc = readn(fd, buffer, msg_length);
    if (rc != msg_length)
        return rc < 0 ? -1 : 0;
    return rc;
}

int main(void)
{
    int listenFd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(5001);

    int on = 1;
    setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    int ret = bind(listenFd, (sockaddr *)&addr, sizeof(addr));
    if (ret < 0)
    {
        errorOutput(1, errno, "bind failed");
        return 1;
    }

    ret = listen(listenFd, 1024);
    if (ret < 0)
    {
        errorOutput(1, errno, "listen failed");
        return 1;
    }

    signal(SIGPIPE, SIG_IGN);

    sockaddr_in cli;
    socklen_t len = sizeof(cli);
    int connFd = accept(listenFd, (sockaddr *)&cli, &len);
    if (connFd < 0)
    {
        errorOutput(1, errno, "accept failed");
        return 1;
    }

    char buf[128];
    int count = 0;
    while (1)
    {
        int n = read_message(connFd, buf, sizeof(buf));
        if (n < 0)
        {
            errorOutput(1, errno, "error read message");
        }
        else if (n == 0)
        {
            errorOutput(1, 0, "client closed \n");
        }
        buf[n] = 0;
        printf("received %d bytes: %s\n", n, buf);
        count++;
    }

    return 0;
}