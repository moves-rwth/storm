#include "storm/api/counterexamples.h"

namespace storm {
    namespace api {
        
        std::shared_ptr<storm::counterexamples::Counterexample> computePrismHighLevelCounterexampleMilp(storm::prism::Program const& program, std::shared_ptr<storm::models::sparse::Mdp<double>> mdp, std::shared_ptr<storm::logic::Formula const> const& formula) {
            return storm::counterexamples::MILPMinimalLabelSetGenerator<double>::computeCounterexample(program, *mdp, formula);
        }
        
        std::shared_ptr<storm::counterexamples::Counterexample> computePrismHighLevelCounterexampleMaxSmt(storm::prism::Program const& program, std::shared_ptr<storm::models::sparse::Mdp<double>> mdp, std::shared_ptr<storm::logic::Formula const> const& formula) {
            return storm::counterexamples::SMTMinimalLabelSetGenerator<double>::computeCounterexample(program, *mdp, formula);
        }
        
    }
}
