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

A：通过客户端和服务器的 ip 和端口组成的四元组确定了一个连接，这个四元组就是连接套接字对。

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

```cpp
// netinet/in.h
struct sockaddr_in {
	sa_family_t sin_family;
	in_port_t sin_port;
	struct in_addr sin_addr;
	uint8_t sin_zero[8];
};
```

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
