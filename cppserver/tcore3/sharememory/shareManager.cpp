#include "shareManager.h"
#include "shareMemory.h"

namespace tcore {
    api::iShareMemory * shareManager::createShareMemory(const std::string & name, const int size) {
        return shareMemory::create(name, size, false);
    }

    api::iShareMemory * shareManager::openShareMemory(const std::string & name) {
        return shareMemory::create(name, 0, true);
    }
}
