#include <iostream>
#include <string>

void output(int status, int errno_, const std::string &msg)
{
    std::cout << strerror(errno_)
        << " status(" << status <<  "): "
        << msg << std::endl;
}