# xftp2

---
作者：谢森鑫(hsbymail@163.com)

[Click to view english README](./README_en.md)

## 简介
xftp2是一个基于[libxco](https://gitee.com/hsby/libxco)的高性能FTP服务器程序，用户使用xftp可以轻松快速搭建高性能FTP服务器，实现不同主机之间文件传输。

## 安装
```
git clone https://gitee.com/hsby/xftp2
cd xftp2
chmod a+x install.sh
./install.sh
```

## 使用
这里以FileZilla(开源FTP客户端)为例，建立新站点，主机是服务器程序所在主机ip, 端口21，加密协议选择"明文传输", 传输设置中选择"主动模式", 账号"root", 密码"123456", 点击连接。