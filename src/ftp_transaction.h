/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：echo_server.cc
      	创 建 者：汉森伯逸
      	创建日期：2022/2/19
 *================================================================*/
#pragma once

#include "common.h"
#include "ftp_session.h"
#include <unordered_map>

class FtpTransactionManager;

// 基础的Ftp事务
class BaseFtpTransaction {
public:
    friend FtpTransactionManager;
    DEFINE_PTR_CREATER(BaseFtpTransaction);

public:
    virtual int OnRequest(const FtpRequest::Ptr req,
                          FtpResponse::Ptr rsp){ return -1; }

protected:
    FtpSession::Ptr session_ = nullptr;  // 命令通道
};

class BaseDataSocketFtpTransaction: public BaseFtpTransaction {
public:
    ~BaseDataSocketFtpTransaction();
protected:
    bool CreateDataSocket();

    void CloseDataSocket();

protected:
    xco::Socket::Ptr data_socket = nullptr;  // 数据通道
};

class FtpTransactionManager {
    struct ITypeBucket {
        virtual ~ITypeBucket(){}
        virtual std::shared_ptr<BaseFtpTransaction> Create(){ return nullptr; }
    };
    template<typename Type>
    struct TypeBucket : ITypeBucket {
        typename BaseFtpTransaction::Ptr Create() {
            return Type::Create();
        }
    };
public:
    ~FtpTransactionManager();
public:
    int HandleRequest(const FtpRequest::Ptr req,
                      FtpResponse::Ptr rsp,
                      FtpSession::Ptr session);

    template<typename TransType>
    bool RegisterTransaction(const std::string& cmd) {
        auto& type_bucket = cmd_to_trans_type_bucket_[cmd];
        if (type_bucket) {
            return false;
        }
        type_bucket = new TypeBucket<TransType>;
        return true;
    }
private:
    std::unordered_map<std::string, ITypeBucket*> cmd_to_trans_type_bucket_;
};
#define FtpTransactionManagerSgt Singleton<FtpTransactionManager>::Instance()

// 注册FtpTransactionXXX
#define REGISTER_TRANSACTION(cmd) \
            static bool ret_register_##cmd = FtpTransactionManagerSgt.RegisterTransaction<FtpTransaction##cmd>(#cmd);

// 声明BaseFtpTransactionXXX
#define DEFINE_TRANSACTION(cmd)                             \
    class FtpTransaction##cmd : public BaseFtpTransaction { \
    public:                                                 \
        DEFINE_PTR_CREATER(FtpTransaction##cmd);            \
    public:                                                 \
        int OnRequest(const FtpRequest::Ptr req, FtpResponse::Ptr rsp) override;\
    };                                                      \
    REGISTER_TRANSACTION(cmd);

// 声明BaseDataSocketFtpTransactionXXX
#define DEFINE_SOCKET_TRANSACTION(cmd)                             \
    class FtpTransaction##cmd : public BaseDataSocketFtpTransaction{ \
    public:                                                 \
        DEFINE_PTR_CREATER(FtpTransaction##cmd);            \
    public:                                                 \
        int OnRequest(const FtpRequest::Ptr req, FtpResponse::Ptr rsp) override;\
    };                                                      \
    REGISTER_TRANSACTION(cmd);

// 事务实现
DEFINE_TRANSACTION(USER);
DEFINE_TRANSACTION(PASS);
DEFINE_TRANSACTION(SYST);
DEFINE_TRANSACTION(PORT);
DEFINE_TRANSACTION(PWD);
DEFINE_TRANSACTION(CWD);
DEFINE_TRANSACTION(CDUP);
DEFINE_TRANSACTION(QUIT);
DEFINE_SOCKET_TRANSACTION(LIST);
DEFINE_SOCKET_TRANSACTION(RETR);
DEFINE_SOCKET_TRANSACTION(STOR);
DEFINE_TRANSACTION(DELE);