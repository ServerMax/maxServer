#ifndef __Navigation_h__
#define __Navigation_h__

#include "iNavigation.h"

class navigation : public iNavigation {
public:
	virtual bool initialize(tcore::api::iCore* core);
	virtual bool launch(tcore::api::iCore* core);
	virtual bool destroy(tcore::api::iCore* core);

    virtual iNavmesh * findNavmesh(const char * name);
};

#endif //__Navigation_h__
