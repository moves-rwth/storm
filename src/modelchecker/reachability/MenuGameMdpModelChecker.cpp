#include "src/modelchecker/reachability/MenuGameMdpModelChecker.h"

#include "src/exceptions/NotSupportedException.h"

namespace storm {
    namespace modelchecker {
        MenuGameMdpModelChecker::MenuGameMdpModelChecker(storm::prism::Program const& program, std::unique_ptr<storm::utility::solver::SmtSolverFactory>&& smtSolverFactory) : originalProgram(program), smtSolverFactory(std::move(smtSolverFactory)) {
            STORM_LOG_THROW(program.getModelType() == storm::prism::Program::ModelType::MDP, storm::exceptions::NotSupportedException, "Currently only MDPs are supported by the game-based model checker.");
            
            // Start by preparing the program. That is, we flatten the modules if there is more than one.
            if (originalProgram.getNumberOfModules() > 1) {
                preprocessedProgram = originalProgram.flattenModules(smtSolverFactory);
            } else {
                preprocessedProgram = originalProgram;
            }
        }
        
        bool MenuGameMdpModelChecker::canHandle(storm::logic::Formula const& formula) const {
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
        
        std::unique_ptr<CheckResult> MenuGameMdpModelChecker::checkProbabilityOperatorFormula(storm::logic::ProbabilityOperatorFormula const& stateFormula) {
            // Depending on whether or not there is a bound, we do something slightly different here.
            
            return nullptr;
        }
        
        std::unique_ptr<CheckResult> computeUntilProbabilities(storm::logic::UntilFormula const& pathFormula, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
            // TODO
            
            return nullptr;
        }

        std::unique_ptr<CheckResult> computeEventuallyProbabilities(storm::logic::EventuallyFormula const& pathFormula, bool qualitative, boost::optional<storm::logic::OptimalityType> const& optimalityType) {
            // TODO

            return nullptr;
        }

    }
}