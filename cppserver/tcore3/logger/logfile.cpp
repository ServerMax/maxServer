#include "logfile.h"
#include "tools.h"

namespace tcore {
    bool logfile::open(const char * path, const char * name) {
        if (_file) {
            tassert(!_file, "WTF");
            return false;
        }

        std::string filepath;
        filepath += path;
        filepath += name;

        _file = fopen(filepath.c_str(), "ab+");
        if (nullptr == _file) {
            tassert(_file, "wtf");
            return false;
        }
        _open_tick = tools::time::getMillisecond();

        return true;
    }

    bool logfile::isopen() {
        return _file != nullptr;
    }

    void logfile::close() {
        if (!_file) {
            tassert(false, "wtf");
            return;
        }

        fflush(_file);
        fclose(_file);
        _file = nullptr;
        _open_tick = 0;
    }

    void logfile::write(const char * data) {
        if (_file) {
            s32 len = strlen(data);
            fwrite(data, len, 1, _file);
        }
    }

    void logfile::flush() {
        tassert(_file, "wtf");
        if (_file) {
            fflush(_file);
        }
    }
}
