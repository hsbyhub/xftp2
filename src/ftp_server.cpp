/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：ftp_server.cpp
      	创 建 者：hsby
      	创建日期：2022/2/18
 *================================================================*/
#include "ftp_server.h"
#include "ftp_session.h"
#include "ftp_transaction.h"
#include "ftp_server_config.h"

bool LoadAllConfig() {
    bool ret = true;
    ret &= FtpUsersConfigSgt.Load("users.cfg");
    return ret;
}

void FtpServer::ClientHandle(xco::Socket::Ptr client) {
    // 创建会话
    auto ftp_session = FtpSession::Create(client);
    client->Send("220 (xFTPd 1.0.1)\r\n");

    // 开始会话
    while(ftp_session->IsConnected()) {
        auto req = ftp_session->GetRequest();
        if (!req) {
            continue;
        }
        LOGDEBUG(XCO_EXP_VARS(req->ToString()));

        int state = FtpTransactionManagerSgt.HandleRequest(req, ftp_session);
        if (state == -1 || state == 221) {
            break;
        }
    }

    // 关闭会话
    LOGDEBUG("close connect, " << XCO_EXP_VARS(client->ToString()));
    ftp_session->Close();
}

bool FtpServer::Init(xco::Socket::Ptr socket, xco::IoManager *io_manager, uint32_t client_handler_cnt) {
    bool ret = LoadAllConfig();
    if (!ret) {
        return ret;
    }
    return TcpServer::Init(socket, io_manager, client_handler_cnt);
}