#include "storm/models/ModelBase.h"

namespace storm {
    namespace models {
        ModelType ModelBase::getType() const {
            return modelType;
        }
        
        bool ModelBase::isSparseModel() const {
            return false;
        }
        
        bool ModelBase::isSymbolicModel() const {
            return false;
        }
        
        bool ModelBase::isOfType(storm::models::ModelType const& modelType) const {
            return this->getType() == modelType;
        }
        
        bool ModelBase::isNondeterministicModel() const {
            return this->isOfType(storm::models::ModelType::Mdp) || this->isOfType(storm::models::ModelType::MarkovAutomaton);
        }
        
        bool ModelBase::supportsParameters() const {
            return false;
        }

        bool ModelBase::hasParameters() const {
            return false;
        }

        bool ModelBase::isExact() const {
            return false;
        }
    }
}
