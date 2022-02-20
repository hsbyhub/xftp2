/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：echo_server.cc
      	创 建 者：汉森伯逸
      	创建日期：2022/2/19
 *================================================================*/
#pragma once

#include "ftp_user_info.h"

class FtpTransactionManager;

enum FtpCmd {
    kFcNull = 0,
    kFcUSER = 1,
    kFcPASS = 2,
    kFcPORT = 3,
    kFcSYST = 4,
    kFcPWD  = 5,
    kFcLIST = 6,
    kFcCWD  = 7,
};


// 基础的Ftp事务
class BaseFtpTransaction {
public:
    friend FtpTransactionManager;
    DEFINE_PTR_CREATER(BaseFtpTransaction);

public:
    virtual int OnRequest(const FtpRequest::Ptr req,
                          FtpResponse::Ptr rsp){ return -1; }

protected:
    FtpUserInfo::Ptr    user_info_      = nullptr;  // 用户信息
    FtpSession::Ptr     cmd_session     = nullptr;  // 命令通道
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
                      FtpSession::Ptr session,
                      FtpUserInfo::Ptr user_info);

    template<typename TransType>
    bool RegisterTransaction(int cmd) {
        auto& type_bucket = cmd_to_trans_type_bucket_[cmd];
        if (type_bucket) {
            return false;
        }
        type_bucket = new TypeBucket<TransType>;
        return true;
    }

    int GetCmdFromStr(const std::string& cmd_str);

    bool SetCmdFromStr(const std::string& cmd_str, int cmd);

private:
    std::unordered_map<int, ITypeBucket*> cmd_to_trans_type_bucket_;
    std::map<std::string, int> str_to_cmd_;
};
#define FtpTransactionManagerSgt Singleton<FtpTransactionManager>::Instance()

template<typename Trans>
inline bool RegisterTransactionHelper(int cmd, const char* cmd_str) {
    FtpTransactionManagerSgt.SetCmdFromStr(cmd_str, cmd);
    FtpTransactionManagerSgt.RegisterTransaction<Trans>(cmd);
    return true;
}

// 注册FtpTransactionXXX
#define REGISTER_TRANSACTION(cmd) \
            static bool ret_register_##cmd = RegisterTransactionHelper<FtpTransaction##cmd>(kFc##cmd, #cmd);

// 声明FtpTransactionXXX
#define DEFINE_TRANSACTION(cmd)                             \
    class FtpTransaction##cmd : public BaseFtpTransaction { \
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
DEFINE_TRANSACTION(LIST);
DEFINE_TRANSACTION(CWD);
