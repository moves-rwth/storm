#include "boundFunc.h"
#include <cmath>

template <typename ValueType>
class hoeffDingBound : public boundFunc<ValueType> {
   public:
    std::pair<ValueType,ValueType> INTERVAL(int totalSamples, int partialSample, double delta);
};

template <typename ValueType>
std::pair<ValueType,ValueType> hoeffDingBound<ValueType>::INTERVAL(int totalSamples, int partialSample, double delta) {
    ValueType tmp = sqrt((log(delta / 2)) / (-2 * totalSamples));
    ValueType t = (ValueType)partialSample / (ValueType)totalSamples;
    return std::make_pair(t - tmp, t + tmp);
}