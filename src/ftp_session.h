/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：ftp_session.h
      	创 建 者：hsby
      	创建日期：2022/2/19
 *================================================================*/
#pragma once

#include <xco/socket_stream.h>
#include "common.h"

const int MAX_FTP_REQUEST_BUFFER_LEN = 128;
const int MAX_FTP_RESPONSE_BUFFER_LEN = 2 * 1024 * 1024;

struct FtpRequest : public BaseDump{
    DEFINE_PTR_CREATER(FtpRequest);

    FtpRequest(const std::string& c, const std::string& m = "") : cmd(c), msg(m) {}
    FtpRequest(std::string&& c, std::string&& m = "") : cmd(std::move(c)), msg(std::move(m)) {}

    void Dump(std::ostream &os) const override {
        os << "FtpRequest{";
        os << "Cmd=";
        if (cmd.size()) {
            os << cmd;
        }else {
            os << "null";
        }
        os << ", Msg=";
        if (msg.size()) {
            os << msg;
        }else {
            os << "null";
        }
        os << "}";
    }

    std::string cmd;
    std::string msg;
};

struct FtpResponse : public BaseDump {
    DEFINE_PTR_CREATER(FtpResponse);

    void Dump(std::ostream &os) const override {
        os << state_code << " " << msg << "\r\n";
    }

    int state_code  = 0;
    std::string msg;
};

class FtpSession : public xco::SocketStream {
public:
    DEFINE_PTR_CREATER(FtpSession)

protected:
    FtpSession(xco::Socket::Ptr client);
public:
    FtpRequest::Ptr GetRequest();
    int SendResponse(FtpResponse::Ptr rsp);
    int SendData(const std::string& data);
};