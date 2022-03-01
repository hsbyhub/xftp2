# xftp2

---
Author：hsby(XieSenxin hsbymail@163.com)

[点击查看中文README](./README.md)

## Introduction
Xftp2 is a high-performance FTP server program based on [libxco](https://gitee.com/hsby/libxco). Users can easily and quickly build a high-performance FTP server using xftp to realize file transfer between different hosts.

## Diretory
```
┃
┠─── src                        
┃    ┠─── common.cpp            Util
┃    ┠─── ftp_server_config.cpp Server config
┃    ┠─── ftp_session.cpp       Session (same lifetime as client command connection)
┃    ┠─── ftp_server.cpp        Server implementation
┃    ┗─── ftp_transaction.cpp   Transaction (specific processing of instructions)

┠─── xftp                       Includes server configuration and shared directory (copied to /usr/local/)
┃    ┠─── config                Config server configuration diretory
┃    ┃    ┠─── server.cfg       Server config file
┃    ┃    ┗─── users.cfg        Server user config file
┃    ┃    
┃    ┗─── root                  Server share diretory root
┃
┠─── CMakeLists.txt             Cmake project configuration file
┠─── xftp.service               Linux service daemon configuration file (will be copied to /etc/systemd/system/)
┃
```

## Installation
```
git clone https://gitee.com/hsby/xftp2
cd xftp2
chmod a+x install.sh
sudo ./install.sh
```

![image-20220224104627789](https://gitee.com/hsby/img/raw/master/202202241046016.png)

## Usage
Use FileZilla for example:  
Establish a new site. 
The host is the host IP where the server program is located, port 21. 
The encryption protocol is "plaintext transmission". 
The default account is [root] and the password is [123456].
![image-20220224103019039](https://gitee.com/hsby/img/raw/master/202202241030104.png)

In transmission settings, select active mode
![image-20220224103111846](https://gitee.com/hsby/img/raw/master/202202241031909.png)

Click connect
![image-20220224104847191](https://gitee.com/hsby/img/raw/master/202202241048264.png)

Download server share file /welcome.txt
![image-20220224105038377](https://gitee.com/hsby/img/raw/master/202202241050450.png)