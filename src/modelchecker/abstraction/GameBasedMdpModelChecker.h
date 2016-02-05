#ifndef STORM_MODELCHECKER_GAMEBASEDMDPMODELCHECKER_H_
#define STORM_MODELCHECKER_GAMEBASEDMDPMODELCHECKER_H_

#include "src/modelchecker/AbstractModelChecker.h"

#include "src/utility/solver.h"

#include "src/storage/prism/Program.h"

#include "src/storage/dd/DdType.h"

namespace storm {
    namespace modelchecker {
        template<storm::dd::DdType Type, typename ValueType>
        class GameBasedMdpModelChecker : public AbstractModelChecker {
        public:
            /*!
             * Constructs a model checker whose underlying model is implicitly given by the provided program. All
             * verification calls will be answererd with respect to this model.
             *
             * @param program The program that implicitly specifies the model to check.
             * @param smtSolverFactory A factory used to create SMT solver when necessary.
             */
            explicit GameBasedMdpModelChecker(storm::prism::Program const& program, std::unique_ptr<storm::utility::solver::SmtSolverFactory>&& smtSolverFactory = std::make_unique<storm::utility::solver::MathsatSmtSolverFactory>());
            
            virtual ~GameBasedMdpModelChecker() override;
                        
            virtual bool canHandle(CheckTask<storm::logic::Formula> const& checkTask) const override;
            
            virtual std::unique_ptr<CheckResult> checkProbabilityOperatorFormula(CheckTask<storm::logic::ProbabilityOperatorFormula> const& stateFormula) override;
            
            virtual std::unique_ptr<CheckResult> computeUntilProbabilities(CheckTask<storm::logic::UntilFormula> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeEventuallyProbabilities(CheckTask<storm::logic::EventuallyFormula> const& checkTask) override;
            
        private:
            void performGameBasedAbstractionRefinement(CheckTask<storm::logic::Formula> const& checkTask, storm::expressions::Expression const& targetStateExpression);
            
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

#endif /* STORM_MODELCHECKER_GAMEBASEDMDPMODELCHECKER_H_ */