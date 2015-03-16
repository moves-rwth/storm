#include "src/modelcheckers/result/SymbolicQualitativeCheckResult.h"

namespace storm {
    namespace modelcheckers {
        template <storm::dd::DdType Type>
        SymbolicQualitativeCheckResult(storm::dd::Dd<Type> const& values) {
            
        }
        
        template <storm::dd::DdType Type>
        bool isSymbolic() const {
            
        }

        template <storm::dd::DdType Type>
        bool isResultForAllStates() const {
            
        }
        
        template <storm::dd::DdType Type>
        bool isSymbolicQualitativeCheckResult() const {
            
        }
        
        template <storm::dd::DdType Type>
        QualitativeCheckResult& operator&=(QualitativeCheckResult const& other) {
            
        }

        template <storm::dd::DdType Type>
        QualitativeCheckResult& operator|=(QualitativeCheckResult const& other) {
            
        }

        template <storm::dd::DdType Type>
        void complement() {
            
        }
        
        template <storm::dd::DdType Type>
        storm::dd::Dd<Type> const& getTruthValuesVector() const {
            
        }
        
        template <storm::dd::DdType Type>
        std::ostream& writeToStream(std::ostream& out) const {
            
        }
        
        template <storm::dd::DdType Type>
        void filter(QualitativeCheckResult const& filter) {
            
        }
    }
}