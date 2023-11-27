#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string>

int main(void)
{
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    sockaddr_in addr;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(5001);

    bind(sockfd, (sockaddr *)&addr, sizeof(addr));

    char recv_buff[1024];
    socklen_t client_len;
    sockaddr_in cli_addr;
    client_len = sizeof(cli_addr);
    int count = 0;

    while (true)
    {
        int n = recvfrom(sockfd, &recv_buff, 1024, 0, (sockaddr *)&cli_addr, &client_len);
        recv_buff[n] = 0;
        std::cout << "[" << count << "] recv: " << n << " bytes, msg:" <<  recv_buff << std::endl;

        std::string send_buff = recv_buff;
        send_buff = "Hi, " + send_buff + '\0';
        sendto(sockfd, &send_buff[0], send_buff.size(), 0, (sockaddr *)&cli_addr, client_len);

        count++;
    }

    return 0;
}