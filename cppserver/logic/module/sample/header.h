/* 
* File:   header.h
* Author : max
*
* Created on 2020-12-02 16:17:10
*/

#ifndef __header_h__
#define __header_h__

#include "tools.h"
#include "iSample.h"

using namespace tcore;

extern api::iCore * g_core;

namespace config {
	enum id {
		eTimerTest1 = 1,
		eTimerTest2 = 2
	};

	const int second = 1000;
	const int minute = second * 60;
	const int hour = minute * 60;
}


#endif //__header_h__
