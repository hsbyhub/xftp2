/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：echo_server.cc
      	创 建 者：汉森伯逸
      	创建日期：2022/2/19
 *================================================================*/
#pragma once

#include <xco/common.h>
#include <unordered_map>
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

    void SetCurDir(const std::string& d) {

    }

    DEFINE_PTR_CREATER(FtpUserInfo);
    std::string         name;                               // 名字
    std::string         pass                    = "123456"; // 密码
    std::string         root_dir                = FtpServerConfigSgt.GetRootDir();// 根目录
    std::string         cur_dir;                            // 当前目录
    int                 state                   = 0;        // 状态
    int64_t             last_active_time_sec    = 0;        // 上一次活跃时间(用于内存淘汰)
    FtpSession::Ptr     data_session            = nullptr;  // 客户端会话(数据通道)
};

class FtpUserInfoManager {
public:
    void Update();
    FtpUserInfo::Ptr GetUserInfo(uint32_t addr);

private:
    std::unordered_map<uint32_t, FtpUserInfo::Ptr> addr_to_user_info_;
};
#define FtpUserInfoManagerSgt Singleton<FtpUserInfoManager>::Instance()
