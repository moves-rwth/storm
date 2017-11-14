#include "storm/api/counterexamples.h"

#include "storm/environment/Environment.h"

namespace storm {
    namespace api {
        
        std::shared_ptr<storm::counterexamples::Counterexample> computePrismHighLevelCounterexampleMilp(storm::prism::Program const& program, std::shared_ptr<storm::models::sparse::Mdp<double>> mdp, std::shared_ptr<storm::logic::Formula const> const& formula) {
            Environment env;
            return storm::counterexamples::MILPMinimalLabelSetGenerator<double>::computeCounterexample(env, program, *mdp, formula);
        }
        
        std::shared_ptr<storm::counterexamples::Counterexample> computePrismHighLevelCounterexampleMaxSmt(storm::prism::Program const& program, std::shared_ptr<storm::models::sparse::Mdp<double>> mdp, std::shared_ptr<storm::logic::Formula const> const& formula) {
            Environment env;
            return storm::counterexamples::SMTMinimalLabelSetGenerator<double>::computeCounterexample(env, program, *mdp, formula);
        }
        
    }
}
