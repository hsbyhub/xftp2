/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：ftp_server.h
      	创 建 者：hsby
      	创建日期：2022/2/18
 *================================================================*/
#pragma once

#include <xco/tcp_server.h>
#include "common.h"
#include "ftp_server_config.h"

class FtpServer : public xco::TcpServer {
public:
    DEFINE_PTR_CREATER(FtpServer);

    bool Init(xco::Socket::Ptr socket,
              xco::IoManager* io_manager = xco::IoManager::GetCurIoManager(),
              uint32_t client_handler_cnt = 512) override;

public:
    void ClientHandle(xco::Socket::Ptr client) override;
};