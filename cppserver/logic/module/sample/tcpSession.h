/* 
* File:   tcpSession.h
* Author : max
*
* Created on 2020-12-02 16:17:10
*/

#ifndef __tcpSession_h__
#define __tcpSession_h__

#include "header.h"

class tcpSession : public api::iTcpSession, api::iTimer {
public:
	virtual ~tcpSession() {}

	virtual int onRecv(api::iCore* core, const char* content, const int size);
	virtual void onConnected(api::iCore* core);
	virtual void onDisconnect(api::iCore* core);
	virtual void onConnectFailed(api::iCore* core);

	virtual void onStart(api::iCore* core, const s32 id, const api::iContext& context, const s64 tick);
	virtual void onTimer(api::iCore* core, const s32 id, const api::iContext& context, const s64 tick);
	virtual void onEnd(api::iCore* core, const s32 id, const api::iContext& context, bool nonviolent, const s64 tick);
	virtual void onPause(api::iCore* core, const s32 id, const api::iContext& context, const s64 tick);
	virtual void onResume(api::iCore* core, const s32 id, const api::iContext& context, const s64 tick);
};

#endif //__tcpSession_h__