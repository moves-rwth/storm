#include "src/storage/jani/Model.h"

namespace storm {
    namespace jani {
        
        Model::Model(ModelType const& modelType, uint64_t version) : modelType(modelType), version(version) {
            // Intentionally left empty.
        }
        
        bool Model::isValid(bool logDebug) const {
            // TODO.
        }
        
    }
}