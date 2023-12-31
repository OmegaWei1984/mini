#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string>

#include "log.hpp"

int main(void)
{
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    sockaddr_in addr_svr;
    addr_svr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr_svr.sin_family = AF_INET;
    addr_svr.sin_port = htons(5001);
    socklen_t len = sizeof(addr_svr);

    connect(sockfd, (sockaddr *)&addr_svr, len);

    sockaddr_in *p_reply_addr;
    std::string send_buff;
    char recv_buff[1024];

    while (std::getline(std::cin, send_buff))
    {
        std::cout << "sending ... " << send_buff << std::endl;
        send_buff += '\0';
        size_t ret = sendto(sockfd, &send_buff[0], send_buff.size(), 0, (sockaddr *)&addr_svr, len);
        if (ret < 0)
        {
            errorOutput(1, errno, "send failed");
        }
        else
        {
            std::cout << "sent " << ret << " bytes" << std::endl;
        }

        len = 0;
        int n = recvfrom(sockfd, &recv_buff, 1024, 0, (sockaddr *)p_reply_addr, &len);
        if (n < 0)
        {
            errorOutput(1, errno, "recv failed");
        }
        else
        {
            std::cout << recv_buff << std::endl;
        }
    }

    return 0;
}