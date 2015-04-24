#ifndef JTHLIM_INTERVAL_H
#define JTHLIM_INTERVAL_H

namespace Javelin {

template<typename T>
class Interval {
public:
    T min;
    T max;

    Interval() {
    }

    Interval<T> &operator|=(const T &x) {
        min.SetMin(x); 
        max.SetMax(x);
        return *this;
    }
};

}

#endif
