/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：test_popen.cpp
      	创 建 者：hsby
      	创建日期：2022/2/21
 *================================================================*/

#include <iostream>
#include <sstream>
#include <xco/common.h>
#include "common.h"
#include <dirent.h>

int main(int argc, char** argv) {
    if (argc != 2) {
        exit(-1);
    }

    // Test diretory
    auto dir = opendir(argv[1]);
    ASSERT_MSG(dir, "Bad path");

    std::string cmd;
    cmd += "cd ";
    cmd += argv[1];
    cmd += ";";
    cmd += "pwd;";
    auto fp = popen(cmd.c_str(), "r");
    char buf[1024];
    int ret = 0;
    std::stringstream ss;
    do {
        ret = fread(buf, 1, sizeof(buf) - 1, fp);
        buf[ret] = 0;
        LOGDEBUG(XCO_EXP_VARS(ret));
        ss << buf;
    }while(ret > 0);
    std::string path = ss.str();
    if (path.back() == '\n') {
        LOGDEBUG("path has \\n");
        path.pop_back();
    }
    LOGDEBUG(XCO_EXP_VARS(path));
}
