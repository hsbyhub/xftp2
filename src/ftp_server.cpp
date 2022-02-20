/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：ftp_server.cpp
      	创 建 者：hsby
      	创建日期：2022/2/18
 *================================================================*/
#include "ftp_server.h"
#include "ftp_session.h"
#include "ftptrans/ftp_transaction.h"

void FtpServer::ClientHandle(xco::Socket::Ptr client) {

    // 加载用户数据
    auto client_addr = client->GetRemoteAddress();
    if (!client_addr) {
        return;
    }
    auto sock_addr = (sockaddr_in*)client_addr->GetAddr();
    auto user_info = FtpUserInfoManagerSgt.GetUserInfo(sock_addr->sin_addr.s_addr);
    if (!user_info) {
        return;
    }

    // 创建会话
    auto ftp_session = FtpSession::Create(client);
    client->Send("220 (xFTPd 1.0.1)\r\n");
    while(true) {
        auto req = ftp_session->GetRequest();
        if (!req) {
            break;
        }
        LOGDEBUG(XCO_VARS_EXP(req->ToString()));

        int ret = FtpTransactionManagerSgt.HandleRequest(req, ftp_session, user_info);
        if (ret == -1) {
            break;
        }
    }
    ftp_session->Close();
}