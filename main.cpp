/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：ftp_session.h
      	创 建 者：hsby
      	创建日期：2022/2/19
 *================================================================*/

#include <iostream>
#include "ftp_server.h"
#include "ftp_server_config.h"

void OnStartFtpServer() {

    // 初始化监听套接字
    auto listen_socket = xco::Socket::CreateTCP();
    listen_socket->Init();
    listen_socket->Bind(xco::Ipv4Address::Create("0.0.0.0", 21));
    listen_socket->Listen();

    // 初始化服务器
    auto ftp_server = FtpServer::Create();
    if (!ftp_server->Init(listen_socket)) {
        LOGFATAL("Ftp server init fail");
        ftp_server->Stop();
        exit(-1);
    }

    // 启动服务器
    if (!ftp_server->Start()) {
        LOGFATAL("Ftp server start fail");
        ftp_server->Stop();
        exit(-1);
    }

    std::cout << "Ftp server start success." << std::endl;
    std::cout << "root diretory=" << FtpServerConfigSgt.GetRootDir() << std::endl;
}

void Run() {

    // 初始化Io管理器
    xco::IoManager iom;

    // 添加启动协程
    iom.Schedule(xco::Coroutine::Create(OnStartFtpServer));

    // 启动Io管理器
    iom.Start();

}

int main(int argc, char** argv) {
    if (argc != 2) {
        LOGFATAL("Usage: ftp_server [server config diretory]");
        return -1;
    }
    // 初始化系统配置目录
    std::string server_config_dir= GetAbsPath(argv[1]);
    if (server_config_dir.empty()) {
        LOGFATAL("Bad server config diretory");
        return -1;
    }
    SetFtpServerConfigDir(server_config_dir);

    // 加载配置
    if (!LoadAllConfig()) {
        LOGFATAL("Load condig error.");
        return -1;
    }

    // 设置库日志等级
    xco::SetLogLevel(5);

    // 设置系统日志等级
    SetLogLevel(FtpServerConfigSgt.GetLogLevel());

    // 启动服务器
    Run();

    return 0;
}