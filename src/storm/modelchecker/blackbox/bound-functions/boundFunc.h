#include <utility> 
#include <stdint.h>
#include <iostream>


/*!
 * Abstract Base Class for the function INTERVAL
 */
template <typename ValueType>
class BoundFunc {
   public:
    /*!
     * Calculates the lower and upper bound for a transition for the eMDP
     * @param totalSamples : total Samples for one Action generated in Simulate
     * @param partialSample : Samples for (state,transition,state) generated in Simulate
     * @param delta : Inconfidence value delta
     * @return : ValueTypePair with upper and lower bound
     */
    virtual std::pair<ValueType,ValueType> INTERVAL(int64_t totalSamples, int64_t partialSample, double delta) = 0; //abstract method
};

template <typename ValueType>
ValueType clamp0(ValueType x) {
        if(0 <= x && x <= 1)  
            return x;
        return 0;
}

template <typename ValueType>
ValueType clamp1(ValueType x) {
        if(0 <= x && x <= 1)  
            return x;
        return 1;
}

template <typename ValueType>
class HoeffDingBound : public BoundFunc<ValueType> {
   public:
    std::pair<ValueType,ValueType> INTERVAL(int64_t totalSamples, int64_t partialSample, double delta) {
        ValueType bound_width = sqrt((log(delta / 2)) / (-2 * totalSamples));
        ValueType median = (ValueType)partialSample / (ValueType)totalSamples;
        return std::make_pair(median - bound_width, median + bound_width);
    }
};

template <typename ValueType>
class OneSidedHoeffDingBound : public BoundFunc<ValueType> {
   public:
    std::pair<ValueType,ValueType> INTERVAL(int64_t totalSamples, int64_t partialSample, double delta) {
        ValueType bound_width = sqrt((log(delta / 2)) / (-2 * totalSamples));
        ValueType median = (ValueType)partialSample / (ValueType)totalSamples;
        return std::make_pair(median - bound_width, 1);
    } 
};






