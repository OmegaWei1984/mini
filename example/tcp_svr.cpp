#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include "log.hpp"

size_t readn(int fd, void *buffer, size_t size)
{
    char *buff = (char *) buffer;
    int len = size;

    while (len > 0)
    {
        int res = read(fd, buff, len);

        if (res < 0) {
            //  EINTR: The call was interrupted by a signal before any data was read;
            if (errno == EINTR) {
                continue;
            } else {
                return (-1);
            }
        } else if (res == 0) {
            break;
        }

        len -= res;
        buff += res;
    }

    return size - len;
}

void readdata(int fd)
{
    ssize_t len;
    char buff[1024];
    int time = 0;

    while(true) {
       output("block in read"); 
       if ((len = read(fd, &buff, 1024)) == 0) {
            return;
       }
       std::cout << "recv: " << len << " bytes " << buff << std::endl;
       time++;
       usleep(1000);
    }
}

int main(void)
{
    int listenFd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in svrsock;
    svrsock.sin_family = AF_INET;
    svrsock.sin_port = htons(5001);
    svrsock.sin_addr.s_addr = htonl(INADDR_ANY);
    bind(listenFd, (sockaddr *) &svrsock, sizeof(svrsock));
    listen(listenFd, 1024);

    sockaddr_in cliaddr;
    while (true) {
        socklen_t len = sizeof(cliaddr);
        output("wait accept");
        int connFd = accept(listenFd, (sockaddr *) &cliaddr, &len);
        readdata(connFd);
        close(connFd);
    }

    return 0;
}
