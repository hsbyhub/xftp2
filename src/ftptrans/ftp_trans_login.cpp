/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：echo_server.cc
      	创 建 者：汉森伯逸
      	创建日期：2022/2/20
 *================================================================*/

#include "ftp_trans_login.h"

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

    rsp->state_code = 230;
    rsp->msg = "Login successful";

    return rsp->state_code;

fail:
    user_info_->state = kFusLogout;
    rsp->state_code = 221;
    rsp->msg = "Invalid user";
    return rsp->state_code;
}
