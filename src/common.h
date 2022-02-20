/*================================================================*
        Copyright (C) 2021 All rights reserved, www.hsby.link.
      	文件名称：echo_server.cc
      	创 建 者：汉森伯逸
      	创建日期：2022/2/19
 *================================================================*/
#pragma once

#define DEFINE_PTR_CREATER(type)                                    \
            typedef std::shared_ptr<type> Ptr;                      \
            template<typename ...ArgsType>                          \
            static type::Ptr Create(ArgsType ...args) {             \
                return std::shared_ptr<type>(new type(args...));    \
            }
