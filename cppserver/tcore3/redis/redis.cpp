#include "redis.h"
#include "rediser.h"

namespace tcore {
    std::set<rediser *> g_rediser_set;

    static redis * static_self = nullptr;

    redis * redis::getInstance() {
        if (nullptr == static_self) {
            static_self = tnew redis;
            static_self->start();
        }
        return static_self;
    }

    void redis::terminate() {

    }

    void redis::run() {
        while (true) {
            oCreatRediserReq req;
            while (_req_queue.tryPull(req)) {
                bool b = req._rediser->tryConnect();
                _aws_queue.push(oCreatRediserAws(b, req));
            }

            msleep(10);
        }
    }

    void redis::loop() {
        oCreatRediserAws aws;
        while (_aws_queue.tryPull(aws)) {
            if (aws._success) {
                aws._req._callback(aws._success, aws._req._rediser, aws._req._context);
            } else {
                aws._req._callback(aws._success, nullptr, aws._req._context);
                tdel aws._req._rediser;
            }
        }
    }

    void redis::shutdown() {
        while (0 != _req_queue.count()) {
            msleep(10);
        }

        for (auto i = g_rediser_set.begin(); i != g_rediser_set.end(); i++) {
            while (0 != (*i)->getOptQueueSize()) {
                msleep(10);
            }
        }

        msleep(1000);
        loop();
    }

    void redis::connectRedis(const char * ip, const s32 port, const char * passwd, const fConnectRedisCall call, const iContext context) {
        rediser * rds = tnew rediser(ip, port, passwd);
        rds->connect();
        call(true, rds, context);
    }
}
