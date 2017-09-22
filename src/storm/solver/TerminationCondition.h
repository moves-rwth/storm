#ifndef ALLOWEARLYTERMINATIONCONDITION_H
#define	ALLOWEARLYTERMINATIONCONDITION_H

#include <vector>

#include "storm/solver/SolverGuarantee.h"
#include "storm/storage/BitVector.h"

namespace storm {
    namespace solver {
        template<typename ValueType>
        class TerminationCondition {
        public:
            /*!
             * Retrieves whether the guarantee provided by the solver for the current result is sufficient to terminate.
             */
            virtual bool terminateNow(std::vector<ValueType> const& currentValues, SolverGuarantee const& guarantee = SolverGuarantee::None) const = 0;
        };
        
        template<typename ValueType>
        class NoTerminationCondition : public TerminationCondition<ValueType> {
        public:
            virtual bool terminateNow(std::vector<ValueType> const& currentValues, SolverGuarantee const& guarantee = SolverGuarantee::None) const override;
        };
        
        template<typename ValueType>
        class TerminateIfFilteredSumExceedsThreshold : public TerminationCondition<ValueType> {
        public:
            TerminateIfFilteredSumExceedsThreshold(storm::storage::BitVector const& filter, ValueType const& threshold, bool strict);
            
            bool terminateNow(std::vector<ValueType> const& currentValues, SolverGuarantee const& guarantee = SolverGuarantee::None) const;

        protected:
            ValueType threshold;
            storm::storage::BitVector filter;
            bool strict;
        };
        
        template<typename ValueType>
        class TerminateIfFilteredExtremumExceedsThreshold : public TerminateIfFilteredSumExceedsThreshold<ValueType>{
        public:
            TerminateIfFilteredExtremumExceedsThreshold(storm::storage::BitVector const& filter, bool strict, ValueType const& threshold, bool useMinimum);
            
            bool terminateNow(std::vector<ValueType> const& currentValue, SolverGuarantee const& guarantee = SolverGuarantee::None) const;
            
        protected:
            bool useMinimum;
        };
        
        template<typename ValueType>
        class TerminateIfFilteredExtremumBelowThreshold : public TerminateIfFilteredSumExceedsThreshold<ValueType>{
        public:
            TerminateIfFilteredExtremumBelowThreshold(storm::storage::BitVector const& filter, ValueType const& threshold, bool strict, bool useMinimum);
            
            bool terminateNow(std::vector<ValueType> const& currentValue, SolverGuarantee const& guarantee = SolverGuarantee::None) const;
            
        protected:
            bool useMinimum;
        };
    }
}





#endif	/* ALLOWEARLYTERMINATIONCONDITION_H */

