#include <errno.h>
#include "include/log.hpp"
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string>
#include <unistd.h>

#define SIZE 102400

void send_data(int sockfd)
{
    std::string data(SIZE, 'a');

    const char *cp = &data[0];
    size_t remaining = data.size();
    while (remaining) {
        int res = send(sockfd, cp, remaining, 0);
        std::cout << "send into buff " << res << std::endl;
        if (res <= 0) {
            errorOutput(1, errno, "send failed");
            return;
        }
        remaining -= res;
        cp += res;
    }
}

int main(int argc, char **argv)
{
    sockaddr_in srvaddr;
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    srvaddr.sin_family = AF_INET;
    srvaddr.sin_port = htons(5001);
    srvaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    output("start connect");
    int res = connect(sockfd, (sockaddr *) &srvaddr, sizeof(srvaddr));
    if (res < 0) {
        errorOutput(1, errno, "connect failed");
        return 0;
    }

    send_data(sockfd);

    return 0;
}
