/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：echo_server.cc
      	创 建 者：汉森伯逸
      	创建日期：2022/2/19
 *================================================================*/
#pragma once

#include <xco/common.h>
#include <unordered_map>
#include <list>
#include "common.h"
#include "ftp_session.h"
#include "ftp_server.h"

enum FtpUserState {
    kFusLogout              = 0,
    kFusAnonymousLogin      = 1,
    kFusNeedPass            = 2,
    kFusLogin               = 3,
};

class FtpUserInfoManager;
struct FtpUserInfo : public BaseDump {
    DEFINE_PTR_CREATER(FtpUserInfo);

    std::string         name;                               // 名字
    std::string         pass                    = "123";    // 密码
};

class FtpUserInfoManager {
public:
    void Update();
    FtpUserInfo::Ptr GetUserInfo(uint32_t addr);

private:
    std::unordered_map<uint32_t, FtpUserInfo::Ptr> addr_to_user_info_;
};
#define FtpUserInfoManagerSgt Singleton<FtpUserInfoManager>::Instance()
