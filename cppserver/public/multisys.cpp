#include "multisys.h"

#include <stdio.h>
#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif 

#ifdef WIN32
#include <windows.h>

    void sleep(int millisecond) {
        Sleep(millisecond);
    }
#endif //WIN32

    void _assertionfail(const char * file, int line, const char * funname, const char * debug) {
#ifdef _DEBUG
        fflush(stdout);
        printf("assert : %s:%s:%d\nassert info:%s\n", file, funname, line, debug);
        fflush(stderr);
        assert(false);
#endif //_DEBUG
    }
#ifdef __cplusplus
};
#endif

#ifdef _DEBUG
#include <map>
#include "autolock.h"
struct oMemeryInfo {
    char _file[256];
    int _line;

    oMemeryInfo(const char * file, int line) {
        safesprintf(_file, sizeof(_file), "%s", file);
        _line = line;
    }

    oMemeryInfo & operator=(const oMemeryInfo & target) {
        memcpy(this, &target, sizeof(target));
        return *this;
    }
};


namespace debug {
    namespace memony {
        class tracer {
        public:
            static tracer * getInstance() {
                static tracer * static_self = nullptr;
                if (nullptr == static_self) {
                    static_self = ::new tracer;
                }

                return static_self;
            }

            inline void * alloc(size_t size, const char* file, const size_t line) {
                if (0 == size) {
                    tassert(false, "wtf new");
                    return nullptr;
                }

                void *p = malloc(size);
                autolocker(lock, _lock);
                _memery_info_map.insert(std::make_pair(p, oMemeryInfo(file, line)));
                return p;
            }

            inline void del(void * p) {
                free(p);

                autolocker(lock, _lock);
                auto itor = _memery_info_map.find(p);
                if (itor != _memery_info_map.end()) {
                    _memery_info_map.erase(itor);
                }
            }

            inline void trace() {
                for (auto i = _memery_info_map.begin(); i != _memery_info_map.end(); i++) {
                    printf("%s:%d\n", i->second._file, i->second._line);
                }
            }

        private:
            std::map<void *, oMemeryInfo> _memery_info_map;
            tlib::spinlock _lock;
        };
    }
}

void * operator new(size_t size, const char* file, const size_t line) {
    return debug::memony::tracer::getInstance()->alloc(size, file, line);
}

void * operator new[](size_t size, const char* file, const size_t line) {
    return operator new(size, file, line);
}

void operator delete(void * p) {
    debug::memony::tracer::getInstance()->del(p);
}

void operator delete[](void * pointer) {
    return operator delete(pointer);
}

void memory_trace() {
    debug::memony::tracer::getInstance()->trace();
}

#endif 
