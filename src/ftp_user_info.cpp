/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：echo_server.cc
      	创 建 者：汉森伯逸
      	创建日期：2022/2/19
 *================================================================*/

#include "ftp_user_info.h"

void FtpUserInfoManager::Update() {

}

FtpUserInfo::Ptr FtpUserInfoManager::GetUserInfo(uint32_t addr) {
    auto& info = addr_to_user_info_[addr];
    if (!info) {
        info = FtpUserInfo::Create();
    }
    info->last_active_time_sec = TimeStampMs() / 1000;
    return info;
}
