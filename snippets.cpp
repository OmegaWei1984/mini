#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/un.h>

struct sockaddr addr;

sockaddr_in addr_in;

sockaddr_in6 addr_in6;

sockaddr_un addr_un;

int fd = socket(PF_INET, SOCK_STREAM, 0);

int ret = bind(fd, &addr, sizeof(addr));