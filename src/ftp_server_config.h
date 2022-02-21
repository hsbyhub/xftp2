/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：ftp_server_config.h
      	创 建 者：hsby
      	创建日期：2022/2/21
 *================================================================*/
#pragma once

#include "common.h"
#include <xco/common.h>

class FtpServerConfig {
private:
    std::string root_dir;
public:
    FUNCTION_BUILDER_VAR(RootDir, root_dir);
};
#define FtpServerConfigSgt Singleton<FtpServerConfig>::Instance()
