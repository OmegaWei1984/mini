#include <errno.h>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string>

#include "log.hpp"
#include "msg2.hpp"

#define MAXLINE 1024

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

    struct
    {
        u_int32_t message_length;
        u_int32_t message_type;
        char buf[128];
    } message;
    int n;
    while (fgets(message.buf, sizeof(message.buf), stdin) != NULL)
    {
        n = strlen(message.buf);
        message.message_length = htonl(n);
        message.message_type = 1;
        if (send(sockFd, (char *)&message, sizeof(message.message_length) + sizeof(message.message_type) + n, 0) < 0)
            errorOutput(1, errno, "send failure");
    }
    exit(0);

    return 0;
}