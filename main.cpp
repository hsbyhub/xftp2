/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：ftp_session.h
      	创 建 者：hsby
      	创建日期：2022/2/19
 *================================================================*/

#include <iostream>
#include <ftp_server.h>
#include <xco/common.h>

// 根路径
std::string root_dir;

void OnStart() {

    // 设置库日志等级
    xco::SetLogLevel(5);

    // 设置系统日志等级
    // SetLogLevel(5);

    // 设置根路径
    FtpServerConfigSgt.SetRootDir(root_dir);

    // 初始化监听套接字
    auto listen_socket = xco::Socket::CreateTCP();
    listen_socket->Init();
    listen_socket->Bind(xco::Ipv4Address::Create("0.0.0.0", 21));
    listen_socket->Listen();

    // 初始化服务器
    auto ftp_server = FtpServer::Create();
    ftp_server->Init(listen_socket);

    // 启动服务器
    ftp_server->Start();

}

void Run() {

    // 初始化Io管理器
    xco::IoManager iom;

    // 添加启动协程
    iom.Schedule(xco::Coroutine::Create(OnStart));

    // 启动Io管理器
    iom.Start();

}

int main(int argc, char** argv) {
    if (argc != 2) {
        LOGFATAL("Usage: ftp_server [root path]");
        exit(-1);
    }

    // 调整路径
    root_dir = std::string(argv[1]);
    if (!AdjustPath(root_dir)) {
        LOGFATAL("Invalid root path");
        exit(-1);
    }

    // 启动
    Run();

    return 0;
}