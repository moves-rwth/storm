#include "src/models/ModelBase.h"

namespace storm {
    namespace models {
        ModelType ModelBase::getType() const {
            return modelType;
        }
    }
}