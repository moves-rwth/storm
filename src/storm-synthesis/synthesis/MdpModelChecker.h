#pragma once

#include "storm/environment/Environment.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/modelchecker/CheckTask.h"
#include "storm/modelchecker/results/CheckResult.h"

namespace storm {
    namespace synthesis {

        template<typename ValueType>
        std::shared_ptr<storm::modelchecker::CheckResult> verifyMdp(
            storm::Environment const& env,
            std::shared_ptr<storm::models::sparse::Mdp<ValueType>> const& mdp,
            storm::logic::Formula const& formula,
            bool produce_schedulers
        );
    
    } 
}
