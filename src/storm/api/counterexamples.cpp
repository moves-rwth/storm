#include "storm/api/counterexamples.h"

#include "storm/environment/Environment.h"

namespace storm {
    namespace api {
        
        std::shared_ptr<storm::counterexamples::Counterexample> computeHighLevelCounterexampleMilp(storm::storage::SymbolicModelDescription const& symbolicModel, std::shared_ptr<storm::models::sparse::Mdp<double>> mdp, std::shared_ptr<storm::logic::Formula const> const& formula) {
            Environment env;
            return storm::counterexamples::MILPMinimalLabelSetGenerator<double>::computeCounterexample(env, symbolicModel, *mdp, formula);
        }
        
        std::shared_ptr<storm::counterexamples::Counterexample> computeHighLevelCounterexampleMaxSmt(storm::prism::Program const& program, std::shared_ptr<storm::models::sparse::Model<double>> model, std::shared_ptr<storm::logic::Formula const> const& formula) {
            Environment env;
            return storm::counterexamples::SMTMinimalLabelSetGenerator<double>::computeCounterexample(env, program, *model, formula);
        }
        
    }
}
