#include <errno.h>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <string>
#include <unistd.h>

#include "log.hpp"
#include "msg.hpp"

int main(void)
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in svrAddr;
    svrAddr.sin_family = AF_INET;
    svrAddr.sin_port = htons(5001);
    svrAddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    socklen_t len = sizeof(svrAddr);
    int ret = connect(fd, (sockaddr *) &svrAddr, len);
    if (ret < 0)
    {
        errorOutput(1, errno, "connect failed");
        return 1;
    }

    std::string buff;
    iovec iov[2];

    char *send = "hello,";
    iov[0].iov_base = send;
    iov[0].iov_len = strlen(send);
    iov[1].iov_base = &buff[0];
    while (std::getline(std::cin, buff))
    {
        buff += '\0';
        iov[1].iov_len = buff.size();
        int n = htonl(iov[1].iov_len);
        if (writev(fd, iov, 2) < 0) 
        {
            errorOutput(1, errno, "writev failed");
            return 1;
        }
    }
    
    return 0;
}