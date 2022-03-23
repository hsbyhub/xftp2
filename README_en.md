# xftp2

---
Author：hsby(xsx hsbymail@163.com)

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

![image-20220312101537066](https://raw.githubusercontent.com/hsbyhub/ximg/main/202203121015105.png)

## Usage
Use FileZilla for example:  
Establish a new site. 
The host is the host IP where the server program is located, port 21. 
The encryption protocol is "plaintext transmission". 
The default account is [root] and the password is [123456].
![image-20220312103515984](https://raw.githubusercontent.com/hsbyhub/ximg/main/202203121035050.png)

In transmission settings, select active mode
![image-20220312103537784](https://raw.githubusercontent.com/hsbyhub/ximg/main/202203121035836.png)

Click connect
![image-20220312103600694](https://raw.githubusercontent.com/hsbyhub/ximg/main/202203121036762.png)

Download server share file /welcome.txt
![image-20220312103636355](https://raw.githubusercontent.com/hsbyhub/ximg/main/202203121037782.png)