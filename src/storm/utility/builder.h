#pragma once

#include <memory>

#include "storm/models/ModelType.h"
#include "storm/models/sparse/Model.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/storage/sparse/ModelComponents.h"

namespace storm {
namespace utility {
namespace builder {

template<typename ValueType, typename RewardModelType = storm::models::sparse::StandardRewardModel<ValueType>>
std::shared_ptr<storm::models::sparse::Model<ValueType, RewardModelType>> buildModelFromComponents(
    storm::models::ModelType modelType, storm::storage::sparse::ModelComponents<ValueType, RewardModelType>&& components);

}
}  // namespace utility
}  // namespace storm
