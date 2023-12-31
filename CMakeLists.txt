cmake_minimum_required(VERSION 3.16)
project(example CXX)

message(STATUS ${PROJECT_SOURCE_DIR})

include_directories(${PROJECT_SOURCE_DIR}/include)

add_executable(tcp_svr example/tcp_svr.cpp)
add_executable(tcp_cli example/tcp_cli.cpp)

add_executable(uds_svr example/uds_svr.cpp)
add_executable(uds_cli example/uds_cli.cpp)

add_executable(heartbeats_svr example/heartbeat_svr.cpp)
add_executable(heartbeats_cli example/heartbeat_cli.cpp)

add_executable(writev_cli example/writev_cli.cpp)
