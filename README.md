# xftp2

---
作者：汉森伯逸(xsx hsbymail@163.com)

[Click to view english README](./README_en.md)

## 简介
xftp2是一个基于[libxco](https://gitee.com/hsby/libxco)的高性能FTP服务器程序，用户使用xftp可以轻松快速搭建高性能FTP服务器，实现不同主机之间文件传输。

## 目录
```
┃
┠─── src                        
┃    ┠─── common.cpp            公共工具
┃    ┠─── ftp_server_config.cpp 服务器配置
┃    ┠─── ftp_session.cpp       会话(与客户端命令连接同生命期)
┃    ┠─── ftp_server.cpp        服务器
┃    ┗─── ftp_transaction.cpp   事务(指令的具体处理)

┠─── xftp                       包括服务器配置和共享目录(安装时复制到/usr/local/)
┃    ┠─── config                服务器配置文件夹
┃    ┃    ┠─── server.cfg       服务器配置
┃    ┃    ┗─── users.cfg        账户配置(包括默认的两个账户)
┃    ┃    
┃    ┗─── root                  服务器共享根目录
┃
┠─── CMakeLists.txt             CMake工程配置文件
┠─── xftp.service               Linux daemon配置文件(安装时将被复制到/etc/systemd/system/)
┃
```

## 安装
```
git clone https://gitee.com/hsby/xftp2
cd xftp2
chmod a+x install.sh
sudo ./install.sh
```
这是我在虚拟机linux上安装的样子:
![image-20220312101537066](https://raw.githubusercontent.com/hsbyhub/ximg/main/202203121015105.png)

## 使用

这里以FileZilla(开源FTP客户端)为例:  
建立新站点，主机是服务器程序所在主机ip,  端口21，加密协议选择"明文传输"， 默认账号 root, 密码 123456
![image-20220312103515984](https://raw.githubusercontent.com/hsbyhub/ximg/main/202203121035050.png)

传输设置中，选择主动模式
![image-20220312103537784](https://raw.githubusercontent.com/hsbyhub/ximg/main/202203121035836.png)

点击连接
![image-20220312103600694](https://raw.githubusercontent.com/hsbyhub/ximg/main/202203121036762.png)

下载 /welcome.txt
![image-20220312103636355](https://raw.githubusercontent.com/hsbyhub/ximg/main/202203121037782.png)

