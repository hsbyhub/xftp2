/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：echo_server.cc
      	创 建 者：汉森伯逸
      	创建日期：2022/2/19
 *================================================================*/

#include "ftp_transaction.h"
#include <fstream>
#include "ftp_server_config.h"

const int g_buffer_size = 10 * 1024 * 1024;
char g_buffer[g_buffer_size] = {0};

BaseDataSocketFtpTransaction::~BaseDataSocketFtpTransaction() {
    CloseDataSocket();
}

bool BaseDataSocketFtpTransaction::CreateDataSocket() {
    // 关闭旧会话
    CloseDataSocket();

    // 主动发起连接
    auto socket = xco::Socket::CreateTCP();
    if (!socket->Init()) {
        socket->Close();
        return false;
    }
    if (!socket->Connect(session_->GetPortAddr())) {
        socket->Close();
        return false;
    }

    data_socket = socket;
    return true;
}
void BaseDataSocketFtpTransaction::CloseDataSocket() {
    if (data_socket) {
        data_socket->Close();
        data_socket = nullptr;
    }
}

FtpTransactionManager::~FtpTransactionManager() {
    for (auto it : cmd_to_trans_type_bucket_) {
        if (it.second) {
            delete it.second;
        }
    }
    cmd_to_trans_type_bucket_.clear();
}

int FtpTransactionManager::HandleRequest(const FtpRequest::Ptr req,
                                         FtpResponse::Ptr rsp,
                                         FtpSession::Ptr session) {
    if (!req || !rsp || !session) {
        return -1;
    }
    auto it = cmd_to_trans_type_bucket_.find(req->cmd);
    if (it == cmd_to_trans_type_bucket_.end()) {
        rsp->state_code = 202;
        rsp->msg = "Not support command";
        session->SendResponse(rsp);
        LOGWARN("recieved no recognize command, " << EX_STRING_VARS(*req));
        return rsp->state_code;
    }

    auto trans = it->second->Create();
    trans->session_ = session;

    int state = trans->OnRequest(req, rsp);

    if (state == -1) {
        session->SendData("221 Goodbye.\r\n");
    }else {
        rsp->state_code = state;
        session->SendResponse(rsp);
    }

    return state;
}

int FtpTransactionUSER::OnRequest(const FtpRequest::Ptr req, FtpResponse::Ptr rsp) {
    // 匿名登录
    const char* req_name = req->msg.c_str();
    if (req->msg.empty()
        || strcasecmp(req_name, "ftp") == 0
        || strcasecmp(req_name, "anonymous") == 0) {
        session_->GetName() = "";
        session_->SetState(kFusLogin);
        rsp->msg = "Anonymous login successful.";
        return 230;
    }

    // 验证用户名
    auto user = FtpUsersConfigSgt.GetItem(req->msg);
    if (!user) {
        return -1;
    }

    session_->SetName(user->name);
    session_->SetState(kFusNeedPass);
    rsp->msg = "Need password.";
    return 331;
}

int FtpTransactionPASS::OnRequest(const FtpRequest::Ptr req, FtpResponse::Ptr rsp) {
    // 验证用户状态
    if (session_->GetState() != kFusNeedPass) {
        return -1;
    }

    // 验证密码
    auto user = FtpUsersConfigSgt.GetItem(session_->GetName());
    if (!user) {
        return -1;
    }
    if (user->pass != req->msg) {
        return -1;
    }

    // 设置登录
    session_->SetState(kFusLogin);

    rsp->msg = "Login successful.";
    return 230;
}

int FtpTransactionSYST::OnRequest(const FtpRequest::Ptr req, FtpResponse::Ptr rsp) {
    rsp->msg = "Linux";
    return 211;
}

int FtpTransactionPORT::OnRequest(const FtpRequest::Ptr req, FtpResponse::Ptr rsp) {
    // 检查状态
    if (session_->GetState() != kFusLogin) {
        rsp->msg = "No login.";
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
    auto addr = xco::Ipv4Address::Create(remote_addr_str.c_str(), vec[4] * 256 + vec[5]);
    if (!addr) {
        return 504;
    }
    session_->SetPortAddr(addr);

    rsp->msg = "PORT command successful. Consider using PASV.";
    return 200;
}

int FtpTransactionPWD::OnRequest(const FtpRequest::Ptr req, FtpResponse::Ptr rsp) {
    // 检查状态
    if (session_->GetState() != kFusLogin) {
        rsp->msg = "No login.";
        return 530;
    }

    rsp->msg = "\"" + session_->GetCurDir() + "\" is current diretory.";
    return 257;
}

int FtpTransactionCWD::OnRequest(const FtpRequest::Ptr req, FtpResponse::Ptr rsp) {
    // 检查状态
    if (session_->GetState() != kFusLogin) {
        rsp->msg = "No login";
        return 530;
    }

    // 设置路径
    session_->SetCurDir(req->msg);

    rsp->msg = "\"" + session_->GetCurDir() + "\" is current diretory.";
    return 250;
}

int FtpTransactionCDUP::OnRequest(const FtpRequest::Ptr req, FtpResponse::Ptr rsp) {
    // 检查状态
    if (session_->GetState() != kFusLogin) {
        rsp->msg = "No login.";
        return 530;
    }

    // 设置路径
    session_->SetCurDir("..");

    rsp->msg = "\"" + session_->GetCurDir() + "\" is current diretory.";
    return 250;
}

int FtpTransactionLIST::OnRequest(const FtpRequest::Ptr req, FtpResponse::Ptr rsp) {
    // 检查状态
    if (session_->GetState() != kFusLogin) {
        rsp->msg = "No login.";
        return 530;
    }

    // 主动连接数据通道
    if (!CreateDataSocket()) {
        rsp->msg = "Create data connect fail.";
        return 425;
    }

    // 通知客户端连接正常
    if (!session_) {
        return -1;
    }
    session_->SendData("150 Connect success. Here comes the list data. \r\n");

    // 开始发送数据
    std::string data = "";
    std::string cmd = "ls -l " + session_->GetAbsFilePath();
    FILE *file = popen(cmd.c_str(), "r");
    if (!file) {
        rsp->msg = "Open diretory fail.";
        return 550;
    }
    while (true) {
        int len = fread(g_buffer, 1, sizeof(g_buffer), file);
        if (len <= 0){
            break;
        }
        int ret = data_socket->Send(g_buffer, len, 0);
        if (ret <= 0) {
            break;
        }
    }
    pclose(file);

    rsp->msg = "Transfer done. Close the data connection.";
    return 226;
}

int FtpTransactionRETR::OnRequest(const FtpRequest::Ptr req, FtpResponse::Ptr rsp) {
    // 检查状态
    if (session_->GetState() != kFusLogin) {
        rsp->msg = "No login.";
        return 530;
    }

    // 主动连接数据通道
    if (!CreateDataSocket()) {
        rsp->msg = "Create data connect fail.";
        return 425;
    }

    // 通知客户端连接正常
    if (!session_) {
        return -1;
    }
    session_->SendData("150 Connect success. Here comes the list data. \r\n");

    // 打开文件，开始传输
    std::string path = session_->GetAbsFilePath(req->msg);
    auto file = fopen(path.c_str(), "rb");
    if (!file) {
        rsp->msg = "Open file fail.";
        return 550;
    }
    while(true) {
        int len = 0;
        try {
             len = fread(g_buffer, 1, g_buffer_size, file);
        }catch (...) {
            fclose(file);
            return 451;
        }
        if (len <= 0) {
            break;
        }
        for (int write_len = 0; write_len < len; ) {
            int ret = data_socket->Send(g_buffer, len - write_len, 0);
            if (ret <= 0) {
                fclose(file);
                return 426;
            }
            write_len += ret;
        }
    }

    rsp->msg = "Transfer done. Close the data connection.";
    fclose(file);
    return 226;
}

int FtpTransactionSTOR::OnRequest(const FtpRequest::Ptr req, FtpResponse::Ptr rsp) {
    // 检查状态
    if (session_->GetState() != kFusLogin) {
        rsp->msg = "No login.";
        return 530;
    }

    // 是否匿名
    if (session_->GetName().empty()) {
        rsp->msg = "Put file need user account login.";
        return 332;
    }

    // 建立连接
    if (!CreateDataSocket()) {
        rsp->msg = "Create data connect fail.";
        return 425;
    }

    // 通知客户端连接正常
    if (!session_) {
        return -1;
    }
    session_->SendData("150 Ok to send data. \r\n");

    // 开始传输
    std::string path = session_->GetAbsFilePath(req->msg);
    auto file = fopen(path.c_str(), "wb");
    if (!file) {
        rsp->msg = "Open file fail.";
        return 550;
    }

    while(true) {
        int len = data_socket->Recv(g_buffer, g_buffer_size);
        if (len <= 0) {
            break;
        }

        for (int write_len = 0; write_len < len; ) {
            int ret = 0;
            try {
                 ret = fwrite(g_buffer, 1, len - write_len, file);
            }catch (...) {
                remove(path.c_str());
                fclose(file);
                return 451;
            }
            if (ret <= 0) {
                remove(path.c_str());
                fclose(file);
                return 451;
            }
            write_len += ret;

        }
    }
    fclose(file);

    rsp->msg = "Transfer done. Close the data connection.";
    return 226;
}

int FtpTransactionQUIT::OnRequest(const FtpRequest::Ptr req, FtpResponse::Ptr rsp) {
    rsp->msg = "Goodbye.";
    return 221;
}

int FtpTransactionDELE::OnRequest(const FtpRequest::Ptr req, FtpResponse::Ptr rsp) {
    // 检查状态
    if (session_->GetState() != kFusLogin) {
        rsp->msg = "No login.";
        return 530;
    }

    // 是否匿名
    if (session_->GetName().empty()) {
        rsp->msg = "Delete file need user account login.";
        return 332;
    }

    // 删除文件
    std::string path = session_->GetAbsFilePath(req->msg);
    if (remove(path.c_str()) == -1) {
        return 550;
    }

    rsp->msg = "Ok to delete file.";
    return 250;
}