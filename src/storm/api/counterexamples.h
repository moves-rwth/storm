#pragma once

#include "storm/counterexamples/MILPMinimalLabelSetGenerator.h"
#include "storm/counterexamples/SMTMinimalLabelSetGenerator.h"

namespace storm {
    namespace api {
        
        std::shared_ptr<storm::counterexamples::Counterexample> computeHighLevelCounterexampleMilp(storm::storage::SymbolicModelDescription const& symbolicModel, std::shared_ptr<storm::models::sparse::Mdp<double>> mdp, std::shared_ptr<storm::logic::Formula const> const& formula);
        
        std::shared_ptr<storm::counterexamples::Counterexample> computeHighLevelCounterexampleMaxSmt(storm::prism::Program const& program, std::shared_ptr<storm::models::sparse::Model<double>> model, std::shared_ptr<storm::logic::Formula const> const& formula);
        
    }
}
