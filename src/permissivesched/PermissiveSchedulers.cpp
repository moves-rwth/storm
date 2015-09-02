#include "PermissiveSchedulers.h"
#include "../storage/BitVector.h"
#include "../utility/solver.h"
#include "../utility/graph.h"
#include "../modelchecker/propositional/SparsePropositionalModelChecker.h"
#include "../modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "MILPPermissiveSchedulers.h"
#include "src/exceptions/NotImplementedException.h"
#include "src/utility/macros.h"

namespace storm {
    namespace ps {
        
         boost::optional<MemorylessDeterministicPermissiveScheduler> computePermissiveSchedulerViaMILP(std::shared_ptr<storm::models::sparse::Mdp<double>> mdp, storm::logic::ProbabilityOperatorFormula const& safeProp) {
            storm::modelchecker::SparsePropositionalModelChecker<storm::models::sparse::Mdp<double>> propMC(*mdp);
            assert(safeProp.getSubformula().isEventuallyFormula());
            auto backwardTransitions = mdp->getBackwardTransitions();
            storm::storage::BitVector goalstates = propMC.check(safeProp.getSubformula().asEventuallyFormula().getSubformula())->asExplicitQualitativeCheckResult().getTruthValuesVector();
            goalstates = storm::utility::graph::performProb1E(*mdp, backwardTransitions, storm::storage::BitVector(goalstates.size(), true), goalstates);
            storm::storage::BitVector sinkstates = storm::utility::graph::performProb0A(*mdp,backwardTransitions, storm::storage::BitVector(goalstates.size(), true), goalstates);

            auto solver = storm::utility::solver::getLpSolver("Gurobi", storm::solver::LpSolverTypeSelection::Gurobi);
            MilpPermissiveSchedulerComputation comp(*solver, mdp, goalstates, sinkstates);
            STORM_LOG_THROW(!storm::logic::isStrict(safeProp.getComparisonType()), storm::exceptions::NotImplementedException, "Strict bounds are not supported");
            comp.calculatePermissiveScheduler(storm::logic::isLowerBound(safeProp.getComparisonType()), safeProp.getBound());
            comp.dumpLpToFile("milpdump.lp");
            std::cout << "Found Solution: " << (comp.foundSolution() ? "yes" : "no") << std::endl;
            if(comp.foundSolution()) {
                return boost::optional<MemorylessDeterministicPermissiveScheduler>(comp.getScheduler());
            } else {
                return boost::optional<MemorylessDeterministicPermissiveScheduler>();
            }
        }
         
         boost::optional<MemorylessDeterministicPermissiveScheduler> computePermissiveSchedulerViaMC(std::shared_ptr<storm::models::sparse::Mdp<double>> mdp, storm::logic::ProbabilityOperatorFormula const& safeProp) {
             
         }
         
         boost::optional<MemorylessDeterministicPermissiveScheduler> computerPermissiveSchedulerViaSMT(std::shared_ptr<storm::models::sparse::Mdp<double>> mdp, storm::logic::ProbabilityOperatorFormula const& safeProp) {
             
         }
    }
}
