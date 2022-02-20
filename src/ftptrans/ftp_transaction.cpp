/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：echo_server.cc
      	创 建 者：汉森伯逸
      	创建日期：2022/2/19
 *================================================================*/

#include "ftp_transaction.h"
#include <memory>

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
    trans->cmd_session = session;
    int ret = trans->OnRequest(req, rsp);

    if (ret != -1) {
        rsp->state_code = ret;
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
        rsp->msg = "Anonymous login successful.";
        return 230;
    }

    // 是否为第一次登录, 设置用户名
    if (user_info_->name.empty()){
        user_info_->name = req->msg;
    }

    // 验证用户名
    if (user_info_->name != req->msg){
        user_info_->state = kFusLogout;
        rsp->msg = "Invalid user";
        return 221;
    }

    user_info_->state = kFusNeedPass;
    rsp->msg = "Valid user, need password.";
    return 331;
}

int FtpTransactionPASS::OnRequest(const FtpRequest::Ptr req, FtpResponse::Ptr rsp) {
    if (!user_info_) {
        return -1;
    }

    LOGDEBUG("On FtpTransactionPASS::OnRequest");

    // 验证用户状态
    if (user_info_->state != kFusNeedPass) {
        rsp->msg = "Invalid user";
        return 221;
    }

    // 验证密码
    if (user_info_->pass != req->msg) {
        rsp->msg = "Invalid password";
        return 221;
    }

    // 设置登录
    user_info_->state = kFusLogin;

    rsp->msg = "Login successful";
    return 230;
}

int FtpTransactionSYST::OnRequest(const FtpRequest::Ptr req, FtpResponse::Ptr rsp) {
    if (!user_info_) {
        return -1;
    }

    rsp->msg = "Linux";
    return 211;
}

int FtpTransactionPORT::OnRequest(const FtpRequest::Ptr req, FtpResponse::Ptr rsp) {
    if (!user_info_) {
        return -1;
    }

    // 检查状态
    if (user_info_->state != kFusLogin && user_info_->state != kFusAnonymousLogin) {
        rsp->msg = "No login";
        return 530;
    }

    // 解析地址
    std::vector<int> vec;
    StringSplitToVectorAs(req->msg, ',', vec);
    if (vec.size() != 6) {
        rsp->msg = "Argument error";
        return 501;
    }

    // 构造地址
    std::string remote_addr_str;
    for (int i = 0; i < 4; ++i) {
        remote_addr_str += std::to_string(vec[i]);
        if (i != 3) {
            remote_addr_str += ".";
        }
    }
    LOGDEBUG(XCO_EXP_VARS(vec[4], vec[5], vec[4]*256+vec[5]));
    auto addr = xco::Ipv4Address::Create(remote_addr_str.c_str(), vec[4] * 256 + vec[5]);
    if (!addr) {
        return 504;
    }
    user_info_->port_addr = addr;
    LOGDEBUG(XCO_EXP_VARS(addr->ToString()));

    rsp->msg = "PORT command successful. Consider using PASV.";
    return 200;
}

int FtpTransactionPWD::OnRequest(const FtpRequest::Ptr req, FtpResponse::Ptr rsp) {
    if (!user_info_) {
        return -1;
    }

    // 检查状态
    if (user_info_->state != kFusLogin && user_info_->state != kFusAnonymousLogin) {
        rsp->msg = "No login";
        return 530;
    }

    // 设置目录
    user_info_->SetCurDir(req->msg);

    rsp->msg = "\"" + user_info_->cur_dir + "\" is current diretory";
    return 257;
}

int FtpTransactionLIST::OnRequest(const FtpRequest::Ptr req, FtpResponse::Ptr rsp) {
    if (!user_info_) {
        return -1;
    }

    // 检查状态
    if (user_info_->state != kFusLogin && user_info_->state != kFusAnonymousLogin) {
        rsp->msg = "No login.";
        return 530;
    }

    // 主动连接数据通道
    if (!user_info_->CreateDataSession()) {
        rsp->msg = "Create data connect fail.";
        return 425;
    }

    // 通知客户端连接正常
    if (!cmd_session) {
        return -1;
    }
    auto tmp_rsp = FtpResponse::Create();
    tmp_rsp->state_code = 150;
    tmp_rsp->msg = "Connect success. Here comes the list data.";
    cmd_session->SendResponse(tmp_rsp);

    // 开始发送数据
    std::string data = "";
    std::string cmd = "ls -l " + user_info_->root_dir + user_info_->cur_dir;
    FILE *file = popen(cmd.c_str(), "r");
    if (!file) {
        rsp->msg = "Open diretory fail.";
        return 550;
    }
    char buf[1024];
    bzero(buf, sizeof(buf));
    while (true) {
        int len = fread(buf, 1, sizeof(buf), file);
        if (len <= 0){
            break;
        }
        int ret = user_info_->data_session->Write(buf, len);
        if (ret <= 0) {
            break;
        }
    }
    pclose(file);

    // 结束连接
    user_info_->data_session->Close();

    return 225;
}

int FtpTransactionCWD::OnRequest(const FtpRequest::Ptr req, FtpResponse::Ptr rsp) {
    if (!user_info_) {
        return -1;
    }

    // 调整路径
    user_info_->SetCurDir(user_info_->cur_dir + req->msg);

    rsp->msg = "Change diretory success. \"" + user_info_->cur_dir + "\" is current diretory.";
    return 257;
}
