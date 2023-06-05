#include "hoeffDingBound.h"
#include "oneSidedHoeffDingBound.h"
#include <cstdio>

/*!
 * File to test Bound Functions
 */

int main() {
    hoeffDingBound<double> bound;
    std::pair<double,double> p = bound.INTERVAL(30, 14, 0.5);
    printf("Test two sided %f, %f\n", p.first, p.second);

    oneSidedHoeffDingBound<double> oneBound;
    std::pair<double,double> p2 = oneBound.INTERVAL(30, 14, 0.5);
    printf("Test one sided %f, %f\n", p2.first, p2.second);
}
