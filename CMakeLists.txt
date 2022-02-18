cmake_minimum_required(VERSION 2.8)
project(xftp2)

set(CMAKE_CXX_STANDARD 11)
set(EXECUTABLE_OUTPUT_PATH ./bin)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAAGS} -O3 -g -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined")

add_executable(xftp2 main.cpp)