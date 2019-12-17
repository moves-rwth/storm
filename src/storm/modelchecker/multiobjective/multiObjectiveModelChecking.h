#pragma once

#include <memory>

#include "storm/modelchecker/results/CheckResult.h"
#include "storm/modelchecker/multiobjective/MultiObjectiveModelCheckingMethod.h"
#include "storm/logic/Formulas.h"

namespace storm {

    class Environment;

    namespace modelchecker {
        namespace multiobjective {
            
            
            template<typename SparseModelType>
            std::unique_ptr<CheckResult> performMultiObjectiveModelChecking(Environment const& env, SparseModelType const& model, storm::logic::MultiObjectiveFormula const& formula);
            
        }
    }
}
