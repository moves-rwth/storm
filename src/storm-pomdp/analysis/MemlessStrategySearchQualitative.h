#include <vector>
#include "storm/storage/expressions/Expressions.h"
#include "storm/solver/SmtSolver.h"
#include "storm/models/sparse/Pomdp.h"
#include "storm/utility/solver.h"

namespace storm {
namespace pomdp {

    template<typename ValueType>
    class MemlessStrategySearchQualitative {
    // Implements an extension to the Chatterjee, Chmelik, Davies (AAAI-16) paper.


    public:
        MemlessStrategySearchQualitative(storm::models::sparse::Pomdp<ValueType> const& pomdp,
                                         std::set<uint32_t> const& targetObservationSet,
                                         std::shared_ptr<storm::utility::solver::SmtSolverFactory>& smtSolverFactory) :
                pomdp(pomdp),
                targetObservations(targetObservationSet) {
            this->expressionManager = std::make_shared<storm::expressions::ExpressionManager>();
            smtSolver = smtSolverFactory->create(*expressionManager);

        }

        void setSurelyReachSinkStates(storm::storage::BitVector const& surelyReachSink) {
            surelyReachSinkStates = surelyReachSink;
        }

        void analyze(uint64_t k) {
            if (k < maxK) {
                initialize(k);
            }
            std::cout << smtSolver->getSmtLibString() << std::endl;
            for (uint64_t state : pomdp.getInitialStates()) {
                smtSolver->add(reachVars[state]);
            }
            auto result = smtSolver->check();
            switch(result) {
                case storm::solver::SmtSolver::CheckResult::Sat:
                    std::cout << std::endl << "Satisfying assignment: " << std::endl << smtSolver->getModelAsValuation().toString(true) << std::endl;

                case storm::solver::SmtSolver::CheckResult::Unsat:
                    // std::cout << std::endl << "Unsatisfiability core: {" << std::endl;
                    // for (auto const& expr : solver->getUnsatCore()) {
                    //    std::cout << "\t " << expr << std::endl;
                    // }
                    // std::cout << "}" << std::endl;

                default:
                    std::cout<< "oops." << std::endl;
                   // STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "SMT solver yielded an unexpected result");
            }
            //std::cout << "get model:" << std::endl;
            //std::cout << smtSolver->getModel().toString() << std::endl;
        }


    private:
        void initialize(uint64_t k);

        std::unique_ptr<storm::solver::SmtSolver> smtSolver;
        storm::models::sparse::Pomdp<ValueType> const& pomdp;
        std::shared_ptr<storm::expressions::ExpressionManager> expressionManager;
        uint64_t maxK = -1;

        std::set<uint32_t> targetObservations;
        storm::storage::BitVector surelyReachSinkStates;

        std::vector<std::vector<uint64_t>> statesPerObservation;
        std::vector<std::vector<storm::expressions::Expression>> actionSelectionVars; // A_{z,a}
        std::vector<storm::expressions::Expression> reachVars;
        std::vector<std::vector<storm::expressions::Expression>> pathVars;



    };
}
}
