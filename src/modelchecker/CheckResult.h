#ifndef STORM_MODELCHECKER_CHECKRESULT_H_
#define STORM_MODELCHECKER_CHECKRESULT_H_

#include <iostream>

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
            
            friend std::ostream& operator<<(std::ostream& out, CheckResult& checkResult);

            virtual bool isExplicit() const;
            virtual bool isQuantitative() const;
            virtual bool isQualitative() const;
            virtual bool isResultForAllStates() const;
            
            virtual bool isExplicitQualitativeCheckResult() const;
            virtual bool isExplicitQuantitativeCheckResult() const;
            
            ExplicitQualitativeCheckResult& asExplicitQualitativeCheckResult();
            ExplicitQualitativeCheckResult const& asExplicitQualitativeCheckResult() const;
            
            template<typename ValueType>
            ExplicitQuantitativeCheckResult<ValueType>& asExplicitQuantitativeCheckResult();

            template<typename ValueType>
            ExplicitQuantitativeCheckResult<ValueType> const& asExplicitQuantitativeCheckResult() const;
            
        protected:
            virtual std::ostream& writeToStream(std::ostream& out) const = 0;
        };
        
        std::ostream& operator<<(std::ostream& out, CheckResult& checkResult);
    }
}

#endif /* STORM_MODELCHECKER_CHECKRESULT_H_ */