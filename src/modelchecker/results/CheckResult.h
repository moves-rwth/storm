#ifndef STORM_MODELCHECKER_CHECKRESULT_H_
#define STORM_MODELCHECKER_CHECKRESULT_H_

#include <iostream>
#include <memory>

#include "src/storage/BitVector.h"
#include "src/logic/ComparisonType.h"

namespace storm {
    namespace modelchecker {
        // Forward-declare the existing subclasses.
        class QualitativeCheckResult;
        template <typename ValueType> class QuantitativeCheckResult;
        class ExplicitQualitativeCheckResult;
        template <typename ValueType> class ExplicitQuantitativeCheckResult;
        
        // The base class of all check results.
        class CheckResult {
        public:
            virtual CheckResult& operator&=(CheckResult const& other);
            virtual CheckResult& operator|=(CheckResult const& other);
            virtual void complement();
            virtual std::unique_ptr<CheckResult> compareAgainstBound(storm::logic::ComparisonType comparisonType, double bound) const;

            virtual bool isExplicit() const;
            virtual bool isQuantitative() const;
            virtual bool isQualitative() const;
            virtual bool isResultForAllStates() const;
            
            virtual bool isExplicitQualitativeCheckResult() const;
            virtual bool isExplicitQuantitativeCheckResult() const;
            
            QualitativeCheckResult& asQualitativeCheckResult();
            QualitativeCheckResult const& asQualitativeCheckResult() const;

            template<typename ValueType>
            QuantitativeCheckResult<ValueType>& asQuantitativeCheckResult();
            template<typename ValueType>
            QuantitativeCheckResult<ValueType> const& asQuantitativeCheckResult() const;
            
            ExplicitQualitativeCheckResult& asExplicitQualitativeCheckResult();
            ExplicitQualitativeCheckResult const& asExplicitQualitativeCheckResult() const;
            
            template<typename ValueType>
            ExplicitQuantitativeCheckResult<ValueType>& asExplicitQuantitativeCheckResult();

            template<typename ValueType>
            ExplicitQuantitativeCheckResult<ValueType> const& asExplicitQuantitativeCheckResult() const;
            
            virtual std::ostream& writeToStream(std::ostream& out) const = 0;
            virtual std::ostream& writeToStream(std::ostream& out, storm::storage::BitVector const& filter) const = 0;
        };
        
        std::ostream& operator<<(std::ostream& out, CheckResult& checkResult);
    }
}

#endif /* STORM_MODELCHECKER_CHECKRESULT_H_ */