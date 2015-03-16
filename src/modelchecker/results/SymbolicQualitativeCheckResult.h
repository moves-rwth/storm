#ifndef STORM_MODELCHECKER_SYMBOLICQUALITATIVECHECKRESULT_H_
#define STORM_MODELCHECKER_SYMBOLICQUALITATIVECHECKRESULT_H_

#include "src/storage/dd/DdType.h"
#include "src/modelchecker/results/QualitativeCheckResult.h"
#include "src/utility/OsDetection.h"

namespace storm {
    namespace modelchecker {
        template <storm::dd::DdType Type>
        class SymbolicQualitativeCheckResult : public QualitativeCheckResult {
        public:
            SymbolicQualitativeCheckResult() = default;
            SymbolicQualitativeCheckResult(storm::dd::Dd<Type> const& values);
            
            SymbolicQualitativeCheckResult(SymbolicQualitativeCheckResult const& other) = default;
            SymbolicQualitativeCheckResult& operator=(SymbolicQualitativeCheckResult const& other) = default;
#ifndef WINDOWS
            SymbolicQualitativeCheckResult(SymbolicQualitativeCheckResult&& other) = default;
            SymbolicQualitativeCheckResult& operator=(SymbolicQualitativeCheckResult&& other) = default;
#endif
            
            virtual bool isSymbolic() const override;
            virtual bool isResultForAllStates() const override;
            
            virtual bool isSymbolicQualitativeCheckResult() const override;
            
            virtual QualitativeCheckResult& operator&=(QualitativeCheckResult const& other) override;
            virtual QualitativeCheckResult& operator|=(QualitativeCheckResult const& other) override;
            virtual void complement() override;
            
            storm::dd::Dd<Type> const& getTruthValuesVector() const;
            
            virtual std::ostream& writeToStream(std::ostream& out) const override;
            
            virtual void filter(QualitativeCheckResult const& filter) override;
            
        private:
            // The values of the qualitative check result.
            storm::dd::Dd<Type> truthValues;
        };
    }
}

#endif /* STORM_MODELCHECKER_SYMBOLICQUALITATIVECHECKRESULT_H_ */