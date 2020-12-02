#ifndef __logfile_h__
#define __logfile_h__
#include "multisys.h"
#include <string>

namespace tcore {
    class logfile {
    public:
        bool open(const char * path, const char * name);
        bool isopen();
        void close();

        void write(const char * data);
        void flush();

        inline s64 getopentick() {
            tassert(_open_tick != 0, "wtf");
            return _open_tick;
        }

        logfile() : _open_tick(0), _file(nullptr) {}

    private:
        s64 _open_tick;
        FILE * _file;
    };
}
#endif //__logfile_h__
