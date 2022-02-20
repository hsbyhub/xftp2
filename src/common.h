/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：echo_server.cc
      	创 建 者：汉森伯逸
      	创建日期：2022/2/19
 *================================================================*/
#pragma once

#include <string>
#include <boost/lexical_cast.hpp>

#define DEFINE_PTR_CREATER(type)                                    \
            typedef std::shared_ptr<type> Ptr;                      \
            template<typename ...ArgsType>                          \
            static type::Ptr Create(ArgsType ...args) {             \
                return std::shared_ptr<type>(new type(args...));    \
            }

template<typename T>
bool StringSplitToVectorAs(const std::string& str, char s, std::vector<T>& vec) {
    size_t pre = 0;
    for (size_t i = 0; i < str.size(); ++i) {
        if (str[i] == s || i == str.size() - 1) {
            vec.push_back(boost::lexical_cast<T>(&str[pre], i - pre));
            pre = i + 1;
        }
    }
    return true;
}

// 日志
void FormatDate(std::ostream& os);
int GetLogLevel();
void SetLogLevel(int level);
#define DEBUG 1
#define WARN  2
#define ERROR 3
#define FATAL 4
#define LOG_IF_LEVEL(level, msg)                                \
        if (level >= GetLogLevel()){                            \
             std::cout  << __FILE__ << ":" << __LINE__ << "|";  \
             FormatDate(std::cout); std::cout << "|";          \
             std::cout  << #level << "|"                        \
                        << msg << std::endl << std::flush;      \
        }
#define LOGDEBUG(msg)   LOG_IF_LEVEL(DEBUG, msg)
#define LOGWARN(msg)    LOG_IF_LEVEL(WARN, msg)
#define LOGERROR(msg)   LOG_IF_LEVEL(ERROR, msg)
#define LOGFATAL(msg)   LOG_IF_LEVEL(FATAL, msg)