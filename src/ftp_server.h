/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：ftp_server.h
      	创 建 者：hsby
      	创建日期：2022/2/18
 *================================================================*/
#pragma once

#include <xco/tcp_server.h>
#include "common.h"

class FtpServerConfig {
private:
    std::string root_dir;
public:
    FUNCTION_BUILDER_VAR(RootDir, root_dir);
};
#define FtpServerConfigSgt Singleton<FtpServerConfig>::Instance()

class FtpServer : public xco::TcpServer {
public:
    DEFINE_PTR_CREATER(FtpServer);
public:
    void ClientHandle(xco::Socket::Ptr client) override;
};