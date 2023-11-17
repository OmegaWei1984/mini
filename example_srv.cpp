#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include "include/log.hpp"

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
       time++;
       usleep(1000);
    }
}

int main(void)
{
    int listenFd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in srvsock;
    srvsock.sin_family = AF_INET;
    srvsock.sin_port = htons(5001);
    srvsock.sin_addr.s_addr = htonl(INADDR_ANY);
    bind(listenFd, (sockaddr *) &srvsock, sizeof(srvsock));
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
