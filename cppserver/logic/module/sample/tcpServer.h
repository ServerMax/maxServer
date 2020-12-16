/* 
* File:   tcpServer.h
* Author : max
*
* Created on 2020-12-02 16:17:10
*/

#ifndef __tcpServer_h__
#define __tcpServer_h__

#include "header.h"

class tcpServer : public api::iTcpServer {
public:
	virtual ~tcpServer() {}

	virtual api::iTcpSession* onMallocConnection(api::iCore* core, const char* remote_ip, const s32 remote_port);
	virtual void onError(api::iCore* core, api::iTcpSession* session);
	virtual void onRelease(api::iCore* core);

};

#endif //__tcpServer_h__