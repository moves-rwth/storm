#ifndef STORM_MODELCHECKER_MENUGAMEMDPMODELCHECKER_H_
#define STORM_MODELCHECKER_MENUGAMEMDPMODELCHECKER_H_

#include "src/modelchecker/AbstractModelChecker.h"

#include "src/storage/prism/Program.h"

#include "src/utility/solver.h"

namespace storm {
    namespace modelchecker {
        class MenuGameMdpModelChecker : public AbstractModelChecker {
        public:
            /*!
             * Constructs a model checker whose underlying model is implicitly given by the provided program. All
             * verification calls will be answererd with respect to this model.
             *
             * @param program The program that implicitly specifies the model to check.
             * @param smtSolverFactory A factory used to create SMT solver when necessary.
             */
            explicit MenuGameMdpModelChecker(storm::prism::Program const& program, std::unique_ptr<storm::utility::solver::SmtSolverFactory>&& smtSolverFactory);
            
            virtual bool canHandle(storm::logic::Formula const& formula) const override;
            
            virtual std::unique_ptr<CheckResult> checkProbabilityOperatorFormula(storm::logic::ProbabilityOperatorFormula const& stateFormula) override;
            
            virtual std::unique_ptr<CheckResult> computeUntilProbabilities(storm::logic::UntilFormula const& pathFormula, bool qualitative = false, boost::optional<storm::logic::OptimalityType> const& optimalityType = boost::optional<storm::logic::OptimalityType>()) override;
            virtual std::unique_ptr<CheckResult> computeEventuallyProbabilities(storm::logic::EventuallyFormula const& pathFormula, bool qualitative = false, boost::optional<storm::logic::OptimalityType> const& optimalityType = boost::optional<storm::logic::OptimalityType>()) override;

        private:
            /*!
             * Performs game-based abstraction refinement on the model until either the precision is met or the provided
             * proof goal was successfully proven.
             *
             * @param filterPredicate A predicate that needs to hold until the target predicate holds.
             * @param targetPredicate A predicate characterizing the target states.
             * @param precision The precision to use. This governs what difference between lower and upper bounds is
             * acceptable.
             * @param proofGoal A proof goal that says the probability must only be established to be above/below a given
             * threshold. If the proof goal is met before the precision is achieved, the refinement procedure will abort
             * and return the current result.
             * @return A pair of values, that are under- and over-approximations of the actual probability, respectively.
             */
            std::pair<double, double> performGameBasedRefinement(storm::expressions::Expression const& filterPredicate, storm::expressions::Expression const& targetPredicate, double precision, boost::optional<std::pair<double, storm::logic::ComparisonType>> const& proofGoal);
            
            // The original program that was used to create this model checker.
            storm::prism::Program originalProgram;
            
            // The preprocessed program that contains only one module and otherwhise corresponds to the semantics of the
            // original program.
            storm::prism::Program preprocessedProgram;
            
            // A factory that is used for creating SMT solvers when needed.
            std::unique_ptr<storm::utility::solver::SmtSolverFactory> smtSolverFactory;
        };
    }
}

#endif /* STORM_MODELCHECKER_MENUGAMEMDPMODELCHECKER_H_ */