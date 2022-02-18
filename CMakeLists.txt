cmake_minimum_required(VERSION 2.8)
project(xftp2)

# Config
set(CMAKE_CXX_STANDARD 11)
set(EXECUTABLE_OUTPUT_PATH ./bin)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAAGS} -O3 -std=c++11 -g -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined")

# Add to target
set(libs xco)
file(GLOB files ${PROJECT_SOURCE_DIR}/src/*.cpp)
add_executable(ftp_server ${files})
target_link_libraries(ftp_server ${libs})

# Add test target.
include_directories(${PROJECT_SOURCE_DIR}/src)
file(GLOB test_files ${PROJECT_SOURCE_DIR}/test/*.cpp)
foreach(test_file ${test_files})
    STRING(REGEX REPLACE ".+/(.+)\\..*" "\\1" target ${test_file})
    add_executable(${target} ${test_file})
    target_link_libraries(${target} ${libs})
endforeach(test_file)