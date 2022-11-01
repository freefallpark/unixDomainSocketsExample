cmake_minimum_required(VERSION 3.21)
project(unixDomainSocketPractice)

set(CMAKE_CXX_STANDARD 11)

include_directories(common)

add_executable(serverProcess serverProcess.cpp
        common/converters.cpp           common/converters.h common/connection.h)
add_executable(clientProcess clientProcess.cpp
        common/converters.cpp           common/converters.h common/connection.h)
