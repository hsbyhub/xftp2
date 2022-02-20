/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：echo_server.cc
      	创 建 者：汉森伯逸
      	创建日期：2022/2/19
 *================================================================*/

#include "ftp_transaction.h"
#include <arpa/inet.h>


FtpTransactionManager::~FtpTransactionManager() {
    for (auto it : cmd_to_trans_type_bucket_) {
        if (it.second) {
            delete it.second;
        }
    }
    cmd_to_trans_type_bucket_.clear();
}

int FtpTransactionManager::HandleRequest(const FtpRequest::Ptr req,
                                         FtpSession::Ptr session,
                                         FtpUserInfo::Ptr user_info) {
    LOGDEBUG(XCO_VARS_EXP(req->cmd));
    auto rsp = FtpResponse::Create();
    int cmd = GetCmdFromStr(req->cmd);
    if (cmd == FtpCmd::kFcNull) {
        rsp->state_code = 202;
        rsp->msg = "Invalid commanad";
        session->SendResponse(rsp);
        return rsp->state_code;
    }
    auto it = cmd_to_trans_type_bucket_.find(cmd);
    if (it == cmd_to_trans_type_bucket_.end()) {
        rsp->state_code = 202;
        rsp->msg = "Not support command";
        session->SendResponse(rsp);
        return rsp->state_code;
    }

    auto trans = it->second->Create();
    LOGDEBUG(XCO_VARS_EXP(cmd));
    trans->user_info_ = user_info;
    int ret = trans->OnRequest(req, rsp);

    if (ret != -1) {
        session->SendResponse(rsp);
    }
    return ret;
}

bool FtpTransactionManager::SetCmdFromStr(const std::string &cmd_str, int cmd) {
    auto& it_cmd = str_to_cmd_[cmd_str];
    LOGDEBUG(XCO_VARS_EXP(cmd_str, cmd, it_cmd));
    if (it_cmd) {
        return false;
    }
    it_cmd = cmd;
    return true;
}

int FtpTransactionManager::GetCmdFromStr(const std::string& cmd_str) {
    for (auto it : str_to_cmd_) {
        if (strcasecmp(it.first.c_str(), cmd_str.c_str()) == 0) {
            return it.second;
        }
    }
    return 0;
}
