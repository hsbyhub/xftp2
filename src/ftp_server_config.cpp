/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：ftp_server_config.cpp
      	创 建 者：hsby
      	创建日期：2022/2/21
 *================================================================*/

#include "ftp_server_config.h"
#include <fstream>

template<typename T>
const T BaseConfigLoader::Element::GetAs(const std::string &key) const {
    auto it = data.find(key);
    if (it == data.end()) {
        return T();
    }
    try {
        return boost::lexical_cast<T>(it->second);
    } catch (...) {
    }
    return T();
}

bool BaseConfigLoader::Load(const std::string &file_name) {
    auto file_path = FtpServerConfigSgt.GetConfigDir() + "/" + file_name;
    std::ifstream ifs(file_path);
    if (!ifs) {
        return false;
    }

    for (std::string line;std::getline(ifs, line);) {
        Element element;
        std::vector<std::string> pair_strs;
        StringSplitToVectorAs(line, '|', pair_strs);
        for (auto pair_str : pair_strs) {
            std::vector<std::string> pair;
            StringSplitToVectorAs(pair_str, ':', pair);
            if (pair.size() != 2) {
                continue;
            }
            element.data[pair[0]] = pair[1];
        }
        if (!OnReadElement(element)) {
            return false;
        }
    }
    return true;
}

void FtpUsersConfig::OnClear() {
    user_to_item_.clear();
}

bool FtpUsersConfig::OnReadElement(const BaseConfigLoader::Element &element) {
    auto item = Item::Create();
    item->name = element.GetAs<std::string>("name");
    item->pass = element.GetAs<std::string>("pass");
    user_to_item_[item->name] = item;
    return true;
}

const FtpUsersConfig::Item::Ptr FtpUsersConfig::GetItem(const std::string& name) const {
    return GetFromMap(user_to_item_, name);
}