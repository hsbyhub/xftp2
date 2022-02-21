/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：ftp_session.cpp
      	创 建 者：hsby
      	创建日期：2022/2/19
 *================================================================*/
#include "ftp_session.h"
#include "ftp_server_config.h"

FtpSession::FtpSession(xco::Socket::Ptr client) : xco::SocketStream(client) { }

FtpRequest::Ptr FtpSession::GetRequest(bool& is_close) {
    // char* buffer = req->buffer;
    char buffer[MAX_FTP_REQUEST_BUFFER_LEN];
    size_t buffer_len = MAX_FTP_REQUEST_BUFFER_LEN;

    size_t parse_off = 0;
    size_t read_off = 0;

    int32_t cmd_off     = -1;
    size_t  cmd_len     = 0;
    int32_t msg_off     = -1;
    size_t  msg_len     = 0;

    bool is_finish = false;
    while(read_off < buffer_len) {
        int len = Read(buffer, buffer_len - read_off);
        if (len == 0) {
            is_close = true;
            return nullptr;
        }
        if (len < 0) {
            return nullptr;
        }
        read_off += len;
        for (; parse_off < read_off; parse_off++) {
            do {
                if (buffer[parse_off] == '\n') {
                    if (parse_off <= 0) {
                        return nullptr;
                    }
                    if (buffer[parse_off - 1] == '\r') {
                        if (msg_off != -1) {
                            msg_len = parse_off - 1 - msg_off;
                        }else if (cmd_off != -1) {
                            cmd_len = parse_off - 1 - cmd_off;
                        }
                        is_finish = true;
                        break;
                    }else {
                        return nullptr;
                    }
                }

                if (cmd_off == -1) {
                    if (buffer[parse_off] != ' ') {
                        cmd_off = parse_off;
                    }
                    break;
                }
                if (cmd_len == 0) {
                    if (buffer[parse_off] == ' ') {
                        cmd_len = parse_off - cmd_off;
                    }
                    break;
                }
                if (msg_off == -1) {
                    if (buffer[parse_off] != ' ') {
                        msg_off = parse_off;
                    }
                    break;
                }
            }while(false);
            if(is_finish) {
                break;
            }
        }
        if (is_finish) {
            break;
        }
    }
    if (!is_finish || cmd_off == -1 || cmd_len == 0) {
        return nullptr;
    }
    std::string cmd(buffer, cmd_off, cmd_len);
    std::string msg;
    if (msg_off != -1 && msg_len) {
        msg = std::string (buffer, msg_off, msg_len);
    }
    auto req = FtpRequest::Create(cmd, msg);
    return req;
}

int FtpSession::SendResponse(FtpResponse::Ptr rsp) {
    if (!rsp) {
        return -1;
    }
    LOGDEBUG(XCO_EXP_VARS(rsp->ToString()));
    auto buf = rsp->ToString();
    return Write(&buf[0], buf.size());
}

int FtpSession::SendData(const std::string &data) {
    if (data.empty()) {
        return -1;
    }
    return Write(&data[0], data.size());
}

void FtpSession::SetCurDir(const std::string& path) {
    auto root_dir = FtpServerConfigSgt.GetRootDir();
    // 设置路径
    std::string new_dir;
    if (path.front() == '/') {
        new_dir = path;
    } else {
        new_dir += cur_dir + "/" + path;
    }

    LOGDEBUG(XCO_EXP_VARS(root_dir, new_dir));
    // 解析新路径
    new_dir = GetAbsPath(root_dir + new_dir);
    LOGDEBUG(XCO_EXP_VARS(root_dir, new_dir));
    auto pos = new_dir.find(root_dir);
    if (pos != 0) {
        new_dir = "/";
    }else {
        new_dir = new_dir.substr(root_dir.size(), new_dir.size() - root_dir.size());
    }

    cur_dir = new_dir;
}

std::string FtpSession::GetCurDir() {
    if (cur_dir.empty()) {
        return "/";
    }
    return cur_dir;
}
