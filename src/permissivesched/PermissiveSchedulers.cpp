#include "PermissiveSchedulers.h"
#include "../storage/BitVector.h"
#include "../utility/solver.h"
#include "../utility/graph.h"
#include "../modelchecker/propositional/SparsePropositionalModelChecker.h"
#include "../modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "MILPPermissiveSchedulers.h"


namespace storm {
    namespace ps {
        
         boost::optional<MemorylessDeterministicPermissiveScheduler> computePermissiveSchedulerViaMILP(std::shared_ptr<storm::models::sparse::Mdp<double>> mdp, storm::logic::ProbabilityOperatorFormula const& safeProp) {
            storm::modelchecker::SparsePropositionalModelChecker<double> propMC(*mdp);
            assert(safeProp.getSubformula().isEventuallyFormula());
            auto backwardTransitions = mdp->getBackwardTransitions();
            storm::storage::BitVector goalstates = propMC.check(safeProp.getSubformula().asEventuallyFormula().getSubformula())->asExplicitQualitativeCheckResult().getTruthValuesVector();
            goalstates = storm::utility::graph::performProb1E(*mdp, backwardTransitions, storm::storage::BitVector(goalstates.size(), true), goalstates);
            storm::storage::BitVector sinkstates = storm::utility::graph::performProb0A(*mdp,backwardTransitions, storm::storage::BitVector(goalstates.size(), true), goalstates);

            auto solver = storm::utility::solver::getLpSolver("Gurobi");
            MilpPermissiveSchedulerComputation comp(*solver, mdp, goalstates, sinkstates);
            comp.calculatePermissiveScheduler(safeProp.getBound());
            if(comp.foundSolution()) {
                return boost::optional<MemorylessDeterministicPermissiveScheduler>(comp.getScheduler());
            } else {
                return boost::optional<MemorylessDeterministicPermissiveScheduler>(comp.getScheduler());
            }
        }
    }
}
