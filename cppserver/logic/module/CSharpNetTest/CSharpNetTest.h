/* 
* File:   CSharpNetTest.h
* Author : mx
*
* Created on 2020-12-01 09:14:04
*/

#ifndef __CSharpNetTest_h__
#define __CSharpNetTest_h__

#include "header.h"

class CSharpNetTest : public iCSharpNetTest, public api::iTcpServer {
public:
    virtual ~CSharpNetTest() {}

    virtual bool initialize(api::iCore * core);
    virtual bool launch(api::iCore * core);
    virtual bool destroy(api::iCore * core);

	virtual api::iTcpSession* onMallocConnection(api::iCore* core, const char* remote_ip, const s32 remote_port);
	virtual void onError(api::iCore* core, api::iTcpSession* session);
	virtual void onRelease(api::iCore* core);
};

#endif //__CSharpNetTest_h__