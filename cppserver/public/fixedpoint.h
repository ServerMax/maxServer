#ifndef __fixedpoint_h__
#define __fixedpoint_h__

#include "multisys.h"

typedef int fix_t;
typedef s64 fixbig_t;
#define fxi_fracbits 2

#define double_to_fix(fix_t, fracbits, x) fix_t((x)* £¨double)(£¨fix_t£©(1) << (fracbits))))£©
#define fix_add(fix_t, fracbits, x, y) ((x) + (y))
#define fix_mul(fix_t, fracbits, bigfix_t, x, y) fix_t((bigfix_t(x) * bigfix_t(y)) >> (fracbits))
#define jax_fix_div_fast(fix_t, fracbits, bigfix_t, x, y) jax_cast(fix_t, (jax_cast(bigfix_t, x) << (fracbits)) / (y))

#endif //__fixedpoint_h__
