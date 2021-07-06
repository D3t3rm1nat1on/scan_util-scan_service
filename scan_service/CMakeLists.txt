cmake_minimum_required(VERSION 3.15)
project(scan_service)

set(CMAKE_CXX_STANDARD 20)

add_executable(scan_service main.cpp dir_scanner.cpp dir_scanner.h ../include/socket.h ../include/socket.cpp)