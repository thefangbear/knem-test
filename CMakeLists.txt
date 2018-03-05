cmake_minimum_required(VERSION 3.9)
project(knem_test C)

set(CMAKE_C_STANDARD 11)
include_directories(/opt/knem/include)
add_executable(knem_test main.c)
