#include "shareMemory.h"

namespace tcore {
    shareMemory * shareMemory::create(const std::string & name, int size, bool exists) {
        std::string path = std::string(tools::file::getApppath()) + "/shareMemery/";
        const tools::memery::shareMemeryInfo * info = tools::memery::getShareMemory(path, name, size + sizeof(s32) + sizeof(tlib::spinlock));
        if (nullptr == info) {
            error(core::getInstance(), "getShareMemory error");
            return nullptr;
        }

        void * lockaddr = (char *)info->_vm + sizeof(s32);
        void * dataaddr = (char *)lockaddr + sizeof(tlib::spinlock);

        tlib::spinlock * lock = nullptr;
        if (!exists) {
            *(int *)info->_vm = size;
            lock = new(lockaddr)tlib::spinlock();
        } else {
            size = *(int *)info->_vm;
            lock = (tlib::spinlock *)lockaddr;
        }

        return tnew shareMemory(info, dataaddr, lock, size);
    }

    void * shareMemory::getMemoryVirtualAddress() {
        return _data;
    }

    int shareMemory::getSize() {
        return _size;
    }

    void shareMemory::lock() {
        _lock->lock(__FILE__, __LINE__);
        //_lock->lock();
    }

    void shareMemory::unlock() {
        _lock->unlock();
    }

    void shareMemory::close() {
        tools::memery::returnShareMemory(_info);
    }
}
