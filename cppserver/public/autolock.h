#ifndef __autolock_h__
#define __autolock_h__

#include "spinlock.h"

namespace tlib {
    template<class t>
    class autolock {
    public:
        autolock(t * lock, const char * file, const int line) {
            tassert(lock != nullptr, "mutexlock point null");
            _lock = lock;
            _lock->lock(file, line);
            //_lock->lock();
        }

        void free() {
            tassert(_lock != nullptr, "mutexlock point null");
            _lock->unlock();
        }

        ~autolock() {
            _lock->unlock();
        }

    private:
        t * _lock;
    };
}

#define autolocker(name, lock) tlib::autolock<tlib::spinlock> name(&lock, __FILE__, __LINE__)

#endif //defined __autolock_h__

