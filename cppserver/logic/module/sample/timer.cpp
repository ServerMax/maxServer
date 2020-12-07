/* 
* File:   sample.h
* Author : max
*
* Created on 2020-12-02 16:17:10
*/

#ifndef __sample_h__
#define __sample_h__

#include "header.h"

class sample : public iSample {
public:
    virtual ~sample() {}

    virtual bool initialize(api::iCore * core);
    virtual bool launch(api::iCore * core);
    virtual bool destroy(api::iCore * core);
};

#endif //__sample_h__