#ifndef __core_h__
#define __core_h__

#include "api.h"
#include <set>
#include "tqueue.h"
#include "cthread.h"

namespace tcore {
    using namespace api;

    class core : public iCore, public tlib::cthread {
    public:
        virtual ~core() {}

        virtual iModule * findModule(const std::string & name);
        virtual const char * getEnv();

        void parseArgs(int argc, const char ** argv);
        virtual const char * getArgs(const char * name);

        virtual void setCorename(const char * name) { _core_name = name; }
        virtual const char * getCorename() { return _core_name.c_str(); }

        static core * getInstance();
        bool launch();

        virtual bool launchUdpSession(iUdpSession * session, const char * ip, const s32 port);
        virtual bool launchTcpSession(iTcpSession * session, const char * ip, const int port, int max_ss, int max_rs);
        virtual bool launchTcpServer(iTcpServer * server, const char * ip, const int port, int max_ss, int max_rs);

        virtual iHttpRequest * getHttpRequest(const u64 delivery, const s64 id, const char * url, iHttpResponse * response, const iContext & context);

        virtual void startTimer(iTimer * timer, const s32 id, s64 delay, s32 count, s64 interval, const iContext context, const char * file, const s32 line);
        virtual void killTimer(iTimer * timer, const s32 id, const iContext context = (s64)0);
        virtual void pauseTimer(iTimer * timer, const s32 id, const iContext context = (s64)0);
        virtual void resumeTimer(iTimer * timer, const s32 id, const iContext context = (s64)0);
        virtual void traceTimer();
        
        virtual iShareMemory * createShareMemory(const std::string & name, const int size);
        virtual iShareMemory * openShareMemory(const std::string & name);

        virtual void connectRedis(const char * ip, const s32 port, const char * passwd, const fConnectRedisCall call, const iContext context);

        virtual void insertLooper(iLooper * looper);
        virtual void removeLooper(iLooper * looper);
        virtual void pushAsyncJober(iAsyncer* asyncer);

        virtual void logSync(const s64 tick, const char * log, const bool echo);
        virtual void logAsync(const s64 tick, const char * log, const bool echo);
        virtual void setSyncFilePrefix(const char * prefix);
        virtual void setAsyncFilePrefix(const char * prefix);

        inline bool isShutdown() { return _is_shutdown; }
        virtual void shutdown() { _is_shutdown = true; }

        virtual void terminate(); // not safe
        virtual void run();

        void loop();
        void exit();
    private:
        core() : _state(tlib::eThreadState::stoped), _is_shutdown(false) {}

    private:
        std::string _core_name;
        std::string _env;
        std::set<iLooper*> _looper;
        tlib::tqueue<iAsyncer*> _asyncJober;
        tlib::tqueue<iAsyncer*> _asyncDone;
        tlib::eThreadState _state;

        bool _is_shutdown;
    };
}

#endif //__core_h__
