/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：echo_server.cc
      	创 建 者：汉森伯逸
      	创建日期：2022/2/20
 *================================================================*/

#include "common.h"

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

bool AdjustPath(std::string& path) {
    std::list<std::string> st;
    int p = 0;
    for (int i = 0; i < (int)path.size(); ) {
        char& c = path[i];
        if (i == (int)path.size() - 1) {
            st.push_back(path.substr(p, i + 1 - p));
            p = i + 2;
            i = p;
        }else if (c == '/'){
            st.push_back(path.substr(p, i - p));
            p = i + 1;
            i = p;
        }else if (c == '.') {
            if (i + 1 < (int)path.size()) {
                if (path[i+1] == '.') {
                    if (st.empty()) {
                        path = "";
                        return true;
                    }else {
                        st.pop_back();
                    }
                    p = i + 3;
                    i = p;
                }else if (path[i+1] == '/'){
                    p = i + 2;
                    i = p;
                }
            }
        }else {
            i++;
        }
    }
    path.clear();
    for (auto it : st) {
        path += it + "/";
    }
    return true;
}