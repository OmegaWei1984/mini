写一个小型的服务器作为使用 C++ 进行开发和网络编程的实践。

- [依赖](#依赖)
- [使用 docker](#使用-docker)
- [基础 Part I](#基础-part-i)
  - [网络连接](#网络连接)
  - [套接字 Socket](#套接字-socket)
    - [套接字地址](#套接字地址)
    - [socket 通信流程](#socket-通信流程)
      - [创建套接字](#创建套接字)
      - [开始 listen](#开始-listen)
      - [accept 客户端的连接请求](#accept-客户端的连接请求)
      - [客户端 connect](#客户端-connect)
    - [网络字节序](#网络字节序)
    - [TCP 三次握手流程](#tcp-三次握手流程)
    - [TCP 四次挥手](#tcp-四次挥手)
  - [使用套接字收发数据](#使用套接字收发数据)
    - [TCP](#tcp)
    - [UDP](#udp)
    - [本地套接字](#本地套接字)
- [基础 Part II](#基础-part-ii)
  - [TIME\_WAIT](#time_wait)
  - [关闭 TCP](#关闭-tcp)
    - [close](#close)
    - [shutdown](#shutdown)
  - [检测连接状态](#检测连接状态)
  - [TCP 传输保障](#tcp-传输保障)
  - [readv 和 writev 函数](#readv-和-writev-函数)
  - [udp connect](#udp-connect)
  - [TCP 是流](#tcp-是流)
- [测试](#测试)
- [参考](#参考)


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

## 基础 Part I

### 网络连接

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

### 套接字 Socket

Soeket 即套接字，可以理解为上层对网络连接的抽象。

#### 套接字地址

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

sockaddr_in 结构体是对于 ipv4 的套接字地址，它有四个成员。实际上 sockaddr_in 更为常用。
```cpp
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
- sin_port 端口号
- sin_addr `in_addr` 结构的地址
- sin_zero 占位，没有实际作用

同样还有对于 ipv6 的套接字地址
```cpp
// netinet/in.h
struct sockaddr_in6 {
    sa_family_t sin6_family;
    in_port_t sin6_port;    /* Transport layer port # */
    uint32_t sin6_flowinfo;    /* IPv6 flow information */
    struct in6_addr sin6_addr;    /* IPv6 address */
    uint32_t sin6_scope_id;    /* IPv6 scope-id */
};
```
- sin6_family 协议族
- sin6_port 端口号
- sin6_flowinfo ipv6 流控信息
- sin6_addr ipv6 地址
- sin6_scope_id ipv6 域 id

最后还有 sockaddr_un 对于本地进程间通信的套接字地址
```cpp
// sys/un.h
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

#### socket 通信流程
```
    client                       server
      │                            │
      │                            │
      ▼                            ▼
    socket                       socket
      │                            │
      │                            │
      │                            ▼
      │                           bind
      │                            │
      │                            │
      ▼         handshake          ▼
    connect ───────────────────► accept
      │                            │
      │                            │
      ▼                            ▼
    read                         read
         ──────────────────────►
     or                           or
         ◄──────────────────────
    write                        write
      │                            │
      │              ┌─────────────┤
      ▼              │             │
    close ───────────┘             │
                                   ▼
                                 close
```

##### 创建套接字
```cpp
int socket (int domain, int type, int protocol);

int fd = socket(PF_INET, SOCK_STREAM, 0);
```

- domain 协议域
  - PF_INET
  - PF_INET6
  - PF_LOCAL
- type 套接字类型
  - SOCK_STREAM 字节流，对应 TCP
  - SOCK_DGRAM 数据报，对应 UDP
  - SOCK_RAW 原始套接字
- protocol 协议，已废弃，一般使用 0

将套接字绑定到地址
```cpp
int bind(int fd, sockaddr * addr, socklen_t len);

int ret = bind(fd, &addr, sizeof(addr));
// 强制转换为 sockaddr
int ret = bind(fd, (struct sockaddr *)&addr_in, sizeof(addr_in));
```

初始化 `sockaddr_in`

可以直接使用通配地址 `INADDR_ANY` 进行地址的设置，
```cpp
addr_in.sin_addr.s_addr = htonl(INADDR_ANY);
addr_in.sin_family = AF_INET;
addr_in.sin_port = htons(port);
```

##### 开始 listen

```cpp
int listen (int socketfd, int backlog);
int ret = listen(sockfd, backlog);
```
- socketfd，之前创建的套接字的 fd
- backlog，已完成但是还没有 accept 的队列大小

##### accept 客户端的连接请求
```cpp
int accept(int listensockfd, struct sockaddr *cliaddr, socklen_t *addrlen);
int clientFd = accept(fd, &clientaddr, &len);
```
- listensockfd，listen 套接字的 fd
- cliaddr，客户端套接字地址
- addrlen，地址大小

##### 客户端 connect
```cpp
int connect(int sockfd, const struct sockaddr *servaddr, socklen_t addrlen);
int ret = connect(clientFd, (sockaddr *)&servaddr, sizeof(serveraddr));
```
- sockfd，客户端创建的套接字的 fd
- servaddr，服务器的套接字地址
- addrlen，地址大小

connect 的常见错误
- TIMEOUT，没有建立三次握手，服务端 ip 本身不可达。
- CONNECTION REFUSED，连接被服务端取消。可能端口错误，服务器上根本没有监听这个端口。
  - 比如端口设置错误，需要使用 `htons` 进行转换
    > htonl, htons, htonll, ntohl, ntohs, ntohll – convert values between host and network byte order

#### 网络字节序

网络协议传输采用的是大端字节序，而计算机处理器体系采用的是小端字节序，所以需要进行转换。以下是 POSIX 标准的转换函数：

```c
uint16_t htons (uint16_t hostshort)
uint16_t ntohs (uint16_t netshort)
uint32_t htonl (uint32_t hostlong)
uint32_t ntohl (uint32_t netlong)
```

#### TCP 三次握手流程
```
socket      │                │    socket,bind,listen
            │                │
            │                │
            │                │
connect     │     SYN i      │   accept
SYN_SENT    ├───────────────►│   SYN_RCVD
            │                │
            │                │
            │  SYN j,ACK i+1 │
            │◄───────────────┤
connect     │                │
ESTABLISED  │                │
            │    ACK j+1     │
            ├───────────────►│   accept,read
            │                │   ESTABLISED
            │                │
            │                │
            │                │
```

#### TCP 四次挥手

```
client                            server
            │                 │
            │                 │
ESTABLISHED │       FIN       │   ESTABLISHED
            ├────────────────►│
            │                 │
FIN_WAIT_1  │                 │
            │       ACK       │
            │◄────────────────┤
            │                 │
FIN_WAIT_2  │                 │   CLOSE_WAIT
            │       FIN       │
            │◄────────────────┤
            │                 │
            │                 │   LAST_ACK
            │       ACk       │
            ├────────────────►│
            │                 │
TIME_WAIT   │                 │   CLOSE
            │                 │
            │                 │
2MSL        │                 │
            │                 │
            │                 │
CLOSE       │                 │
            │                 │
```

### 使用套接字收发数据

#### TCP

读
```c
ssize_t read (int socketfd, void *buffer, size_t size);
```

写
```c
ssize_t write (int socketfd, const void *buffer, size_t size)
ssize_t send (int socketfd, const void *buffer, size_t size, int flags)
ssize_t sendmsg(int sockfd, const struct msghdr *msg, int flags)
```

#### UDP
读
```c
// sys/socket.h
ssize_t recvfrom(int sockfd, void *buff, size_t nbytes, int flags, 
　　　　　　　　　　struct sockaddr *from, socklen_t *addrlen); 
```
- sockfd 套接字 fd
- buff 缓冲
- nbytes 读取的最大数据字节
- flags 标志
- from 接收端套接字地址
- addrlen 地址大小

写
```c
// sys/socket.h
ssize_t sendto(int sockfd, const void *buff, size_t nbytes, int flags,
                const struct sockaddr *to, socklen_t addrlen); 
```
- sockfd 套接字 fd
- buff 缓冲
- nbytes 读取的最大数据字节
- flags 标志
- to 发送端套接字地址
- addrlen 地址大小

#### 本地套接字

本地套接字地址的结构有所不同，需要设置套接字文件的路径 `sun_path`，进行进程间通信这个路径的文件必不可少（并非会向文件写入数据，只是这个文件代表两个进程之间产生了联系）。

本地套接字可以使用 `SOCK_STREAM` 的 `read/write`，也可以使用 `SOCK_DGRAM` 的 `recvfrom/sendto`，很显然用法和 tcp、udp socket 的用法的类似。

## 基础 Part II

### TIME_WAIT

在 TCP 连接断开进行四次挥手后，有一段时间 `TIME_WAIT` 后才会完全关闭，这是为了等待~~由于种种原因而产生的~~异常的报文自行消失的时间而设计的。因为这种报文的连接四元组与正常的完全相同，会干扰正常的 TCP 连接。但是这个 `TIME_WAIT` 同时也会占用服务器的端口和内存资源，所以需要对它进行优化。（在 TCP 拓展规范中有时间戳，通过时间戳可以判断是否为异常的报文）

- 调整 `net.ipv4.tcp_tw_reuse` 参数使得在可行的情况下可以复用 TIME_WAIT 状态的连接，比如连接是客户端发起的。

下面几种做法能达到目的但是不推荐：

- 调整 `net.ipv4.tcp_max_tw_buckets` 参数控制最大 `TIME_WAIT` 状态连接的数量，当超过这个值系统会将所有 `TIME_WAIT` 状态的连接，但是这样调整可能会产生多问题
- 调低 `TCP_TIMEWAIT_LEN`，这个参数控制 `TIME_WAIT` 的时长，但是调整这个参数需要重新编译内核。
- SO_LINGER 套接字选项的设置，`l_onoff` 非 0 ，在 close 后会经过 `l_linger` 设置的计时结束后发送 RST，这个连接会跳过四次挥手和 `TIME_WAIT`，但是这样会导致还在排队的数据不会被发送完毕，被关闭的一端没有经过挥手，在 recv 时才能得知被 RST 了。

下面是一个实验：

启动后客户端连接上去，此时状态为 `ESTABLISHED`

```bash
tcp        0      0 127.0.0.1:5001          127.0.0.1:41332         ESTABLISHED 6667/./tcp_svr
tcp        0      0 127.0.0.1:41332         127.0.0.1:5001          ESTABLISHED 6671/telnet
```

使用 `ctrl+c` 关闭了服务端，状态进入 `TIME_WAIT`

```bash
tcp        0      0 127.0.0.1:5001          127.0.0.1:41332         TIME_WAIT   -
```

此时立即再启动服务端，使用客户端连接会返回 "Connection refused"，并且此时状态仍然是 `TIME_WAIT`

```bash
tcp        0      0 127.0.0.1:5001          127.0.0.1:41332         TIME_WAIT   -

telnet: Unable to connect to remote host: Connection refused
```

等待两个 MSL（[Maximum segment lifetime](https://en.wikipedia.org/wiki/Maximum_segment_lifetime)）后才能再正常启动服务端。

### 关闭 TCP

#### [close](https://man7.org/linux/man-pages/man2/close.2.html)

使用 `close` 函数会关闭 fd，使其不再引用任何文件（引用计数减 1），并且可以重用。对于关闭套接字的 fd，如果是 fd 的最后一个引用，才会真正释放相关资源，并且关闭 TCP 输入输出的数据流。内核把套接字设置为不可读，无法再从套接字读取数据。内核把缓冲区的数据发送完毕后会发送 `FIN`，无法再从套接字写入数据。此时另一端如果继续发送数据将会返回 `RST`。

返回 `0` 代表成功，`-1` 代表出错

```cpp
#include <unistd.h>
int close(int fd);
```

- [How is "multiple file descriptors refer to the same socket" created?](https://unix.stackexchange.com/questions/621373/how-is-multiple-file-descriptors-refer-to-the-same-socket-created)
- [How Linux creates sockets and counts them](https://ops.tips/blog/how-linux-creates-sockets/)

#### [shutdown](https://man7.org/linux/man-pages/man2/shutdown.2.html)

`shutdown` 则是 `sys/socket.h` 下的函数，使用 `shutdown` 函数会让 fd 关联的套接字上的全部或部分全双工连接被直接关闭（而不是引用计数减 1），并且不会释放套接字和相关资源。参数 `how` 有三种选项 `SHUT_RD` `SHUT_WR` `SHUT_RDWR`（值分别对应 0、1、2）来控制关闭读、关闭写和关闭读写（即前面描述的“全部或部分全双工”）。

返回 `0` 代表成功，`-1` 代表出错

```cpp
#include <sys/socket.h>
int shutdown(int sockfd, int how);
```

客户端执行 `shutdown(fd, SHUT_WR)` 后不再发送数据，仍然可以读取服务器未发送完的数据，等读取到 `EOF`（zero indicates end of file）代表服务器也关闭再进行退出。

### 检测连接状态

`keepalive` 相关参数 `net.ipv4.tcp_keepalive_time`、`net.ipv4.tcp_keepalive_intvl`、`net.ipv4.tcp_keepalve_probes`，默认值分别为 7200、75、9，意为 7200 秒后每 75 秒探测一次，探测 9 次后才能认为连接已经失效了。

此外可以自己在应用层实现一个心跳机制。与 `keepalive` 类似但是由应用层发起一个 `ping` 消息进行探测，另一端收到后回复 `pong` 消息，如果多次间隔的 `ping` 都没有收到回复就可以认为连接状态异常。

### TCP 传输保障

收发端能处理的流量不是对等的，需要通过发送窗口和接收窗口来控制。同样的在网络传输的每个环节的能力也是有限的，需要通过拥塞控制避免传输中的问题在网络中被放大。

- [TCP 重传、滑动窗口、流量控制、拥塞控制](https://www.xiaolincoding.com/network/3_tcp/tcp_feature.html)

TCP 缓冲区中的数据能否真正发出取决于 “发送窗口” 和 “拥塞窗口” 的大小。

- 糊涂窗口综合症 发送窗口过小、应用为交互式、大量 ACK 报文 -> 每次只能发送很少的数据，导致带宽消耗真实要传输的数据之外 -> Nagle 算法、延时 ACK、写操作合并

### readv 和 writev 函数

`v` 是向量 `vector` 的意思

```cpp
struct iovec {
  void *iov_base; /* starting address of buffer */
  size_t　iov_len; /* size of buffer */
};

ssize_t writev(int filedes, const struct iovec *iov, int iovcnt)
ssize_t readv(int filedes, const struct iovec *iov, int iovcnt);
```

### udp connect

`SOCK_DGRAM` 类型的套接字也可以使用 `connect` 函数。进行 `connect` 时并不会进行通信，而是将 UDP 套接字和服务器的地址端口进行了绑定。当服务端不可达时，没有进行 `connect` 的套接字没有关联，当收到 ICMP 不可达报文时无法得知是哪个套接字不可达，进行了 `connect` 的套接字会返回 `Connection refused` 的错误。

### TCP 是流

虽然 “TCP 粘包” 的说法是错的，但是数据是流，不会按照 `send` 时分界，“在应用时需要处理” 的问题是真实的。

- [怎么解决TCP网络传输「粘包」问题？](https://www.zhihu.com/question/20210025)
- [如何理解是 TCP 面向字节流协议？](https://xiaolincoding.com/network/3_tcp/tcp_stream.html#%E5%A6%82%E4%BD%95%E7%90%86%E8%A7%A3%E5%AD%97%E8%8A%82%E6%B5%81)

应用对数据的拆分有三种常见的方法：

- 定长，不足的长度用别的字符填充
- 定义特殊的边界字符
- 通过字段定义消息长度

## 测试

test
```
g++ -DBOOST_TEST_DYN_LINK test.cpp -lboost_unit_test_framework
```
- [Docker —— 从入门到实践](https://yeasy.gitbook.io/docker_practice/)
- [CMake菜谱 - 使用Boost Test进行单元测试](https://www.bookstack.cn/read/CMake-Cookbook/content-chapter4-4.4-chinese.md)

## 参考

- [使用socket()函数创建套接字](https://xiaoxiami.gitbook.io/linux-server/socket/socket-xiang-guan-han-shu/shi-yong-socket-han-shu-chuang-jian-tao-jie-zi)
- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/html/)
- [Fixed width integer types (since C++11)](https://en.cppreference.com/w/cpp/types/integer)
