#include "timerTest.h"

void timerTest::onStart(api::iCore* core, const s32 id, const api::iContext& context, const s64 tick) {
	trace(core, "定时器启动, id : %d, time tick %lld", id, tick);
}

void timerTest::onTimer(api::iCore* core, const s32 id, const api::iContext& context, const s64 tick) {
	trace(core, "定时器跳动, id : %d, time tick %lld", id, tick);
}

void timerTest::onEnd(api::iCore* core, const s32 id, const api::iContext& context, bool nonviolent, const s64 tick) {
	trace(core, "定时器结束, id : %d, time tick %lld", id, tick);
}

void timerTest::onPause(api::iCore* core, const s32 id, const api::iContext& context, const s64 tick) {

}

void timerTest::onResume(api::iCore* core, const s32 id, const api::iContext& context, const s64 tick) {

}
