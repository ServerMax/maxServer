#include "redisTransaction.h"
#include "rediser.h"

namespace tcore {
    void redisTransaction::pushCommand(const char* cmd) {
        _commands.push_back(redisCmd(cmd));
    }
    void redisTransaction::pushCommand(const char* cmd, const tlib::cargs& args) {
        _commands.push_back(redisCmd(cmd, args));
    }

    void redisTransaction::doTransacation(const u32 id, const fRdsTransactionCall& call, bool sync, const iContext context) {
        _rediser->doTransation(this, id, call, sync, context);
    }
}
