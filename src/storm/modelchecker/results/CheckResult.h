#ifndef STORM_MODELCHECKER_CHECKRESULT_H_
#define STORM_MODELCHECKER_CHECKRESULT_H_

#include <iostream>
#include <memory>

#include "storm/storage/dd/DdType.h"
#include "storm/logic/ComparisonType.h"

namespace storm {
    namespace modelchecker {
        // Forward-declare the existing subclasses.
        class QualitativeCheckResult;
        template<typename ValueType>
        class QuantitativeCheckResult;
        class ExplicitQualitativeCheckResult;
        
        template <typename ValueType>
        class ExplicitQuantitativeCheckResult;
        
        template <storm::dd::DdType Type>
        class SymbolicQualitativeCheckResult;
        
        template <storm::dd::DdType Type, typename ValueType>
        class SymbolicQuantitativeCheckResult;
        
        template <storm::dd::DdType Type, typename ValueType>
        class HybridQuantitativeCheckResult;
        
        template <typename ValueType>
        class ParetoCurveCheckResult;
        
        // The base class of all check results.
        class CheckResult {
        public:
            virtual ~CheckResult() = default;
            
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
            virtual bool isParetoCurveCheckResult() const;
            virtual bool isResultForAllStates() const;
            
            QualitativeCheckResult& asQualitativeCheckResult();
            QualitativeCheckResult const& asQualitativeCheckResult() const;

            template<typename ValueType>
            QuantitativeCheckResult<ValueType>& asQuantitativeCheckResult() {
                return static_cast<QuantitativeCheckResult<ValueType> &>(*this);
            }
            template<typename ValueType>
            QuantitativeCheckResult<ValueType> const& asQuantitativeCheckResult() const {
                return static_cast<QuantitativeCheckResult<ValueType> const&>(*this);
            }


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

            template <storm::dd::DdType Type, typename ValueType>
            SymbolicQuantitativeCheckResult<Type, ValueType>& asSymbolicQuantitativeCheckResult();

            template <storm::dd::DdType Type, typename ValueType>
            SymbolicQuantitativeCheckResult<Type, ValueType> const& asSymbolicQuantitativeCheckResult() const;

            template <storm::dd::DdType Type, typename ValueType>
            HybridQuantitativeCheckResult<Type, ValueType>& asHybridQuantitativeCheckResult();
            
            template <storm::dd::DdType Type, typename ValueType>
            HybridQuantitativeCheckResult<Type, ValueType> const& asHybridQuantitativeCheckResult() const;

            template <typename ValueType>
            ParetoCurveCheckResult<ValueType>& asParetoCurveCheckResult();
            
            template <typename ValueType>
            ParetoCurveCheckResult<ValueType> const& asParetoCurveCheckResult() const;
            
            virtual std::ostream& writeToStream(std::ostream& out) const = 0;
        };
        
        std::ostream& operator<<(std::ostream& out, CheckResult const& checkResult);
    }
}

#endif /* STORM_MODELCHECKER_CHECKRESULT_H_ */
