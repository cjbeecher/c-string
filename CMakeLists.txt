cmake_minimum_required(VERSION 3.15)
project(c_json C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -O2")

add_library(cstring SHARED cstring.c)

add_executable(test_cstring test_cstring.c)
target_link_libraries(test_cstring cstring)
