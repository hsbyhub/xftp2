/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：ftp_session.h
      	创 建 者：hsby
      	创建日期：2022/2/19
 *================================================================*/

#include <iostream>
#include "ftp_server.h"
#include "ftp_server_config.h"

std::string root_dir;
std::string config_dir;

void OnStart() {

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
        return ;
    }

    // 启动服务器
    if (!ftp_server->Start()) {
        LOGFATAL("Ftp server start fail");
        ftp_server->Stop();
        return ;
    }

    std::cout << "Ftp server start success." << std::endl;
    std::cout << XCO_EXP_VARS(root_dir) << std::endl;
    std::cout << XCO_EXP_VARS(config_dir) << std::endl;
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
    if (argc != 3) {
        LOGFATAL("Usage: ftp_server [root diretory] [server config diretory]");
        exit(-1);
    }

    // 设置库日志等级
    xco::SetLogLevel(5);

    // 设置系统日志等级
    // SetLogLevel(5);

    // 初始化
    root_dir = GetAbsPath(argv[1]);
    config_dir= GetAbsPath(argv[2]);
    ASSERT_MSG(!root_dir.empty(), "Bad root path");
    ASSERT_MSG(!config_dir.empty(), "Bad config path");
    FtpServerConfigSgt.SetRootDir(root_dir);
    FtpServerConfigSgt.SetConfigDir(config_dir);

    // 启动
    Run();

    return 0;
}