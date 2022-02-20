/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：echo_server.cc
      	创 建 者：汉森伯逸
      	创建日期：2022/2/19
 *================================================================*/

#include "ftp_transaction.h"

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
    trans->user_info_ = user_info;
    int ret = trans->OnRequest(req, rsp);

    if (ret != -1) {
        session->SendResponse(rsp);
    }
    return ret;
}

bool FtpTransactionManager::SetCmdFromStr(const std::string &cmd_str, int cmd) {
    auto& it_cmd = str_to_cmd_[cmd_str];
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


int FtpTransactionUSER::OnRequest(const FtpRequest::Ptr req, FtpResponse::Ptr rsp) {
    if (!user_info_) {
        return -1;
    }

    // 匿名登录
    const char* req_name = req->msg.c_str();
    if (req->msg.empty()
        || strcasecmp(req_name, "ftp") == 0
        || strcasecmp(req_name, "anonymous") == 0) {
        user_info_->name = "";
        user_info_->state = kFusAnonymousLogin;
        rsp->state_code = 230;
        rsp->msg = "Anonymous login successful.";
        return rsp->state_code;
    }

    // 是否为第一次登录, 设置用户名
    if (user_info_->name.empty()){
        user_info_->name = req->msg;
    }

    // 验证用户名
    if (user_info_->name == req->msg){
        user_info_->state = kFusNeedPass;
        rsp->state_code = 331;
        rsp->msg = "Valid user, need password.";
    }else {
        user_info_->state = kFusLogout;
        rsp->state_code = 221;
        rsp->msg = "Invalid user";
    }

    return rsp->state_code;
}

int FtpTransactionPASS::OnRequest(const FtpRequest::Ptr req, FtpResponse::Ptr rsp) {
    if (!user_info_) {
        return -1;
    }

    LOGDEBUG("On FtpTransactionPASS::OnRequest");

    // 验证用户状态
    if (user_info_->state != kFusNeedPass) {
        goto fail;
    }

    // 验证密码
    if (user_info_->pass != req->msg) {
        goto fail;
    }

    user_info_->state = kFusLogin;
    // todo 设置目录

    rsp->state_code = 230;
    rsp->msg = "Login successful";

    return rsp->state_code;

fail:
    user_info_->state = kFusLogout;
    rsp->state_code = 221;
    rsp->msg = "Invalid user";
    return rsp->state_code;
}

int FtpTransactionSYST::OnRequest(const FtpRequest::Ptr req, FtpResponse::Ptr rsp) {
    if (!user_info_) {
        return -1;
    }

    rsp->state_code = 211;
    rsp->msg = "Linux";

    return rsp->state_code;
}

int FtpTransactionPORT::OnRequest(const FtpRequest::Ptr req, FtpResponse::Ptr rsp) {
    if (!user_info_) {
        return -1;
    }

    // 检查状态
    if (user_info_->state != kFusLogin && user_info_->state != kFusAnonymousLogin) {
        rsp->state_code = 530;
        rsp->msg = "No login";
        return rsp->state_code;
    }

    // 解析地址
    std::vector<int> vec;
    StringSplitToVectorAs(req->msg, ',', vec);
    if (vec.size() != 6) {
        rsp->state_code = 501;
        rsp->msg = "Argument error";
        return rsp->state_code;
    }

    // 构造地址
    std::string remote_addr_str;
    for (int i = 0; i < 4; ++i) {
        remote_addr_str += std::to_string(vec[i]);
        if (i != 3) {
            remote_addr_str += ".";
        }
    }
    auto remote_addr = xco::Ipv4Address::Create(remote_addr_str.c_str(), vec[4] * 256 + vec[5]);
    if (!remote_addr) {
        rsp->state_code = 504;
        return rsp->state_code;
    }
    user_info_->port_addr = remote_addr;

    rsp->state_code = 200;
    rsp->msg = "PORT command successful. Consider using PASV.";
    return rsp->state_code;
}

int FtpTransactionPWD::OnRequest(const FtpRequest::Ptr req, FtpResponse::Ptr rsp) {
    return BaseFtpTransaction::OnRequest(req, rsp);
}

int FtpTransactionLIST::OnRequest(const FtpRequest::Ptr req, FtpResponse::Ptr rsp) {
    if (!user_info_) {
        return -1;
    }

    //
    return rsp->state_code;
}
