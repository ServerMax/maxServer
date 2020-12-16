/* 
* File:   sample.cpp
* Author : max
*
* Created on 2020-12-02 16:17:10
*/

#include "sample.h"
#include "timerTest.h"

api::iCore * g_core = nullptr;

bool sample::initialize(api::iCore * core) {
    g_core = core;
    return true;
}

bool sample::launch(api::iCore * core) {
	//启动一个定时器测试, id为eTimerTest1, 2秒后启动, 定时器跳动10次, 每次间隔3.6秒
    start_timer(core, this, config::id::eTimerTest1, 2 * config::second, 10, 3.6 * config::second, 0);
    
	//启动一个定时器测试, id为eTimerTest2, 4.3秒后启动, 定时器跳动13次, 每次间隔1.2秒
	start_timer(core, this, config::id::eTimerTest2, 4.3 * config::second, 13, 1.2 * config::second, 0);
    return true;
}

bool sample::destroy(api::iCore * core) {
    return true;
}

void sample::onStart(api::iCore* core, const s32 id, const api::iContext& context, const s64 tick) {
    switch (id) {
    case config::id::eTimerTest1: {
        trace(core, "定时器开始执行, id : %d, time tick : %lld", id, tick);
        break;
    }
	case config::id::eTimerTest2: {
		trace(core, "定时器开始执行, id : %d, time tick : %lld", id, tick);
        break;
    }
    }
}

void sample::onTimer(api::iCore* core, const s32 id, const api::iContext& context, const s64 tick) {
	switch (id) {
	case config::id::eTimerTest1: {
		trace(core, "定时器跳动, id : %d, time tick : %lld", id, tick);
		break;
	}
	case config::id::eTimerTest2: {
		trace(core, "定时器跳动, id : %d, time tick : %lld", id, tick);
		break;
	}
	}
}

void sample::onEnd(api::iCore* core, const s32 id, const api::iContext& context, bool nonviolent, const s64 tick) {
	switch (id) {
	case config::id::eTimerTest1: {
		trace(core, "定时器结束, id : %d, time tick : %lld", id, tick);
		break;
	}
	case config::id::eTimerTest2: {
		trace(core, "定时器结束, id : %d, time tick : %lld", id, tick);
		break;
	}
	}
}

void sample::onPause(api::iCore* core, const s32 id, const api::iContext& context, const s64 tick) {

}

void sample::onResume(api::iCore* core, const s32 id, const api::iContext& context, const s64 tick) {

}
