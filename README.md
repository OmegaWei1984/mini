# mini

写一个小型的服务器作为使用 C++ 进行开发和网络编程的实践。

## 依赖

- gcc
- boost/test

## 使用 docker

1. build 镜像
    ```bash
    docker build -t mini_dev .

    docker run -v $repo_dir:/mini_dev -it mini_dev
    ```
2. 启动容器
    ```
    docker start container_name

    docker attach container_id
    ```

# note

test
```
g++ -DBOOST_TEST_DYN_LINK test.cpp -lboost_unit_test_framework
```
- [Docker —— 从入门到实践](https://yeasy.gitbook.io/docker_practice/)
- [CMake菜谱 - 使用Boost Test进行单元测试](https://www.bookstack.cn/read/CMake-Cookbook/content-chapter4-4.4-chinese.md)
