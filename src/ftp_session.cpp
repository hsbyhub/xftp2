/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：ftp_session.cpp
      	创 建 者：hsby
      	创建日期：2022/2/19
 *================================================================*/
#include "ftp_session.h"

FtpRequest::Ptr FtpSession::GetRequest() {
    auto req = FtpRequest::Create();
    char* buffer = req->buffer;
    size_t buffer_len = req->buffer_len;

    size_t parse_off = 0;
    size_t read_off = 0;
    bool is_finish = false;
    while(read_off < buffer_len) {
        int len = Read(buffer, buffer_len - read_off);
        if (len <= 0) {
            return nullptr;
        }
        read_off += len;
        for (; parse_off < read_off; parse_off++) {
            do {
                if (req->cmd_off == -1) {
                    if (buffer[parse_off] != ' ') {
                        req->cmd_off = parse_off;
                    }
                    break;
                }
                if (req->cmd_len == 0) {
                    if (buffer[parse_off] == ' ') {
                        req->cmd_len = parse_off - req->cmd_off;
                    }
                    break;
                }
                if (req->msg_len == -1) {
                    if (buffer[parse_off] != ' ') {
                        req->msg_off = parse_off;
                    }
                    break;
                }
                if (req->msg_len == 0) {
                    if (buffer[parse_off] == '\n') {
                        if (buffer[parse_off - 1] == '\r') {
                            is_finish = true;
                        }else {
                            return nullptr;
                        }
                    }
                    break;
                }

            }while(false);
        }
    }
}
