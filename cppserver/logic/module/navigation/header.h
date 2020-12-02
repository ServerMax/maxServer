/*
* File:   header.h
* Author : mx
*
* Created on 2020-12-01 09:14:04
*/

#ifndef __navigation_header_h__
#define __navigation_header_h__

#include "tools.h"
#include "geometry.h"
#include "iNavigation.h"
#include "test.pb.h"

#include <vector>

using namespace tcore;

extern api::iCore* g_core;

#define safe_release_array(ar) delete[] ar;ar=nullptr

static void vector3ToFloatArray(const geometry::vector3 & pos, OUT float ary[3]) {
    ary[0] = pos.x;
    ary[1] = pos.y;
    ary[2] = pos.z;
}

typedef std::vector<vector3> oVector3Array;

typedef std::vector<std::string> oNameArray;

#endif //__NavigationHeader_h__
