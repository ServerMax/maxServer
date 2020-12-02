#ifndef __redis_h__
#define __redis_h__

#include "header.h"

namespace tcore {
    class rediser;

    struct oCreatRediserReq {
        const fConnectRedisCall _callback;
        const iContext _context;
        rediser * _rediser;

        oCreatRediserReq() : _callback(nullptr), _context(-1), _rediser(nullptr) {}
        oCreatRediserReq(const fConnectRedisCall fun, const iContext context, rediser * rediser) : _callback(fun), _context(context), _rediser(rediser) {}
        oCreatRediserReq(const oCreatRediserReq & o) : _callback(nullptr), _context(-1) {
            tools::memery::safeMemcpy(this, sizeof(oCreatRediserReq), &o, sizeof(o));
        }

        oCreatRediserReq & operator=(const oCreatRediserReq & req) {
            tools::memery::safeMemcpy(this, sizeof(oCreatRediserReq), &req, sizeof(req));
            return *this;
        }
    };

    struct oCreatRediserAws {
        bool _success;
        oCreatRediserReq _req;

        oCreatRediserAws() {}
        oCreatRediserAws(bool success, oCreatRediserReq & req) : _success(success), _req(req) {}

        oCreatRediserAws & operator=(const oCreatRediserAws & aws) {
            tools::memery::safeMemcpy(this, sizeof(oCreatRediserAws), &aws, sizeof(aws));
            return *this;
        }
    };

    class redis : public tlib::cthread {
    public:
        virtual ~redis() {}
        static redis * getInstance();

        virtual void terminate(); // not safe
        virtual void run();

        void loop();
        void shutdown();

        void connectRedis(const char * ip, const s32 port, const char * passwd, const fConnectRedisCall call, const iContext context);

    private:
        tlib::tqueue<oCreatRediserReq> _req_queue;
        tlib::tqueue<oCreatRediserAws> _aws_queue;
    };
}

#endif //__redis_h__s
