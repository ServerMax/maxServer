#ifndef __redis_header_h__
#define __redis_header_h__

#include "api.h"
#include "tools.h"
#include "cthread.h"
#include "tpool.h"
#include "tqueue.h"
#include "autolock.h"
#include "hiredis/hiredis.h"

#include <set>

using namespace tcore::api;

namespace tcore {
    class rediser;
    extern std::set<rediser *> g_rediser_set;
}

#endif //__redis_header_h__
