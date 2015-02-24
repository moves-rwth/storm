#include "src/models/ModelBase.h"

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
    }
}