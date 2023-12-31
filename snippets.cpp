#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>

int main(void)
{
    sockaddr addr;
    sockaddr_in addr_in;
    sockaddr_in6 addr_in6;
    sockaddr_un addr_un;

    int fd = socket(PF_INET, SOCK_STREAM, 0);
    int ret = bind(fd, &addr, sizeof(addr));

    addr_in.sin_family = AF_INET;
    addr_in.sin_addr.s_addr = htonl(INADDR_ANY);
    // addr_in.sin_port = htons(0);
    addr_in.sin_port = htons(5001);
    int ret2 = bind(fd, (sockaddr *)&addr_in, sizeof(addr_in));

    listen(fd, 128);

    sockaddr_in clientaddr;
    socklen_t len = sizeof(addr_in);
    int clientFd = accept(fd, (sockaddr *)&clientaddr, &len);

    ssize_t size;
    char buffer[1024] = {};
    size = send(fd, buffer, 1023, 0);


    return 0;
}
