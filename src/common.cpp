/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：echo_server.cc
      	创 建 者：汉森伯逸
      	创建日期：2022/2/20
 *================================================================*/

#include "common.h"
#include <dirent.h>

static int g_xco_log_level = 1;

void FormatDate(std::ostream& os){
    char buf[64];
    time_t t = time(0);
    struct tm *tm = localtime(&t);
    strftime(buf, sizeof(buf), "%X", tm);
    os << buf;
}

int GetLogLevel() {
    return g_xco_log_level;
}

void SetLogLevel(int level) {
    g_xco_log_level = level;
}


std::string GetAbsPath(const std::string& path) {
    // Test diretory
    auto dir = opendir(path.c_str());
    if (!dir) {
        return "";
    }

    std::string cmd;
    cmd += "cd ";
    cmd += path;
    cmd += "; pwd;";
    auto fp = popen(cmd.c_str(), "r");
    char buf[1024];
    int ret = 0;
    std::stringstream ss;
    do {
        ret = fread(buf, 1, sizeof(buf) - 1, fp);
        buf[ret] = 0;
        ss << buf;
    }while(ret > 0);
    std::string res = ss.str();
    if (res.back() == '\n') {
        res.pop_back();
    }
    return res;
}