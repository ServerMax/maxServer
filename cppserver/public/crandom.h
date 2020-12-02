#ifndef __crandom_h__
#define __crandom_h__

#include "tools.h"

class crandom {
public:
    crandom() : _seed(tools::time::getMillisecond()) {}

    inline void setSeed(const u32 seed) {
        _seed = seed;
    }

    inline u32 rand(u32 range) {
        if (0 == range) { return 0; }
        _seed = (((_seed * 214013L + 2531011L) >> 16) & 0x7fff);
        return _seed % range;
    }

private:
    u32 _seed;
};

#endif //__crandom_h__
