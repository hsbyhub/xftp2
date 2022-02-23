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
        auto rsp = FtpResponse::Create();

        LOGWARN("Recieved request," << XCO_EXP_VARS(*req));
        int state = FtpTransactionManagerSgt.HandleRequest(req, rsp, ftp_session);
        LOGWARN("Sended response," << XCO_EXP_VARS(*rsp));
        if (state == -1 || state == 221) {
            break;
        }
    }

    // 关闭会话
    LOGWARN("Session over, close connect, " << XCO_EXP_VARS(client->ToString()));
    ftp_session->Close();
}