#include "tcpServer.h"
#include "tcpSession.h"

api::iTcpSession* tcpServer::onMallocConnection(api::iCore* core, const char* remote_ip, const s32 remote_port) {
	return tnew tcpSession();
}

void tcpServer::onError(api::iCore* core, api::iTcpSession* session) {

}

void tcpServer::onRelease(api::iCore* core) {

}
