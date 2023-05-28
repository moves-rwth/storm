#include "boundFunc.h"

template <typename ValueType>
class oneSidedHoeffDingBound : public boundFunc<ValueType> {
   public:
    std::pair<ValueType,ValueType> INTERVAL(int totalSamples, int partialSample, double delta);
};

template <typename ValueType>
std::pair<ValueType,ValueType> oneSidedHoeffDingBound<ValueType>::INTERVAL(int totalSamples, int partialSample, double delta) {
    ValueType tmp = sqrt((log(delta / 2)) / (-2 * totalSamples));
    ValueType t = (ValueType)partialSample / (ValueType)totalSamples;
    return std::make_pair(t - tmp, 1);
}
