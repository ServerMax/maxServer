#ifndef __shareManager_h__
#define __shareManager_h__

#include "header.h"

namespace tcore {
    class shareManager {
    public:
        virtual ~shareManager() {}

        static api::iShareMemory * createShareMemory(const std::string & name, const int size);
        static api::iShareMemory * openShareMemory(const std::string & name);
    };
}

#endif //__shareManager_h__
