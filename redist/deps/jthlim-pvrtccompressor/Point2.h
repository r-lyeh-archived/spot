#ifndef JTHLIM_POINT2_H
#define JTHLIM_POINT2_H

namespace Javelin {

template<typename T>
class Point2 {
public:
    T x;
    T y;

    Point2(int a, int b)
        : x(a)
        , y(b) {
    }
};

}

#endif
