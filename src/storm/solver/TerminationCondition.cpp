#include "storm/solver/TerminationCondition.h"
#include "storm/utility/vector.h"

#include "storm/adapters/RationalFunctionAdapter.h"

#include "storm/utility/macros.h"

namespace storm {
    namespace solver {
        
        template<typename ValueType>
        bool NoTerminationCondition<ValueType>::terminateNow(std::vector<ValueType> const& currentValues, SolverGuarantee const& guarantee) const {
            return false;
        }
        
        template<typename ValueType>
        TerminateIfFilteredSumExceedsThreshold<ValueType>::TerminateIfFilteredSumExceedsThreshold(storm::storage::BitVector const& filter, ValueType const& threshold, bool strict) : threshold(threshold), filter(filter), strict(strict) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        bool TerminateIfFilteredSumExceedsThreshold<ValueType>::terminateNow(std::vector<ValueType> const& currentValues, SolverGuarantee const& guarantee) const {
            if (guarantee != SolverGuarantee::GreaterOrEqual) {
                return false;
            }
            
            STORM_LOG_ASSERT(currentValues.size() == filter.size(), "Vectors sizes mismatch.");
            ValueType currentThreshold = storm::utility::vector::sum_if(currentValues, filter);
            return strict ? currentThreshold > this->threshold : currentThreshold >= this->threshold;
        }
        
        template<typename ValueType>
        TerminateIfFilteredExtremumExceedsThreshold<ValueType>::TerminateIfFilteredExtremumExceedsThreshold(storm::storage::BitVector const& filter, bool strict, ValueType const& threshold, bool useMinimum) : TerminateIfFilteredSumExceedsThreshold<ValueType>(filter, threshold, strict), useMinimum(useMinimum) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        bool TerminateIfFilteredExtremumExceedsThreshold<ValueType>::terminateNow(std::vector<ValueType> const& currentValues, SolverGuarantee const& guarantee) const {
            if (guarantee != SolverGuarantee::GreaterOrEqual) {
                return false;
            }
            
            STORM_LOG_ASSERT(currentValues.size() == this->filter.size(), "Vectors sizes mismatch.");
            ValueType currentValue = useMinimum ? storm::utility::vector::min_if(currentValues, this->filter) : storm::utility::vector::max_if(currentValues, this->filter);
            return this->strict ? currentValue > this->threshold : currentValue >= this->threshold;
        }
        
        template<typename ValueType>
        TerminateIfFilteredExtremumBelowThreshold<ValueType>::TerminateIfFilteredExtremumBelowThreshold(storm::storage::BitVector const& filter, ValueType const& threshold, bool strict, bool useMinimum) : TerminateIfFilteredSumExceedsThreshold<ValueType>(filter, threshold, strict), useMinimum(useMinimum) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        bool TerminateIfFilteredExtremumBelowThreshold<ValueType>::terminateNow(std::vector<ValueType> const& currentValues, SolverGuarantee const& guarantee) const {
            if (guarantee != SolverGuarantee::LessOrEqual) {
                return false;
            }
            
            STORM_LOG_ASSERT(currentValues.size() == this->filter.size(), "Vectors sizes mismatch.");
            ValueType currentValue = useMinimum ? storm::utility::vector::min_if(currentValues, this->filter) : storm::utility::vector::max_if(currentValues, this->filter);
            return this->strict ? currentValue < this->threshold : currentValue <= this->threshold;
        }
        
        template class TerminateIfFilteredSumExceedsThreshold<double>;
        template class TerminateIfFilteredExtremumExceedsThreshold<double>;
        template class TerminateIfFilteredExtremumBelowThreshold<double>;
#ifdef STORM_HAVE_CARL
        template class TerminateIfFilteredSumExceedsThreshold<storm::RationalNumber>;
        template class TerminateIfFilteredExtremumExceedsThreshold<storm::RationalNumber>;
        template class TerminateIfFilteredExtremumBelowThreshold<storm::RationalNumber>;

#endif
    }
}
