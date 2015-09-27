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
        
         boost::optional<SubMDPPermissiveScheduler> computePermissiveSchedulerViaMILP(std::shared_ptr<storm::models::sparse::Mdp<double>> mdp, storm::logic::ProbabilityOperatorFormula const& safeProp) {
            storm::modelchecker::SparsePropositionalModelChecker<storm::models::sparse::Mdp<double>> propMC(*mdp);
            assert(safeProp.getSubformula().isEventuallyFormula());
            auto backwardTransitions = mdp->getBackwardTransitions();
            storm::storage::BitVector goalstates = propMC.check(safeProp.getSubformula().asEventuallyFormula().getSubformula())->asExplicitQualitativeCheckResult().getTruthValuesVector();
            goalstates = storm::utility::graph::performProb1A(*mdp, backwardTransitions, storm::storage::BitVector(goalstates.size(), true), goalstates);
            storm::storage::BitVector sinkstates = storm::utility::graph::performProb0A(*mdp,backwardTransitions, storm::storage::BitVector(goalstates.size(), true), goalstates);

            auto solver = storm::utility::solver::getLpSolver("Gurobi", storm::solver::LpSolverTypeSelection::Gurobi);
            MilpPermissiveSchedulerComputation comp(*solver, mdp, goalstates, sinkstates);
            STORM_LOG_THROW(!storm::logic::isStrict(safeProp.getComparisonType()), storm::exceptions::NotImplementedException, "Strict bounds are not supported");
            comp.calculatePermissiveScheduler(storm::logic::isLowerBound(safeProp.getComparisonType()), safeProp.getBound());
            //comp.dumpLpToFile("milpdump.lp");
            std::cout << "Found Solution: " << (comp.foundSolution() ? "yes" : "no") << std::endl;
            if(comp.foundSolution()) {
                return boost::optional<SubMDPPermissiveScheduler>(comp.getScheduler());
            } else {
                return boost::optional<SubMDPPermissiveScheduler>();
            }
        }
         
         boost::optional<SubMDPPermissiveScheduler> computePermissiveSchedulerViaMC(std::shared_ptr<storm::models::sparse::Mdp<double>> mdp, storm::logic::ProbabilityOperatorFormula const& safeProp) {
             
         }
         
         boost::optional<SubMDPPermissiveScheduler> computerPermissiveSchedulerViaSMT(std::shared_ptr<storm::models::sparse::Mdp<double>> mdp, storm::logic::ProbabilityOperatorFormula const& safeProp) {
             storm::modelchecker::SparsePropositionalModelChecker<storm::models::sparse::Mdp<double>> propMC(*mdp);
             assert(safeProp.getSubformula().isEventuallyFormula());
             auto backwardTransitions = mdp->getBackwardTransitions();
             storm::storage::BitVector goalstates = propMC.check(safeProp.getSubformula().asEventuallyFormula().getSubformula())->asExplicitQualitativeCheckResult().getTruthValuesVector();
             goalstates = storm::utility::graph::performProb1A(*mdp, backwardTransitions, storm::storage::BitVector(goalstates.size(), true), goalstates);
             storm::storage::BitVector sinkstates = storm::utility::graph::performProb0A(*mdp,backwardTransitions, storm::storage::BitVector(goalstates.size(), true), goalstates);

            SmtPermissiveSchedulerComputation comp(mdp, goalstates, sinkstates)

            if(comp.foundSolution()) {
                return boost::optional<SubMDPPermissiveScheduler>(comp.getScheduler());
            } else {
                return boost::optional<SubMDPPermissiveScheduler>();
            }
         }
    }
}
