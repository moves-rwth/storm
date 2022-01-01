#pragma once
#include <vector>
#include "../models/ModelBase.h"

namespace storm {
namespace logic {
class Formula;
}

namespace storage {
struct ModelFormulasPair {
    std::shared_ptr<storm::models::ModelBase> model;
    std::vector<std::shared_ptr<storm::logic::Formula const>> formulas;
};
}  // namespace storage
}  // namespace storm
