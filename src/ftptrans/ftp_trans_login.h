/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：echo_server.cc
      	创 建 者：汉森伯逸
      	创建日期：2022/2/20
 *================================================================*/
#pragma once

#include "ftp_transaction.h"

// USER
class FtpTransactionUSER : public BaseFtpTransaction {
public:
    DEFINE_PTR_CREATER(FtpTransactionUSER);
public:
    int OnRequest(const FtpRequest::Ptr req, FtpResponse::Ptr rsp) override;
};
REGISTER_TRANSACTION(USER);

// PASS
class FtpTransactionPASS : public BaseFtpTransaction {
public:
    DEFINE_PTR_CREATER(FtpTransactionPASS);
public:
    int OnRequest(const FtpRequest::Ptr req, FtpResponse::Ptr rsp) override;
};
REGISTER_TRANSACTION(PASS);