//
// Created by Zach Lee on 2021/5/7.
//

#ifndef EXPLOSION_MODULE_PASSBLOADER_H
#define EXPLOSION_MODULE_PASSBLOADER_H

#include <string>
#include <PassBuilder/Api.h>
#include "PassDefines.h"

class PASS_API PassLoader {
public:
    PassLoader() {}
    ~PassLoader() {}

    static GraphInfo Load(const std::string& url);
};

#endif