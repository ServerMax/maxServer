#include "logger.h"
#include "tools.h"
#ifdef WIN32
#include <windows.h>
#endif //WIN32

namespace tcore {
#define time_out_for_cut_file 5 * 60 * 60 * 1000
#define read_count 64

    logger * logger::getInstance() {
        static logger * s_logger = nullptr;
        if (nullptr == s_logger) {
            s_logger = tnew logger;
            s_logger->launch();
        }

        return s_logger;
    }

    bool logger::launch() {
        setSyncFilePrefix(tools::int64AsString(tools::system::getCurrentProcessId()).c_str());
        setAsyncFilePrefix(tools::int64AsString(tools::system::getCurrentProcessId()).c_str());

        start();
        _thread_status = STATUS_STARTING;
        return true;
    }


    void logger::shutdown() {
        terminate();
        tdel this;
    }

    void logger::logSync(const s64 tick, const char * log, const bool echo) {
        std::string timestr = tools::time::getTimeString(tick);
        _sync_file.write(timestr.c_str());
        _sync_file.write("|");
        _sync_file.write(log);
        _sync_file.write("\n");
        _sync_file.flush();

        if (echo) {
            printf(timestr.c_str());
            printf(" | ");
            printf("%s\n", log);
        }
    }

    void logger::logAsync(const s64 tick, const char * log, const bool echo) {
        logunit * lu = tnew logunit;

        lu->_tick = tick;
        lu->_log = log;
        lu->_echo = echo;
        lu->_has_data = true;

        _queue.push(lu);
    }

    void logger::setSyncFilePrefix(const char * prefix) {
        _sync_prefix = prefix;
        if (_sync_file.isopen()) {
            _sync_file.close();
        }

        std::string path = tools::file::getApppath();
        path.append("/log/");

        tools::file::mkdir(path.c_str());
        tassert(!_sync_file.isopen(), "sync log file created, fuck");

        if (!_sync_file.isopen()) {
            std::string name;
            name.append(_sync_prefix).append("_").append(tools::time::getCurrentTimeString("sync_%4d_%02d_%02d_%02d_%02d_%02d.log"));

            if (!_sync_file.open(path.c_str(), name.c_str())) {
                tassert(false, "wtf");
                printf("open log file : %s error\n", name.c_str());
            }
        }
    }

    void logger::setAsyncFilePrefix(const char * prefix) {
        _async_prefix = prefix;
        if (_async_file.isopen()) {
            _async_file.close();
        }

        std::string path = tools::file::getApppath();
        path.append("/log/");

        tools::file::mkdir(path.c_str());
        tassert(!_async_file.isopen(), "sync log file created, fuck");

        std::string name;
        name.append(_async_prefix.c_str()).append("_").append(tools::time::getCurrentTimeString("async_%4d_%02d_%02d_%02d_%02d_%02d.log"));

        if (!_async_file.open(path.c_str(), name.c_str())) {
            tassert(false, "wtf");
            printf("open log file : %s error\n", name.c_str());
            return;
        }
    }

    void logger::terminate() {
        tassert(_thread_status == STATUS_STARTING, "wtf");
        if (_thread_status == STATUS_STARTING) {
            _thread_status = STATUS_STOPPING;
            while (_thread_status != STATUS_STOPPED) {
                msleep(1);
            }
        }
    }

    void logger::run() {
        while (true) {
            while (_async_prefix == "") {
                msleep(1);
            }

            if (!_async_file.isopen()) {
                msleep(1);
                continue;
            }
            
            s64 tick = tools::time::getMillisecond();
            if (tick - _async_file.getopentick() >= time_out_for_cut_file) {
                _async_file.close();
                std::string name;
                name.append( _async_prefix).append("_").append(tools::time::getCurrentTimeString("async_%4d_%02d_%02d_%02d_%02d_%02d.log"));

                std::string path;
                path.append(tools::file::getApppath()).append("/log/");

                if (!_async_file.open(path.c_str(), name.c_str())) {
                    tassert(false, "wtf");
                    printf("open log file : %s error\n", name.c_str());
                    return;
                }
                printf("log file cut %lld\n", tick);
            }

            logunit * lu = nullptr;
            s32 count = 0;
            while (_queue.tryPull(lu) && count++ <= read_count) {
                _async_file.write(tools::time::getTimeString(lu->_tick).c_str());
                _async_file.write("|");
                _async_file.write(lu->_log.c_str());
                _async_file.write("\n");
                tdel lu;
            }
            _async_file.flush();

            if (_thread_status == STATUS_STOPPING) {
                _thread_status = STATUS_STOPPED;
                return;
            }

            if (0 == count) {
                msleep(1);
            }
        }
    }
}
