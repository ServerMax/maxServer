#ifndef __INavigation_h__
#define __INavigation_h__

#include "api.h"
#include "tools.h"
#include "geometry.h"

using namespace geometry;

#define nav_error -1
class iNavmesh {
public:
    virtual ~iNavmesh() {}
    virtual s32 findStraightPath(int layer, const vector3 & start, const vector3 & end, std::vector<vector3> & paths) const = 0;
    virtual s32 raycast(int layer, const vector3 & start, const vector3 & end, vector3 & hit) const = 0;
    virtual bool check(s32 layer, const vector3 & point) const = 0;
    virtual bool findnearPoint(const vector3 & center, vector3 & target, const float radius) const = 0;
};

class iNavigation : public iModule {
public:
    virtual ~iNavigation() {}
    virtual iNavmesh * findNavmesh(const char * name) = 0;
};

#endif //__INavigation_h__
