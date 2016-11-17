#pragma once
#include "../models/ModelBase.h"
#include <vector>

namespace storm {
    namespace logic {
        class Formula;
    }

    namespace storage {
        struct ModelFormulasPair {
            std::shared_ptr<storm::models::ModelBase> model;
            std::vector<std::shared_ptr<storm::logic::Formula const>> formulas;
        };
    }
}
