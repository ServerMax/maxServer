#include "tcpSession.h"

int tcpSession::onRecv(api::iCore* core, const char* content, const int size) {
	//收到数据后，会收到这个回调, 这个回调是需要return 你使用了多少数据的len
	if (size < sizeof(int)) {
		return 0;
	}
	int len = *(int*)content;

	if (len > size) {
		return 0;
	}

	trace(core, "recv data : %s", content + sizeof(int));
    return len;
}

void tcpSession::onConnected(api::iCore* core) {
    //连接成功后 会调用这个
    //连接成功后 随机向服务器发送几组数据 并且随机时间后自动断开连接
    int random = tools::rand(3);
	switch (random) {
	case 0: {
		const char* content = "Today we're on a journey to discovering the most popular baby girl names in 2015. According to Nameberry's list, this year parents choose both usual and unusual names for girls &";
		int len = (strlen(content) + 1) + sizeof(int);
		send(&len, sizeof(int));
		send(content, (strlen(content) + 1));
		break;
	}
	case 1: {
		const char* content = "As we enter the last month of 2015, BabyCenter has released its annual list of the most popular baby names. For the third year in a row, the top three names are Sophia";
		int len = (strlen(content) + 1) + sizeof(int);
		send(&len, sizeof(int));
		send(content, (strlen(content) + 1));
		break;
	}
	case 2: {
		const char* content = "As 2016 came to close, BabyCenter has released its annual list of most popular baby names. See the top 20 names for boys and girls below.";
		int len = (strlen(content) + 1) + sizeof(int);
		send(&len, sizeof(int));
		send(content, (strlen(content) + 1));
		break;
	}
    }

	start_timer(core, this, config::id::eTcpClientClose, 100 + tools::rand(100), 1, 100);
}

void tcpSession::onDisconnect(api::iCore* core) {
	core->killTimer(this, config::id::eTcpClientClose);
	tdel this;
	trace(core, "onDisconnect");
}

void tcpSession::onConnectFailed(api::iCore* core) {

}

void tcpSession::onStart(api::iCore* core, const s32 id, const api::iContext& context, const s64 tick) {

}

void tcpSession::onTimer(api::iCore* core, const s32 id, const api::iContext& context, const s64 tick) {
	switch (id) {
	case config::id::eTcpClientClose: {
		close();
		break;
	}
	}
}

void tcpSession::onEnd(api::iCore* core, const s32 id, const api::iContext& context, bool nonviolent, const s64 tick) {

}

void tcpSession::onPause(api::iCore* core, const s32 id, const api::iContext& context, const s64 tick) {

}

void tcpSession::onResume(api::iCore* core, const s32 id, const api::iContext& context, const s64 tick) {

}
