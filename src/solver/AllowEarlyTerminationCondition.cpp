#include "AllowEarlyTerminationCondition.h"
#include "src/utility/vector.h"

namespace storm {
    namespace solver {

        template<typename ValueType>
        TerminateAfterFilteredSumPassesThresholdValue<ValueType>::TerminateAfterFilteredSumPassesThresholdValue(storm::storage::BitVector const& filter, ValueType threshold) :
        terminationThreshold(threshold), filter(filter)
        {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        bool TerminateAfterFilteredSumPassesThresholdValue<ValueType>::terminateNow(const std::vector<ValueType>& currentValues) const {
            assert(currentValues.size() >= filter.size());
            ValueType currentThreshold = storm::utility::vector::sum_if(currentValues, filter);
            
            return currentThreshold >= this->terminationThreshold;
            
            
        }
        
        template<typename ValueType>
        TerminateAfterFilteredExtremumPassesThresholdValue<ValueType>::TerminateAfterFilteredExtremumPassesThresholdValue(storm::storage::BitVector const& filter, ValueType threshold,  bool useMinimum) :
        terminationThreshold(threshold), filter(filter), useMinimumAsExtremum(useMinimum)
        {
            // Intentionally left empty.
        }

        template<typename ValueType>
        bool TerminateAfterFilteredExtremumPassesThresholdValue<ValueType>::terminateNow(const std::vector<ValueType>& currentValues) const {
            assert(currentValues.size() >= filter.size());
            
            ValueType initVal = terminationThreshold - 1;
            ValueType currentThreshold = useMinimumAsExtremum ? storm::utility::vector::max_if(currentValues, filter, initVal) : storm::utility::vector::max_if(currentValues, filter, initVal);
            
               return currentThreshold >= this->terminationThreshold;
            
            
        }
        
        
        template class TerminateAfterFilteredExtremumPassesThresholdValue<double>;
        template class TerminateAfterFilteredSumPassesThresholdValue<double>;
                
    }
}
