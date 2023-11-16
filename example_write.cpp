#include <errno.h>
#include "include/log.hpp"
#include <iostream>
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
            output(1, errno, "send failed");
            return;
        }
        remaining -= res;
        cp += res;
    }
}

int main(void)
{
    return 0;
}
