#ifndef __cthread_h__
#define __cthread_h__

#include "multisys.h"

namespace tlib {
    enum eThreadState {
        stoped = 0,
        working,
        stopping,
    };

    class cthread {
    public:
        virtual ~cthread() {}

        //interface
    public:
        bool start(s32 threadcount = 1);

        virtual void terminate() = 0; // not safe
        virtual void run() = 0;

    protected:
        eThreadState _status;
    };
}

#endif //__cthread_h__
