#include <vector>
#include "storm/storage/expressions/Expressions.h"
#include "storm/solver/SmtSolver.h"
#include "storm/models/sparse/Pomdp.h"
#include "storm/utility/solver.h"
#include "storm/exceptions/UnexpectedException.h"

#include "storm-pomdp/analysis/WinningRegion.h"

namespace storm {
namespace pomdp {

    class MemlessSearchOptions {

    public:
        void setExportSATCalls(std::string const& path) {
            exportSATcalls = path;
        }

        std::string const& getExportSATCallsPath() const {
            return exportSATcalls;
        }

        bool isExportSATSet() const {
            return exportSATcalls == "";
        }
    private:
        std::string exportSATcalls = "";

    };

    struct InternalObservationScheduler {
        std::vector<std::set<uint64_t>> actions;
        std::vector<uint64_t> schedulerRef;
        storm::storage::BitVector switchObservations;

        void clear() {
            actions.clear();
            schedulerRef.clear();
            switchObservations.clear();
        }

        bool empty() const {
            return actions.empty();
        }

        void printForObservations(storm::storage::BitVector const& observations, storm::storage::BitVector const& observationsAfterSwitch) const {
            for (uint64_t obs = 0; obs < observations.size(); ++obs) {
                if (observations.get(obs)) {
                    std::cout << "observation: " << obs << std::endl;
                    std::cout << "actions:";
                    for (auto act : actions[obs]) {
                        std::cout << " " << act;
                    }
                    if (switchObservations.get(obs)) {
                        std::cout << " and switch.";
                    }
                    std::cout << std::endl;
                }
                if (observationsAfterSwitch.get(obs)) {
                    std::cout << "scheduler ref: " << schedulerRef[obs] << std::endl;
                }

            }
        }
    };

    template<typename ValueType>
    class MemlessStrategySearchQualitative {
    // Implements an extension to the Chatterjee, Chmelik, Davies (AAAI-16) paper.

    public:
        MemlessStrategySearchQualitative(storm::models::sparse::Pomdp<ValueType> const& pomdp,
                                         std::set<uint32_t> const& targetObservationSet,
                                         storm::storage::BitVector const& targetStates,
                                         storm::storage::BitVector const& surelyReachSinkStates,
                                         std::shared_ptr<storm::utility::solver::SmtSolverFactory>& smtSolverFactory);

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
        storm::expressions::Expression const& getDoneActionExpression(uint64_t obs) const;

        void printScheduler(std::vector<InternalObservationScheduler> const& );

        void initialize(uint64_t k);


        std::unique_ptr<storm::solver::SmtSolver> smtSolver;
        storm::models::sparse::Pomdp<ValueType> const& pomdp;
        std::shared_ptr<storm::expressions::ExpressionManager> expressionManager;
        uint64_t maxK = std::numeric_limits<uint64_t>::max();

        std::set<uint32_t> targetObservations;
        storm::storage::BitVector targetStates;
        storm::storage::BitVector surelyReachSinkStates;

        std::vector<storm::expressions::Variable> schedulerVariables;
        std::vector<storm::expressions::Expression> schedulerVariableExpressions;
        std::vector<std::vector<uint64_t>> statesPerObservation;
        std::vector<std::vector<storm::expressions::Expression>> actionSelectionVarExpressions; // A_{z,a}
        std::vector<std::vector<storm::expressions::Variable>> actionSelectionVars;

        std::vector<storm::expressions::Variable> reachVars;
        std::vector<storm::expressions::Expression> reachVarExpressions;

        std::vector<storm::expressions::Variable> switchVars;
        std::vector<storm::expressions::Expression> switchVarExpressions;
        std::vector<storm::expressions::Variable> continuationVars;
        std::vector<storm::expressions::Expression> continuationVarExpressions;
        std::vector<std::vector<storm::expressions::Expression>> pathVars;

        std::vector<InternalObservationScheduler> finalSchedulers;
        std::vector<std::vector<uint64_t>> schedulerForObs;
        WinningRegion winningRegion;



    };
}
}
