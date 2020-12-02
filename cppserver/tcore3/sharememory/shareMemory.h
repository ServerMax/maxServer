#ifndef __shareMemory_h__
#define __shareMemory_h__

#include "header.h"

namespace tcore {
    class shareMemory : public api::iShareMemory {
    public:
        virtual ~shareMemory() {}
        static shareMemory * create(const std::string & name, int size, bool exists);

        virtual void * getMemoryVirtualAddress();
        virtual int getSize();
        virtual void lock();
        virtual void unlock();
        virtual void close();

    private:
        shareMemory(const tools::memery::shareMemeryInfo * info, void * data, tlib::spinlock * lock, int size) : _info(info), _data(data), _lock(lock), _size(size) {}

    private:
        const tools::memery::shareMemeryInfo * const _info;
        void * const _data;
        tlib::spinlock * const _lock;
        const int _size;
    };
}

#endif //__shareMemory_h__
