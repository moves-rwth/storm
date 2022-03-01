#pragma once

#include "storm/models/ModelBase.h"

namespace storm {
namespace models {

template<typename ValueType>
class Model : public ModelBase {
   public:
    /*!
     * Constructs a model of the given type.
     *
     * @param modelType The type of the model.
     */
    Model(ModelType const& modelType) : ModelBase(modelType) {
        // Intentionally left empty.
    }

    virtual ~Model() = default;
};

}  // namespace models
}  // namespace storm
