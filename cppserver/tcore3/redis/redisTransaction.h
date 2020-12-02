#ifndef __redisTransaction_h__
#define __redisTransaction_h__

#include "header.h"

namespace tcore {
    class rediser;

    struct redisCmd {
        const std::string _cmd;
        const tlib::cargs _args;

        redisCmd(const std::string& cmd) : _cmd(cmd) {}
        redisCmd(const std::string& cmd, const tlib::cargs& args) : _cmd(cmd), _args(args) {}
    };

    class redisTransaction : public iRedisTransaction {
    public:
        virtual ~redisTransaction() {}
        redisTransaction(rediser * rds) : _rediser(rds) {}

        virtual void pushCommand(const char* cmd);
        virtual void pushCommand(const char* cmd, const tlib::cargs& args);
        virtual void doTransacation(const u32 id, const fRdsTransactionCall& call, bool sync, const iContext context);

        const std::vector<redisCmd>& getCommands() { return _commands; }

    public:
        std::vector<redisCmd> _commands;
        rediser* const _rediser;
    };
}

#endif //__redisTransaction_h__