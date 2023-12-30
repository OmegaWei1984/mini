#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "log.hpp"

#define PATH "/tmp/udstest"

int main(void) {
    int sockFd = socket(AF_LOCAL, SOCK_STREAM, 0);
    if (sockFd < 0) {
        errorOutput(1, errno, "create socket failed");
        return 1;
    }
    sockaddr_un svraddr;
    svraddr.sun_family = AF_LOCAL;
    strcpy(svraddr.sun_path, PATH);

    if (connect(sockFd, (sockaddr *) &svraddr, sizeof(svraddr)) < 0) {
        errorOutput(1, errno, "connect failed");
        return 1;
    }

    std::string send_buff;
    char recv_buff[1024];
    for (; std::getline(std::cin, send_buff);) {
        std::cout << "sending ... " << send_buff << std::endl;
        size_t ret = write(sockFd, &send_buff[0], send_buff.size());
        if (ret < 0) {
            std::cout << "send failed" << std::endl;
        } else {
            std::cout << "sent " << ret << " bytes" << std::endl;
        }

        int n = read(sockFd, &recv_buff[0], 1024);
        if (n < 0) {
            std::cout << "recv failed" << std::endl;
        } else {
            std::cout << recv_buff << std::endl;
        }
    }

    return 0;
}