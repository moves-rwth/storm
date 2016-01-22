#include "src/modelchecker/abstraction/GameBasedMdpModelChecker.h"

#include "src/utility/macros.h"

#include "src/exceptions/NotSupportedException.h"

#include "src/modelchecker/results/CheckResult.h"

namespace storm {
    namespace modelchecker {
        template<storm::dd::DdType Type, typename ValueType>
        GameBasedMdpModelChecker<Type, ValueType>::GameBasedMdpModelChecker(storm::prism::Program const& program, std::unique_ptr<storm::utility::solver::SmtSolverFactory>&& smtSolverFactory) : originalProgram(program), smtSolverFactory(std::move(smtSolverFactory)) {
            STORM_LOG_THROW(program.getModelType() == storm::prism::Program::ModelType::MDP, storm::exceptions::NotSupportedException, "Currently only MDPs are supported by the game-based model checker.");
                        
            // Start by preparing the program. That is, we flatten the modules if there is more than one.
            if (originalProgram.getNumberOfModules() > 1) {
                preprocessedProgram = originalProgram.flattenModules(this->smtSolverFactory);
            } else {
                preprocessedProgram = originalProgram;
            }
        }

        template<storm::dd::DdType Type, typename ValueType>
        GameBasedMdpModelChecker<Type, ValueType>::~GameBasedMdpModelChecker() {
            // Intentionally left empty.
        }

        template<storm::dd::DdType Type, typename ValueType>
        bool GameBasedMdpModelChecker<Type, ValueType>::canHandle(storm::logic::Formula const& formula) const {
            if (formula.isProbabilityOperatorFormula()) {
                storm::logic::ProbabilityOperatorFormula const& probabilityOperatorFormula = formula.asProbabilityOperatorFormula();
                return this->canHandle(probabilityOperatorFormula.getSubformula());
            } else if (formula.isUntilFormula() || formula.isEventuallyFormula()) {
                if (formula.isUntilFormula()) {
                    storm::logic::UntilFormula const& untilFormula = formula.asUntilFormula();
                    if (untilFormula.getLeftSubformula().isPropositionalFormula() && untilFormula.getRightSubformula().isPropositionalFormula()) {
                        return true;
                    }
                } else if (formula.isEventuallyFormula()) {
                    storm::logic::EventuallyFormula const& eventuallyFormula = formula.asEventuallyFormula();
                    if (eventuallyFormula.getSubformula().isPropositionalFormula()) {
                        return true;
                    }
                }
            }
            return false;
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        std::unique_ptr<CheckResult> GameBasedMdpModelChecker<Type, ValueType>::checkProbabilityOperatorFormula(storm::logic::ProbabilityOperatorFormula const& stateFormula) {
            // Depending on whether or not there is a bound, we do something slightly different here.
            return nullptr;
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        std::unique_ptr<CheckResult> GameBasedMdpModelChecker<Type, ValueType>::computeUntilProbabilities(storm::logic::UntilFormula const& pathFormula, bool qualitative, boost::optional<OptimizationDirection> const& optimalityType) {
            // TODO
            return nullptr;
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        std::unique_ptr<CheckResult> GameBasedMdpModelChecker<Type, ValueType>::computeEventuallyProbabilities(storm::logic::EventuallyFormula const& pathFormula, bool qualitative, boost::optional<OptimizationDirection> const& optimalityType) {
            // TODO
            return nullptr;
        }
        
        template class GameBasedMdpModelChecker<storm::dd::DdType::CUDD, double>;
    }
}