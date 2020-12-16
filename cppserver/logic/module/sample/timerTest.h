/* 
* File:   timeTest.h
* Author : max
*
* Created on 2020-12-02 16:17:10
*/

#ifndef __timeTest_h__
#define __timeTest_h__

#include "header.h"

class timerTest : api::iTimer {
public:
	virtual ~timerTest() {}

	virtual void onStart(api::iCore* core, const s32 id, const api::iContext& context, const s64 tick);
	virtual void onTimer(api::iCore* core, const s32 id, const api::iContext& context, const s64 tick);
	virtual void onEnd(api::iCore* core, const s32 id, const api::iContext& context, bool nonviolent, const s64 tick);
	virtual void onPause(api::iCore* core, const s32 id, const api::iContext& context, const s64 tick);
	virtual void onResume(api::iCore* core, const s32 id, const api::iContext& context, const s64 tick);
};

#endif //__timeTest_h__