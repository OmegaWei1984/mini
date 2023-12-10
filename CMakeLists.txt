cmake_minimum_required(VERSION 3.16)
project(example CXX)

message(STATUS ${PROJECT_SOURCE_DIR})

include_directories(${PROJECT_SOURCE_DIR}/include)
add_executable(uds_srv example/uds_srv.cpp)
add_executable(uds_cli example/uds_cli.cpp)
