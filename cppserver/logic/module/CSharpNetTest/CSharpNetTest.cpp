/* 
* File:   CSharpNetTest.cpp
* Author : mx
*
* Created on 2020-12-01 09:14:04
*/

#include "CSharpNetTest.h"

api::iCore * g_core = nullptr;

class TcpClient : public api::iTcpSession, public api::iTimer {
public:
    virtual int onRecv(api::iCore* core, const char* content, const int size) {
        //send(content, size);
        return size;
    }

    virtual void onConnected(api::iCore* core) {
        trace(core, "onConnected");
        start_timer(core, this, 0, 100, forever, 100, 0);
    }

    virtual void onDisconnect(api::iCore* core) {
        trace(core, "onDisconnect");
        core->killTimer(this, 0);
    }

    virtual void onConnectFailed(api::iCore* core) {
        tdel this;
    }

    virtual void onStart(api::iCore* core, const s32 id, const api::iContext& context, const s64 tick) {

    }

    virtual void onTimer(api::iCore* core, const s32 id, const api::iContext& context, const s64 tick) {

        int rd = tools::rand(50);
        std::string str;
        for (int i = 0; i < rd; i++) {
            str.append(tools::intAsString(i));
            str.append(" ");
        }

        tester a;
        a.set_content(str);
        a.set_tick(tick);

        int len = a.ByteSize();
        char* temp = (char*)alloca(len);
        if (a.SerializePartialToArray(temp, len)) {
            send(&len, sizeof(len));
            send(temp, len);
        }
    }

    virtual void onEnd(api::iCore* core, const s32 id, const api::iContext& context, bool nonviolent, const s64 tick) {

    }

    virtual void onPause(api::iCore* core, const s32 id, const api::iContext& context, const s64 tick) {

    }

    virtual void onResume(api::iCore* core, const s32 id, const api::iContext& context, const s64 tick) {

    }
};

bool CSharpNetTest::initialize(api::iCore * core) {
    g_core = core;
    return true;
}

bool CSharpNetTest::launch(api::iCore * core) {
    core->launchTcpServer(this, "0.0.0.0", 12345, 4096, 4096);
    return true;
}

bool CSharpNetTest::destroy(api::iCore * core) {
    return true;
}

api::iTcpSession* CSharpNetTest::onMallocConnection(api::iCore* core, const char* remote_ip, const s32 remote_port) {
    return tnew TcpClient;
}

void CSharpNetTest::onError(api::iCore* core, api::iTcpSession* session) {

}

void CSharpNetTest::onRelease(api::iCore* core) {

}
