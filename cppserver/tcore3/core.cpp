#include "core.h"
#include "instance.h"
#include "logger/logger.h"
#include "timer/timermgr.h"
#include "httper/httper.h"
#include "logic/logic.h"
#include "sharememory/shareManager.h"
#include "redis/redis.h"

#include "tinyxml/tinyxml2.h"

#include <unordered_map>

namespace tcore {
    using namespace api;

    static core * static_core = nullptr;
    static unordered_map<std::string, std::string> static_args_map;

    iModule * core::findModule(const std::string & name) {
        return logic::getInstance()->findModule(name.c_str());
    }

    const char * core::getEnv() {
        return _env.c_str();
    }

    void core::parseArgs(int argc, const char ** argv) {
        for (int i = 1; i < argc; ++i) {
            if (strncmp(argv[i], "--", 2) == 0) {
                const char * start = argv[i] + 2;
                const char * equal = strstr(start, "=");
                if (equal != nullptr) {
                    std::string name(start, equal);
                    std::string val(equal + 1);
                    static_args_map[name] = val;
                } else if (strlen(argv[i]) > 2) {
                    static_args_map[argv[i] + 2] = "";
                }
            } else {
                //errorlog
            }
        }
    }

    const char * core::getArgs(const char * name) {
        auto itor = static_args_map.find(name);
        if (itor == static_args_map.end()) {
            return nullptr;
        }

        return itor->second.c_str();
    }

    core * core::getInstance() {
        if (nullptr == static_core) {
            static_core = tnew core;
        }

        return static_core;
    }

    bool core::launch() {
        _env = getArgs("env");

        getNetInstance();
        httper::getInstance();
        timermgr::getInstance();
        logger::getInstance();
        logic::getInstance();

        start();
        return true;
    }

    bool core::launchUdpSession(iUdpSession * session, const char * ip, const s32 port) {
        return getNetInstance()->launchUdpSession(session, ip, port);
    }

    bool core::launchTcpSession(iTcpSession * session, const char * ip, const int port, const int max_ss, const int max_rs) {
        return getNetInstance()->launchTcpSession(session, ip, port, max_ss, max_rs);
    }

    bool core::launchTcpServer(iTcpServer * server, const char * ip, const int port, const int max_ss, const int max_rs) {
        return getNetInstance()->launchTcpServer(server, ip, port, max_ss, max_rs);
    }

    iHttpRequest * core::getHttpRequest(const u64 delivery, const s64 id, const char * url, iHttpResponse * response, const iContext & context) {
        return httper::getInstance()->getHttpRequest(delivery, id, url, response, context);
    }

    void core::startTimer(iTimer * timer, const s32 id, s64 delay, s32 count, s64 interval, const iContext context, const char * file, const s32 line){
        timermgr::getInstance()->startTimer(timer, id, context, delay, count, interval, file, line);
    }

    void core::killTimer(iTimer * timer, const s32 id, const iContext context){
        timermgr::getInstance()->killTimer(timer, id, context);
    }

    void core::pauseTimer(iTimer * timer, const s32 id, const iContext context){
        timermgr::getInstance()->pauseTimer(timer, id, context);
    }

    void core::resumeTimer(iTimer * timer, const s32 id, const iContext context){
        timermgr::getInstance()->resumeTimer(timer, id, context);
    }

    void core::traceTimer() {

    }

    iShareMemory * core::createShareMemory(const std::string & name, const int size) {
        return shareManager::createShareMemory(name, size);
    }

    iShareMemory * core::openShareMemory(const std::string & name) {
        return shareManager::openShareMemory(name);
    }

    void core::connectRedis(const char * ip, const s32 port, const char * passwd, const fConnectRedisCall call, const iContext context) {
        redis::getInstance()->connectRedis(ip, port, passwd, call, context);
    }

    void core::insertLooper(iLooper* looper) {
        _looper.insert(looper);
    }

    void core::removeLooper(iLooper * looper) {
        _looper.erase(looper);
    }

    void core::pushAsyncJober(iAsyncer* asyncer) {

    }

    void core::logSync(const s64 tick, const char * log, const bool echo) {
        logger::getInstance()->logSync(tick, log, echo);
    }

    void core::logAsync(const s64 tick, const char * log, const bool echo) {
        logger::getInstance()->logAsync(tick, log, echo);
    }

    void core::setSyncFilePrefix(const char * prefix) {
        logger::getInstance()->setSyncFilePrefix(prefix);
    }

    void core::setAsyncFilePrefix(const char * prefix) {
        logger::getInstance()->setAsyncFilePrefix(prefix);
    }

    void core::terminate() {
        _state = tlib::eThreadState::stopping;
    }

    void core::run() {
        _state = tlib::eThreadState::working;
        while (tlib::eThreadState::working == _state) {
            iAsyncer* asyncer = nullptr;
            while (_asyncJober.tryPull(asyncer)) {
                asyncer->asyncDo();
                _asyncDone.push(asyncer);
            }
            msleep(10);
        }
    }

    void core::loop() {
        getNetInstance()->deal();
        timermgr::getInstance()->update(5);
        httper::getInstance()->update();
        redis::getInstance()->loop();

        for (auto i = _looper.begin(); i != _looper.end(); i++) {
            (*i)->deal();
        }

        iAsyncer* asyncer = nullptr;
        while (_asyncDone.tryPull(asyncer)) {
            asyncer->done();
        }
    }

    void core::exit() {
        httper::getInstance()->shutdown();
        redis::getInstance()->shutdown();
    }

}
