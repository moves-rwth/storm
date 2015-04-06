#ifndef STORM_MODELCHECKER_CHECKRESULT_H_
#define STORM_MODELCHECKER_CHECKRESULT_H_

#include <iostream>
#include <memory>

#include "src/storage/dd/DdType.h"
#include "src/logic/ComparisonType.h"

namespace storm {
    namespace modelchecker {
        // Forward-declare the existing subclasses.
        class QualitativeCheckResult;
        class QuantitativeCheckResult;
        class ExplicitQualitativeCheckResult;
        template <typename ValueType> class ExplicitQuantitativeCheckResult;
        template <storm::dd::DdType Type> class SymbolicQualitativeCheckResult;
        template <storm::dd::DdType Type> class SymbolicQuantitativeCheckResult;
        template <storm::dd::DdType Type> class HybridQuantitativeCheckResult;
        
        // The base class of all check results.
        class CheckResult {
        public:
            /*!
             * Filters the current result wrt. to the filter, i.e. only keeps the entries that are selected by the filter.
             * This means that the filter must be a qualitative result of proper type (symbolic/explicit).
             *
             * @param filter A qualitative result that serves as a filter.
             */
            virtual void filter(QualitativeCheckResult const& filter) = 0;
            
            // Methods to retrieve the actual type of the check result.
            virtual bool isExplicit() const;
            virtual bool isSymbolic() const;
            virtual bool isHybrid() const;
            virtual bool isQuantitative() const;
            virtual bool isQualitative() const;
            virtual bool isExplicitQualitativeCheckResult() const;
            virtual bool isExplicitQuantitativeCheckResult() const;
            virtual bool isSymbolicQualitativeCheckResult() const;
            virtual bool isSymbolicQuantitativeCheckResult() const;
            virtual bool isHybridQuantitativeCheckResult() const;
            virtual bool isResultForAllStates() const;
            
            QualitativeCheckResult& asQualitativeCheckResult();
            QualitativeCheckResult const& asQualitativeCheckResult() const;

            QuantitativeCheckResult& asQuantitativeCheckResult();
            QuantitativeCheckResult const& asQuantitativeCheckResult() const;
            
            ExplicitQualitativeCheckResult& asExplicitQualitativeCheckResult();
            ExplicitQualitativeCheckResult const& asExplicitQualitativeCheckResult() const;
            
            template<typename ValueType>
            ExplicitQuantitativeCheckResult<ValueType>& asExplicitQuantitativeCheckResult();

            template<typename ValueType>
            ExplicitQuantitativeCheckResult<ValueType> const& asExplicitQuantitativeCheckResult() const;
            
            template <storm::dd::DdType Type>
            SymbolicQualitativeCheckResult<Type>& asSymbolicQualitativeCheckResult();

            template <storm::dd::DdType Type>
            SymbolicQualitativeCheckResult<Type> const& asSymbolicQualitativeCheckResult() const;

            template <storm::dd::DdType Type>
            SymbolicQuantitativeCheckResult<Type>& asSymbolicQuantitativeCheckResult();

            template <storm::dd::DdType Type>
            SymbolicQuantitativeCheckResult<Type> const& asSymbolicQuantitativeCheckResult() const;

            template <storm::dd::DdType Type>
            HybridQuantitativeCheckResult<Type>& asHybridQuantitativeCheckResult();
            
            template <storm::dd::DdType Type>
            HybridQuantitativeCheckResult<Type> const& asHybridQuantitativeCheckResult() const;
            
            virtual std::ostream& writeToStream(std::ostream& out) const = 0;
        };
        
        std::ostream& operator<<(std::ostream& out, CheckResult& checkResult);
    }
}

#endif /* STORM_MODELCHECKER_CHECKRESULT_H_ */