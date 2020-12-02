#ifndef __geometry_h__
#define __geometry_h__

#include "multisys.h"

#define PI 3.1415926535897932385f

namespace geometry {
    class vector3 {
    public:
        float x;
        float y;
        float z;
    };

    namespace t2d{
        inline float calcAngle(const float x1, const float y1, const float x2, const float y2) {
            float angle = 0xfff;
            if (x1 == x2) {
                angle = (y1 < y2) ?  90 : 270;
            } else {
                if (y1 <= y2) {
                    angle = (x1 < x2) ? ::atan((y2 - y1) / (x2 - x1)) * 180 / PI : ::atan((y2 - y1) / (x2 - x1)) * 180 / PI + 180;
                } else {
                    angle = (x1 < x2) ? ::atan((y2 - y1) / (x2 - x1)) * 180 / PI + 360 : ::atan((y2 - y1) / (x2 - x1)) * 180 / PI + 180;
                }
            }
            return angle;
        }

        inline float calcDistance(const float x1, const float y1, const float x2, const float y2) {
            return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
        }

        inline bool isSamePosition(const float x1, const float y1, const float x2, const float y2) {
            return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1)) < 0.0001f;
        }
    }
}

#endif //__geometry_h__
