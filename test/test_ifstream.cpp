/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：echo_server.cc
      	创 建 者：汉森伯逸
      	创建日期：2022/2/21
 *================================================================*/

#include <fstream>
#include <iostream>

int main() {
    std::string buf;
    std::ifstream ifs("./Makefile");
    while(ifs >> buf) {
        std::cout << buf;
    }
    std::cout << std::endl;
    return 0;
}