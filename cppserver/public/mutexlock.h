#ifndef __mutexlock_h__
#define __mutexlock_h__

#include "multisys.h"

#pragma pack(1)
namespace tlib{
#ifdef WIN32
#ifndef _WINDOWS_  
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif //_WINDOWS_
#include <synchapi.h>
    class mutexlock {
    public:
        mutexlock() {
            ::InitializeCriticalSection(&m_lock);
        }

        ~mutexlock() {
            ::DeleteCriticalSection(&m_lock);
        }

        void clear() {
            ::DeleteCriticalSection(&m_lock);
            ::InitializeCriticalSection(&m_lock);
        }

        void lock() {
            ::EnterCriticalSection(&m_lock);
        }

        void unlock() {
            ::LeaveCriticalSection(&m_lock);
        }

        bool trylock() {
            return ::TryEnterCriticalSection(&m_lock);
        }

    private:
        CRITICAL_SECTION m_lock;
    };
#else
    class mutexlock {
    public:
        mutexlock() {
            pthread_mutex_init(&m_lock, nullptr);
        }

        ~mutexlock() {
            pthread_mutex_destroy(&m_lock);
        }
        
        void clear() {
            ::pthread_mutex_destroy(&m_lock);
            ::pthread_mutex_init(&m_lock, nullptr);
        }
        
        void lock() {
            pthread_mutex_lock(&m_lock);
        }

        void unlock() {
            pthread_mutex_unlock(&m_lock);
        }

        bool trylock() {
            return !pthread_mutex_trylock(&m_lock);
        }

    private:
        pthread_mutex_t m_lock;
    };
#endif //WIN32
}
#pragma pack()

#endif //__mutexlock_h__

