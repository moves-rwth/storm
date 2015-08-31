#ifndef ALLOWEARLYTERMINATIONCONDITION_H
#define	ALLOWEARLYTERMINATIONCONDITION_H

#include <vector>
#include "src/storage/BitVector.h"


namespace storm {
    namespace solver {
        template<typename ValueType>
        class AllowEarlyTerminationCondition {
            public:
                virtual bool terminateNow(std::vector<ValueType> const& currentValues) const = 0;
        };
        
        template<typename ValueType>
        class NoEarlyTerminationCondition :  public AllowEarlyTerminationCondition<ValueType> {
            public:
                bool terminateNow(std::vector<ValueType> const& currentValues) const { return false; }
        };
        
        template<typename ValueType>
        class TerminateAfterFilteredSumPassesThresholdValue :  public AllowEarlyTerminationCondition<ValueType> {
            public:
                TerminateAfterFilteredSumPassesThresholdValue(storm::storage::BitVector const& filter, ValueType threshold);
                bool terminateNow(std::vector<ValueType> const& currentValues) const;
            
            protected:
                ValueType terminationThreshold;
                storm::storage::BitVector filter;
        };
        
        template<typename ValueType>
        class TerminateAfterFilteredExtremumPassesThresholdValue :  public AllowEarlyTerminationCondition<ValueType>{
        public:
            TerminateAfterFilteredExtremumPassesThresholdValue(storm::storage::BitVector const& filter, ValueType threshold, bool useMinimum);
            bool terminateNow(std::vector<ValueType> const& currentValue) const;
            
        protected:
            ValueType terminationThreshold;
            storm::storage::BitVector filter;
            bool useMinimumAsExtremum;
        };
    }
}





#endif	/* ALLOWEARLYTERMINATIONCONDITION_H */

