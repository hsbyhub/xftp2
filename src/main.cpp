/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：ftp_session.h
      	创 建 者：hsby
      	创建日期：2022/2/19
 *================================================================*/

#include <iostream>
#include <ftp_server.h>
#include <xco/common.h>

int main(int argc, char** argv) {
    // xco::SetLogLevel(5);
    auto listen_socket = xco::Socket::CreateTCP();
    listen_socket->Init();
    listen_socket->Bind(xco::Ipv4Address::Create("0.0.0.0", 21));
    listen_socket->Listen();
    xco::IoManager iom;
    FtpServerConfigSgt.SetRootDir(std::string(argv[1]));
    auto ftp_server = FtpServer::Create();
    ftp_server->Init(listen_socket, &iom);
    ftp_server->Start();
    iom.Start();
}