cmake_minimum_required(VERSION 3.16)
project(example CXX)

message(STATUS ${PROJECT_SOURCE_DIR})

include_directories(${PROJECT_SOURCE_DIR}/include)

set(PART_I "tcp_svr;tcp_cli;udp_svr;udp_cli;uds_svr;uds_cli")
set(PART_II "heartbeat_svr;heartbeat_cli;writev_cli;udp_conn_cli")

foreach(TARGET IN LISTS PART_I PART_II)
    MESSAGE(STATUS "TARGET=${TARGET}")
    add_executable(${TARGET} example/${TARGET}.cpp)
endforeach()