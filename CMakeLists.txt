cmake_minimum_required(VERSION 2.8)
project(xftp2)

# Config
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/xftp/bin)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAAGS} -O3 -std=c++11 -g -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined")

# Add to target
include_directories(${PROJECT_SOURCE_DIR}/src)
set(libs xco)
file(GLOB files ${PROJECT_SOURCE_DIR}/src/*.cpp)
add_library(ftp STATIC ${files})
target_link_libraries(ftp ${libs})

# Add ftp_server target
add_executable(xftp_server main.cpp)
target_link_libraries(xftp_server ftp)

# Add test target
#file(GLOB test_files ${PROJECT_SOURCE_DIR}/test/*.cpp)
#foreach(test_file ${test_files})
#    STRING(REGEX REPLACE ".+/(.+)\\..*" "\\1" target ${test_file})
#    add_executable(${target} ${test_file} )
#    target_link_libraries(${target} ${libs} ftp)
#endforeach(test_file)