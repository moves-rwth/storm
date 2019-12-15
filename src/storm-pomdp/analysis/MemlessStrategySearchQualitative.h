#include <vector>
#include "storm/storage/expressions/Expressions.h"
#include "storm/solver/SmtSolver.h"
#include "storm/models/sparse/Pomdp.h"
#include "storm/utility/solver.h"
#include "storm/exceptions/UnexpectedException.h"

namespace storm {
namespace pomdp {

    template<typename ValueType>
    class MemlessStrategySearchQualitative {
    // Implements an extension to the Chatterjee, Chmelik, Davies (AAAI-16) paper.


    public:
        MemlessStrategySearchQualitative(storm::models::sparse::Pomdp<ValueType> const& pomdp,
                                         std::set<uint32_t> const& targetObservationSet,
                                         storm::storage::BitVector const& targetStates,
                                         storm::storage::BitVector const& surelyReachSinkStates,
                                         std::shared_ptr<storm::utility::solver::SmtSolverFactory>& smtSolverFactory) :
                pomdp(pomdp),
                targetStates(targetStates),
                surelyReachSinkStates(surelyReachSinkStates),
                targetObservations(targetObservationSet) {
            this->expressionManager = std::make_shared<storm::expressions::ExpressionManager>();
            smtSolver = smtSolverFactory->create(*expressionManager);

        }

        void setSurelyReachSinkStates(storm::storage::BitVector const& surelyReachSink) {
            surelyReachSinkStates = surelyReachSink;
        }

        void analyzeForInitialStates(uint64_t k) {
            analyze(k, pomdp.getInitialStates(), pomdp.getInitialStates());
        }

        void findNewStrategyForSomeState(uint64_t k) {
            std::cout << surelyReachSinkStates << std::endl;
            std::cout << targetStates << std::endl;
            std::cout << (~surelyReachSinkStates & ~targetStates) << std::endl;
            analyze(k, ~surelyReachSinkStates & ~targetStates);


        }

        bool analyze(uint64_t k, storm::storage::BitVector const& oneOfTheseStates, storm::storage::BitVector const& allOfTheseStates = storm::storage::BitVector());


    private:
        void initialize(uint64_t k);


        std::unique_ptr<storm::solver::SmtSolver> smtSolver;
        storm::models::sparse::Pomdp<ValueType> const& pomdp;
        std::shared_ptr<storm::expressions::ExpressionManager> expressionManager;
        uint64_t maxK = std::numeric_limits<uint64_t>::max();

        std::set<uint32_t> targetObservations;
        storm::storage::BitVector targetStates;
        storm::storage::BitVector surelyReachSinkStates;

        std::vector<std::vector<uint64_t>> statesPerObservation;
        std::vector<std::vector<storm::expressions::Expression>> actionSelectionVarExpressions; // A_{z,a}
        std::vector<std::vector<storm::expressions::Variable>> actionSelectionVars;
        std::vector<storm::expressions::Variable> reachVars;
        std::vector<storm::expressions::Expression> reachVarExpressions;
        std::vector<std::vector<storm::expressions::Expression>> pathVars;



    };
}
}
