/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：echo_server.cc
      	创 建 者：汉森伯逸
      	创建日期：2022/2/20
 *================================================================*/


#include "common.h"
#include <xco/common.h>

int main() {
    std::string path;
    while(std::cin >> path) {
        if (AdjustPath(path)) {
            LOGDEBUG(XCO_EXP_VARS(path));
        }else {
            LOGDEBUG("adjust fail, " << XCO_EXP_VARS(path));
        }
    }
}