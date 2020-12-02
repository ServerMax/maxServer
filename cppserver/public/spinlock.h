#ifndef __spinlock_h__
#define __spinlock_h__

#include <atomic>
#include "tools.h"

#pragma pack(1)
namespace tlib {
    class spinlock {
        std::atomic_flag _flag;

        u64 _lock_thread_id;
        int _reference;

#ifdef _DEBUG
        std::string _file;
        int _line;
#endif

    public:
#ifdef WIN32
#if ( _MSC_VER >= 1927)
		spinlock() : _lock_thread_id(0), _reference(0) { _flag._Storage = ATOMIC_FLAG_INIT; }
#else
		spinlock() : _lock_thread_id(0), _reference(0) { _flag._My_flag = ATOMIC_FLAG_INIT; }
#endif
#else
        spinlock() : _flag(ATOMIC_FLAG_INIT), _lock_thread_id(0), _reference(0) {}
#endif //WIN32

        inline void clear() {
            _lock_thread_id = 0;
            _reference = 0;
            _flag.clear(std::memory_order_release);
        }

        inline void lock(const char * file, const int line) {
            if (_lock_thread_id == 0 || _lock_thread_id != tools::system::getCurrentThreadID()) {
                while (_flag.test_and_set(std::memory_order_acquire)) {
                    int i = 100;
                }
                _lock_thread_id = tools::system::getCurrentThreadID();
            }
            _reference++;
#ifdef _DEBUG
            _file = file;
            _line = line;
            //printf("lock %s:%d %x : %lld : %d\n", _file.c_str(), _line, this, _lock_thread_id, _reference);
#endif
        }

        inline void unlock() {
            tassert(_lock_thread_id == tools::system::getCurrentThreadID(), "wtf");
            _reference--;
            if (0 == _reference) {
#ifdef _DEBUG
                //printf("unlock %s:%d %x : %lld : %d\n", _file.c_str(), _line, this, _lock_thread_id, _reference);
#endif
                _lock_thread_id = 0;
                _flag.clear(std::memory_order_release);
            }
        }
    };
}
#pragma pack()

#endif //__spinlock_h__
