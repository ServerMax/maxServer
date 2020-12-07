/* 
* File:   sample.cpp
* Author : max
*
* Created on 2020-12-02 16:17:10
*/

#include "sample.h"

api::iCore * g_core = nullptr;

bool sample::initialize(api::iCore * core) {
    g_core = core;
    return true;
}

bool sample::launch(api::iCore * core) {
    return true;
}

bool sample::destroy(api::iCore * core) {
    return true;
}