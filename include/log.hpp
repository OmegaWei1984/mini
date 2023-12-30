#include <iostream>
#include <string>
#include <cstring>

void errorOutput(int status, int errno_, const std::string &msg)
{
    std::cout << strerror(errno_)
        << " status(" << status <<  "): "
        << msg << std::endl;
}

void output(const std::string &msg)
{
    std::cout << msg << std::endl;
}