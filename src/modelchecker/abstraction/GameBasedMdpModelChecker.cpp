#include "src/modelchecker/abstraction/GameBasedMdpModelChecker.h"

#include "src/storage/expressions/ExpressionManager.h"

#include "src/utility/macros.h"

#include "src/exceptions/NotSupportedException.h"
#include "src/exceptions/InvalidPropertyException.h"

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
        bool GameBasedMdpModelChecker<Type, ValueType>::canHandle(CheckTask<storm::logic::Formula> const& checkTask) const {
            storm::logic::Formula const& formula = checkTask.getFormula();
            if (formula.isProbabilityOperatorFormula()) {
                return this->canHandle(checkTask.replaceFormula(formula.asProbabilityOperatorFormula().getSubformula()));
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
        std::unique_ptr<CheckResult> GameBasedMdpModelChecker<Type, ValueType>::computeUntilProbabilities(CheckTask<storm::logic::UntilFormula> const& checkTask) {
            storm::logic::UntilFormula const& pathFormula = checkTask.getFormula();
            return performGameBasedAbstractionRefinement(CheckTask<storm::logic::Formula>(pathFormula), getExpression(pathFormula.getLeftSubformula()), getExpression(pathFormula.getRightSubformula()));
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        std::unique_ptr<CheckResult> GameBasedMdpModelChecker<Type, ValueType>::computeEventuallyProbabilities(CheckTask<storm::logic::EventuallyFormula> const& checkTask) {
            storm::logic::EventuallyFormula const& pathFormula = checkTask.getFormula();
            return performGameBasedAbstractionRefinement(CheckTask<storm::logic::Formula>(pathFormula), originalProgram.getManager().boolean(true), getExpression(pathFormula.getSubformula()));
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        std::unique_ptr<CheckResult> GameBasedMdpModelChecker<Type, ValueType>::performGameBasedAbstractionRefinement(CheckTask<storm::logic::Formula> const& checkTask, storm::expressions::Expression const& constraintExpression, storm::expressions::Expression const& targetStateExpression) {
            return nullptr;
        }
        
        template<storm::dd::DdType Type, typename ValueType>
        storm::expressions::Expression GameBasedMdpModelChecker<Type, ValueType>::getExpression(storm::logic::Formula const& formula) {
            STORM_LOG_THROW(formula.isAtomicExpressionFormula() || formula.isAtomicLabelFormula(), storm::exceptions::InvalidPropertyException, "The target states have to be given as label or an expression.");
            storm::expressions::Expression result;
            if (formula.isAtomicLabelFormula()) {
                result = preprocessedProgram.getLabelExpression(formula.asAtomicLabelFormula().getLabel());
            } else {
                result = formula.asAtomicExpressionFormula().getExpression();
            }
            return result;
        }
        
        template class GameBasedMdpModelChecker<storm::dd::DdType::CUDD, double>;
    }
}