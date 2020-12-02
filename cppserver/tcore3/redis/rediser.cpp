#include "rediser.h"
#include "core.h"
#include "redisTransaction.h"

#define increase_redis_reference(reference) { \
    int ref = (reference) + 1; \
    tools::memery::safeMemcpy((void *)&(reference), sizeof(reference), &ref, sizeof(ref)); \
}

#define reduce_redis_increase(reference) { \
    int ref = (reference) - 1; \
    tools::memery::safeMemcpy((void *)&(reference), sizeof(reference), &ref, sizeof(ref)); \
}

namespace tcore {
    namespace timer {
        namespace id {
            enum {
                queue_count_check,
            };
        }

        namespace config {
            static const int queue_count_check_interval = 60000;
        }
    }

    rediser::~rediser() {
        if (_context) {
            redisFree(_context);
            _context = nullptr;
        }
        core::getInstance()->removeLooper(this);
        core::getInstance()->killTimer(this, timer::id::queue_count_check);

        g_rediser_set.erase(this);
    }

    rediser::rediser(const char * ip, const s32 port, const char * passwd) : _addr(ip, port), _passwd(passwd), _context(nullptr) {
        core::getInstance()->insertLooper(this);
        start_timer(core::getInstance(), this, timer::id::queue_count_check, timer::config::queue_count_check_interval, forever, timer::config::queue_count_check_interval, 0);
        start();

        g_rediser_set.insert(this);
    }

    void rediser::completerSet(const u32 id, const char * key, iRedisCompleter * completer, const void * data, const u32 len, bool sync, const iContext context) {
        if (sync) {
            autolocker(lock, _lock);
            checkContext();
            redisReply * res = (redisReply *)redisCommand(_context, "set %s %b", key, data, (size_t)len);
            bool b = (res && res->type == REDIS_REPLY_STATUS && tools::memery::safeStrcmp(res->str, "OK")) ? true : false;
            completer->onSetComplete(id, b, key, (char *)data, len, context);
            if (res) {
                freeReplyObject(res);
            }
        } else {
            increase_redis_reference(completer->_reference);
            void * async_data = tmalloc(len);
            tools::memery::safeMemcpy(async_data, len, data, len);

            _opt_queue.push(tnew oOpt(id, key, async_set, async_data, len, context, completer, nullptr, nullptr, nullptr));
        }
    }

    void rediser::completerGet(const u32 id, const char * key, iRedisCompleter * completer, bool sync, const iContext context) {
        if (sync) {
            autolocker(lock, _lock);
            checkContext();
            redisReply * res = (redisReply *)redisCommand(_context, "get %s", key);
            bool b = (res && res->type == REDIS_REPLY_STRING) ? true : false;
            completer->onGetComplete(id, b, key, res->str, res->len, context);
            if (res) {
                freeReplyObject(res);
            }
        } else {
            increase_redis_reference(completer->_reference);
            _opt_queue.push(tnew oOpt(id, key, async_get, nullptr, 0, context, completer, nullptr, nullptr, nullptr));
        }
    }

    void rediser::completerDel(const u32 id, const char * key, iRedisCompleter * completer, bool sync, const iContext context) {
        if (sync) {
            autolocker(lock, _lock);
            checkContext();
            redisReply * res = (redisReply *)redisCommand(_context, "del %s", key);
            bool b = (res && res->type == REDIS_REPLY_INTEGER && res->integer == 1) ? true : false;
            completer->onDelComplete(id, b, key, context);
            if (res) {
                freeReplyObject(res);
            }
        } else {
            increase_redis_reference(completer->_reference);
            _opt_queue.push(tnew oOpt(id, key, async_del, nullptr, 0, context, completer, nullptr, nullptr, nullptr));
        }
    }

    void rediser::callerSet(const u32 id, const char * key, const fRedisCall & call, const void * data, const u32 len, bool sync, const iContext context) {
        if (sync) {
            autolocker(lock, _lock);
            checkContext();
            redisReply * res = (redisReply *)redisCommand(_context, "set %s %b", key, data, (size_t)len);
            bool b = (res && res->type == REDIS_REPLY_STATUS && tools::memery::safeStrcmp(res->str, "OK")) ? true : false;
            if (call) {
                call(id, b, key, (const char *)data, len, context);
            }
            if (res) {
                freeReplyObject(res);
            }
        } else {
            void * async_data = tmalloc(len);
            tools::memery::safeMemcpy(async_data, len, data, len);

            _opt_queue.push(tnew oOpt(id, key, async_set, async_data, len, context, nullptr, call, nullptr, nullptr));
        }
    }

    void rediser::callerGet(const u32 id, const char * key, const fRedisCall& call, bool sync, const iContext context) {
        if (sync) {
            autolocker(lock, _lock);
            checkContext();
            redisReply * res = (redisReply *)redisCommand(_context, "get %s", key);
            bool b = (res && (res->type == REDIS_REPLY_STRING || res->type == REDIS_REPLY_NIL)) ? true : false;
            if (res) {
                call(id, b, key, res->str, res->len, context);
            } else {
                call(id, b, key, nullptr, 0, context);
            }

            if (res) {
                freeReplyObject(res);
            }
        } else {
            _opt_queue.push(tnew oOpt(id, key, async_get, nullptr, 0, context, nullptr, call, nullptr, nullptr));
        }
    }

    void rediser::callerDel(const u32 id, const char * key, const fRedisCall& call, bool sync, const iContext context) {
        if (sync) {
            autolocker(lock, _lock);
            checkContext();
            redisReply * res = (redisReply *)redisCommand(_context, "del %s", key);
            bool b = (res && res->type == REDIS_REPLY_INTEGER && res->integer == 1) ? true : false;
            call(id, b, key, nullptr, 0, context);
            if (res) {
                freeReplyObject(res);
            }
        } else {
            _opt_queue.push(tnew oOpt(id, key, async_del, nullptr, 0, context, nullptr, call, nullptr, nullptr));
        }
    }

    iRedisTransaction* rediser::createTransation() {
        return tnew redisTransaction(this);
    }

    inline void pushRelpyToResults(redisReply* reply, tlib::cargs& results) {
        if (nullptr == reply) {
            results << tlib::carg(tlib::eCargType::__nil__);
        }
        else {
            switch (reply->type) {
            case REDIS_REPLY_STRING: {
                results << std::string(reply->str, reply->len);
                break;
            }
            case REDIS_REPLY_ARRAY: {
                for (auto i = 0; i < reply->elements; i++) {
                    pushRelpyToResults(reply->element[i], results);
                }
                break;
            }
            case REDIS_REPLY_INTEGER: {
                results << reply->integer;
                break;
            }
            case REDIS_REPLY_NIL: {
                results << tlib::carg(tlib::eCargType::__nil__);
                break;
            }
            case REDIS_REPLY_STATUS: {
                results << std::string(reply->str, reply->len);
                break;
            }
            case REDIS_REPLY_ERROR: {
                results << tlib::carg(tlib::eCargType::__nil__);
                break;
            }
            default:
                tassert(false, "type out of control");
                break;
            }
        }
    }

    void rediser::doTransation(redisTransaction* tras, const u32 id, const fRdsTransactionCall& call, bool sync, const iContext context) {
        if (sync) {
            tlib::cargs results;

            int count = tras->getCommands().size();
            for (auto i = 0; i < count; i++) {
                int argc = tras->getCommands()[i]._args.getCount();

                char** cmd = tnew char* [argc + 1];
                size_t* len = tnew size_t[argc + 1];

                cmd[0] = (char *)tras->getCommands()[i]._cmd.c_str();
                len[0] = (size_t)tras->getCommands()[i]._cmd.length();

                for (auto index = 0; index < tras->getCommands()[i]._args.getCount(); index++) {
                    cmd[index + 1] = (char *)tras->getCommands()[i]._args.getData(index, len[index + 1]);
                }

                int ret = redisAppendCommandArgv(_context, argc + 1, (const char**)cmd, len);
                redisReply* reply;
                redisGetReply(_context, (void **)&reply); //reply for set
                pushRelpyToResults(reply, results);
                freeReplyObject(reply);
                tdel[] cmd;
                tdel[] len;
            }

            call(id, results, context);
            tdel tras;
        }
        else {
            _opt_queue.push(tnew oOpt(id, "", async_transaction, nullptr, 0, context, nullptr, nullptr, tras, call));
        }
    }

    const oAddress * rediser::getAddrInfo() {
        return &_addr;
    }

    void rediser::onTimer(iCore * core, const s32 id, const iContext & context, const s64 tick) {
        switch (id) {
        case timer::id::queue_count_check: {
            trace(core, "redis async queue count %d", _opt_queue.count());
            break;
        }
    }
    }

    void rediser::terminate() {

    }

    void rediser::run() {
        oOpt * opt;
        tlib::cargs results;
        while (true) {
            while (_opt_queue.tryPull(opt)) {
                autolocker(lock, _lock);
                checkContext();
                redisReply * res = nullptr;
                switch (opt->_type) {
                case eOptType::async_set: {
                    res = (redisReply *)redisCommand(_context, "set %s %b", opt->_word.c_str(), opt->_data, (size_t)opt->_len);
                    break;
                }
                case eOptType::async_get: {
                    res = (redisReply *)redisCommand(_context, "get %s", opt->_word.c_str());
                    break;
                }
                case eOptType::async_del: {
                    res = (redisReply *)redisCommand(_context, "del %s", opt->_word.c_str());
                    break;
                }
                case eOptType::async_transaction: {
                    int count = opt->_transaction->getCommands().size();
                    for (auto i = 0; i < count; i++) {
                        int argc = opt->_transaction->getCommands()[i]._args.getCount();

                        char** cmd = tnew char* [argc + 1];
                        size_t* len = tnew size_t[argc + 1];

                        cmd[0] = (char*)opt->_transaction->getCommands()[i]._cmd.c_str();
                        len[0] = (size_t)opt->_transaction->getCommands()[i]._cmd.length();

                        for (auto index = 0; index < opt->_transaction->getCommands()[i]._args.getCount(); index++) {
                            cmd[index + 1] = (char*)opt->_transaction->getCommands()[i]._args.getData(index, len[index + 1]);
                        }

                        int ret = redisAppendCommandArgv(_context, argc + 1, (const char**)cmd, len);
                        redisReply* reply;
                        redisGetReply(_context, (void**)& reply); //reply for set
                        pushRelpyToResults(reply, results);
                        freeReplyObject(reply);

                        tdel[] cmd;
                        tdel[] len;
                    }

                    break;
                }
                default:
                    tassert(false, "unknown redis opt type");
                    break;
                }

                _res_queue.push(tnew oResult(opt, res, results));
                results.clear();
            }
            msleep(10);
        }
    }

    void rediser::deal() {
        oResult * result;
        while (_res_queue.tryPull(result)) {
            if (result->_opt->_completer) {
                reduce_redis_increase(result->_opt->_completer->_reference);
            }

            switch (result->_opt->_type) {
            case eOptType::async_set: {
                bool b = (result->_redisReply && result->_redisReply->type == REDIS_REPLY_STATUS && tools::memery::safeStrcmp(result->_redisReply->str, "OK")) ? true : false;
                if (result->_opt->_completer) {
                    result->_opt->_completer->onSetComplete(result->_opt->_id, b, result->_opt->_word.c_str(), (char *)result->_opt->_data, result->_opt->_len, result->_opt->_context);
                }

                if (result->_opt->_caller) {
                    result->_opt->_caller(result->_opt->_id, b, result->_opt->_word.c_str(), (char *)result->_opt->_data, result->_opt->_len, result->_opt->_context);
                }

                if (result->_redisReply) {
                    freeReplyObject(result->_redisReply);
                }
                tfree(result->_opt->_data);
                break;
            }
            case eOptType::async_get: {
                bool b = (result->_redisReply && (result->_redisReply->type == REDIS_REPLY_STRING || result->_redisReply->type == REDIS_REPLY_NIL)) ? true : false;
                if (result->_opt->_completer) {
                    if (b) {
                        result->_opt->_completer->onGetComplete(result->_opt->_id, b, result->_opt->_word.c_str(), result->_redisReply->str, result->_redisReply->len, result->_opt->_context);
                    }
                    else {
                        result->_opt->_completer->onGetComplete(result->_opt->_id, b, result->_opt->_word.c_str(), nullptr, 0, result->_opt->_context);
                    }
                }

                if (result->_opt->_caller) {
                    if (b) {
                        result->_opt->_caller(result->_opt->_id, b, result->_opt->_word.c_str(), result->_redisReply->str, result->_redisReply->len, result->_opt->_context);
                    }
                    else {
                        result->_opt->_caller(result->_opt->_id, b, result->_opt->_word.c_str(), nullptr, 0, result->_opt->_context);
                    }
                }

                if (result->_redisReply) {
                    freeReplyObject(result->_redisReply);
                }
                break;
            }
            case eOptType::async_del: {
                bool b = (result->_redisReply && result->_redisReply->type == REDIS_REPLY_INTEGER && result->_redisReply->integer == 1) ? true : false;
                if (result->_opt->_completer) {
                    if (b) {
                        result->_opt->_completer->onDelComplete(result->_opt->_id, b, result->_opt->_word.c_str(), result->_opt->_context);
                    }
                }

                if (result->_opt->_caller) {
                    result->_opt->_caller(result->_opt->_id, b, result->_opt->_word.c_str(), nullptr, 0, result->_opt->_context);
                }

                if (result->_redisReply) {
                    freeReplyObject(result->_redisReply);
                }
                break;
            }
            case eOptType::async_transaction: {
                result->_opt->_rdsTransCall(result->_opt->_id, result->_results, result->_opt->_context);
                tdel result->_opt->_transaction;
                break;
            }
            default: {
                tassert(false, "unknown redis opt type");
                break;
            }
            }
            tdel result->_opt;
            tdel result;
            result = nullptr;
        }
    }

    bool rediser::tryConnect() {
        if (_context) {
            redisFree(_context);
        }

        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 10000;

        _context = redisConnectWithTimeout(_addr._ip.c_str(), _addr._port, tv);

        if (_context->err != REDIS_OK) {
            error(core::getInstance(), "connect redis error, code %d, desc %s", _context->err, _context->errstr);
            redisFree(_context);
            _context = nullptr;
            return false;
        }

        if (_passwd.length() > 0) {
            redisReply * res = (redisReply *)redisCommand(_context, "auth %s", _passwd.c_str());
            bool b = (res && res->type == REDIS_REPLY_STATUS && tools::memery::safeStrcmp(res->str, "OK")) ? true : false;
            if (res) {
                freeReplyObject(res);
            }

            if (false == b) {
                error(core::getInstance(), "connect redis error, auth error, check ur passwd");
                redisFree(_context);
                _context = nullptr;
                return false;
            }
        }

        return true;
    }

    void rediser::connect() {
        if (_context) {
            redisFree(_context);
            _context = nullptr;
        }

        struct timeval tv;
        tv.tv_sec = 1;
        tv.tv_usec = 0;

        _context = redisConnectWithTimeout(_addr._ip.c_str(), _addr._port, tv);
        while (_context->err != REDIS_OK) {
            error(core::getInstance(), "connect redis error, code %d, desc %s", _context->err, _context->errstr);
            msleep(10);
            redisFree(_context);
            _context = redisConnectWithTimeout(_addr._ip.c_str(), _addr._port, tv);
        }

        if (_passwd.length() > 0) {
            redisReply * res = (redisReply *)redisCommand(_context, "auth %s", _passwd.c_str());
            bool b = (res && res->type == REDIS_REPLY_STATUS && tools::memery::safeStrcmp(res->str, "OK")) ? true : false;
            if (res) {
                freeReplyObject(res);
            }

            if (false == b) {
                while (true) {
                    error(core::getInstance(), "connect redis error, auth error, check ur passwd");
                    msleep(1000);
                }
            }
        }
    }

    void rediser::checkContext() {
        if (!_context || _context->err != 0) {
            if (_context) {
                redisFree(_context);
                _context = nullptr;
            }
            connect();
        }
    }
}
