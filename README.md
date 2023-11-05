# mini

写一个小型的服务器作为使用 C++ 进行开发和网络编程的实践。

## 依赖

- gcc
- boost/test

## 使用 docker

1. build 镜像
    ```bash
    docker build -t mini_dev .

    docker run -v $(pwd):/mini_dev -it mini_dev
    ```
2. 启动容器
    ```
    docker start container_name

    docker attach container_id
    ```

# note

## 计算机网络基础知识

Q：如何确定客户端和服务器的连接？

A：通过客户端和服务器的 ip 和端口组成的四元组确定了一个连接（或者再加上协议组成五元组），这个四元组就是连接套接字对。

- ip 地址
    - 描述：
        是网际协议（Internet Protocol，ip 协议）中用于标识发送或接收数据报的设备的一串数字。
    - 作用：
        1. 标识主机：标识其网络接口，并且提供主机在网络中的位置。
        2. 网络寻址：将数据报从一个网络模块送到目的地时，在发送的整个过程，IP地址充当着目的地的位置。
    - 格式：
        - 常见的IP位址分为IPv4与IPv6两大类，IP地址由一串数字组成。
            - IPv4为32位长，通常书写时以四组十进制数字组成，并以点分隔；
            - IPv6为128位长，通常书写时以八组十六进制数字组成，以冒号分割；
- port（端口）
    - 描述：
        是一种经由软件建立的服务，在电脑操作系统中扮演通信的端点。每个通讯端口都会与主机的IP地址及通信协议关联。
    - 作用：
        1. 标识服务器上提供特定网络服务的进程。
        2. 由本机地址、本机端口号、目标机地址、目标机端口号、通信协议组成的五元组，用于唯一确定正在使用的网络链接。
    - 格式：
        - 16位无符号整数，范围介于 0 与 65535 之间。
            - 0 不可使用
            - 1 ~ 1023 由系统保留
            - 1024 ~ 4999 用于客户端程序
            - 5000 ~ 65535 用于服务端。
        - 常用 5000 以上。此外有很多公认端口号需要避开，比如下列：
            - FTP 21、SSH 22、HTTPS 443、HTTP 80
            - MySQL 3306、Reids 6379、MongoDB 27017
            - [TCP/UDP端口列表](https://zh.wikipedia.org/wiki/TCP/UDP%E7%AB%AF%E5%8F%A3%E5%88%97%E8%A1%A8)

TCP/IP 模型中重要的两个传输层协议
- TCP 协议
- UDP 协议

todo：关于 TCP 流的相关讨论

Soeket 即套接字，可以理解为上层对网络连接的抽象。

套接字地址

sockaddr 结构体是通用套接字地址，它有两个成员
- sa_family 地址族
    - AF_LOCAL（AF_UNIX、AF_FILE）本地地址，进行本地 socket 通讯（进程间通信）
    - AF_INET ipv4 地址
    - AF_INET6 ipv6 地址

- sa_data 确切的地址数据

```cpp
//  sys/socket.h
struct sockaddr {
	sa_family_t sa_family;
	char sa_data[14];
};
```

sockaddr_in 结构体是对于 ipv4 的套接字地址，它有四个成员
```c
// netinet/in.h
typedef uint32_t in_addr_t;

struct in_addr {
    in_addr_t s_addr;
};

struct sockaddr_in {
	sa_family_t sin_family;
	in_port_t sin_port;
	struct in_addr sin_addr;
	uint8_t sin_zero[8];
};
```
- sin_family 协议族
- sin_addr 端口号
- sin_addr `in_addr` 结构的地址
- sin_zero 占位，没有实际作用

同样还有对于 ipv6 的套接字地址
```c
struct sockaddr_in6 {
    sa_family_t sin6_family;
    in_port_t sin6_port;	/* Transport layer port # */
    uint32_t sin6_flowinfo;	/* IPv6 flow information */
    struct in6_addr sin6_addr;	/* IPv6 address */
    uint32_t sin6_scope_id;	/* IPv6 scope-id */
};
```
- sin6_family 协议族
- sin6_port 端口号
- sin6_flowinfo ipv6 流控信息
- sin6_addr ipv6 地址
- sin6_scope_id ipv6 域 id

最后还有 sockaddr_un 对于本地进程间通信的套接字地址
```c
struct sockaddr_un {
    unsigned short sun_family;
    char sun_path[108];
};
```
- sun_family 即 AF_LOCAL
- sun_path 路径

sockaddr 和 sockaddr_in

- [Socket programming: Struct sockaddr VS struct sockaddr_in](https://stackoverflow.com/questions/29649348/socket-programming-struct-sockaddr-vs-struct-sockaddr-in)
- [What's the difference between sockaddr, sockaddr_in, and sockaddr_in6?](https://stackoverflow.com/questions/18609397/whats-the-difference-between-sockaddr-sockaddr-in-and-sockaddr-in6)
- [Why do we cast sockaddr_in to sockaddr when calling bind()?](https://stackoverflow.com/questions/21099041/why-do-we-cast-sockaddr-in-to-sockaddr-when-calling-bind)
- [Why does a cast from sockaddr_in to sockaddr work](https://stackoverflow.com/questions/51287930/why-does-a-cast-from-sockaddr-in-to-sockaddr-work)

## 测试

test
```
g++ -DBOOST_TEST_DYN_LINK test.cpp -lboost_unit_test_framework
```
- [Docker —— 从入门到实践](https://yeasy.gitbook.io/docker_practice/)
- [CMake菜谱 - 使用Boost Test进行单元测试](https://www.bookstack.cn/read/CMake-Cookbook/content-chapter4-4.4-chinese.md)
