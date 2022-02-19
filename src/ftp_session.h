/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：ftp_session.h
      	创 建 者：hsby
      	创建日期：2022/2/19
 *================================================================*/
#pragma once

#include <xco/socket_stream.h>

const int MAX_FTP_REQUEST_BUFFER_LEN = 128;

struct FtpRequest : public BaseDump{
    typedef std::shared_ptr<FtpRequest> Ptr;
    static FtpRequest::Ptr Create() {
        return std::shared_ptr<FtpRequest>(new FtpRequest);
    }

    void Dump(std::ostream &os) const override {
        os << "FtpRequest{";
        os << "Cmd=";
        if (cmd_len) {
            assert(cmd_off > -1);
            for (int i = cmd_off; i < cmd_off + cmd_len; ++i) {
                os << buffer[i];
            }
        }else {
            os << "null";
        }
        os << ", Msg=";
        if (msg_len) {
            assert(msg_off > -1);
            for (int i = msg_off; i < msg_off + msg_len; ++i) {
                os << buffer[i];
            }
        }else {
            os << "null";
        }
        os << "}";
    }

    char buffer[MAX_FTP_REQUEST_BUFFER_LEN];
    size_t  buffer_len  = MAX_FTP_REQUEST_BUFFER_LEN;
    int32_t cmd_off     = -1;
    size_t  cmd_len     = 0;
    int32_t msg_off     = -1;
    size_t  msg_len     = 0;
};

class FtpSession : xco::SocketStream {
public:
    FtpRequest::Ptr GetRequest();
};