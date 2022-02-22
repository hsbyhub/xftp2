/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：ftp_server_config.h
      	创 建 者：hsby
      	创建日期：2022/2/21
 *================================================================*/
#pragma once

#include "common.h"
#include <xco/common.h>
#include <unordered_map>

bool LoadAllConfig();
const std::string GetFtpServerConfigDir();
void SetFtpServerConfigDir(const std::string& path);

class BaseConfigLoader {
public:
    struct Element {
        template<typename T>
        const T GetAs(const std::string& key) const;
        std::unordered_map<std::string, std::string> data;
    };
public:
    ~BaseConfigLoader(){
        OnClear();
    }
public:
    bool Load(const std::string& file_name);
    virtual void OnClear() {}
    virtual bool OnReadElement(const Element& element) {return true;}
};

class FtpServerConfig : public BaseConfigLoader{
public:
    void OnClear() override;

    bool OnReadElement(const BaseConfigLoader::Element& element) override;

public:
    const std::string GetRootDir() const { return root_dir;}

private:
    std::string root_dir;
};
#define FtpServerConfigSgt Singleton<FtpServerConfig>::Instance()

class FtpUsersConfig : public BaseConfigLoader{
    struct Item {
        DEFINE_PTR_CREATER(Item);
        std::string name;       // 名字
        std::string pass;       // 密码
    };
public:
    void OnClear() override;

    bool OnReadElement(const BaseConfigLoader::Element& element) override;

    const Item::Ptr GetItem(const std::string& name) const;

private:
    std::unordered_map<std::string, Item::Ptr> user_to_item_;
};
#define FtpUsersConfigSgt Singleton<FtpUsersConfig>::Instance()