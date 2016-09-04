#ifndef STORM_MODELCHECKER_GAMEBASEDMDPMODELCHECKER_H_
#define STORM_MODELCHECKER_GAMEBASEDMDPMODELCHECKER_H_

#include "src/modelchecker/AbstractModelChecker.h"

#include "src/storage/prism/Program.h"

#include "src/storage/dd/DdType.h"

#include "src/utility/solver.h"
#include "src/utility/graph.h"

namespace storm {
    namespace abstraction {
        template<storm::dd::DdType Type, typename ValueType>
        class MenuGame;
    }
    
    namespace modelchecker {
        namespace detail {
            template<storm::dd::DdType Type>
            struct GameProb01Result {
            public:
                GameProb01Result() = default;
                GameProb01Result(storm::utility::graph::GameProb01Result<Type> const& prob0Min, storm::utility::graph::GameProb01Result<Type> const& prob1Min, storm::utility::graph::GameProb01Result<Type> const& prob0Max, storm::utility::graph::GameProb01Result<Type> const& prob1Max);
                
                std::pair<storm::utility::graph::GameProb01Result<Type>, storm::utility::graph::GameProb01Result<Type>> min;
                std::pair<storm::utility::graph::GameProb01Result<Type>, storm::utility::graph::GameProb01Result<Type>> max;
            };
        }

        template<storm::dd::DdType Type, typename ModelType>
        class GameBasedMdpModelChecker : public AbstractModelChecker<ModelType> {
        public:
            typedef typename ModelType::ValueType ValueType;
            
            /*!
             * Constructs a model checker whose underlying model is implicitly given by the provided program. All
             * verification calls will be answererd with respect to this model.
             *
             * @param program The program that implicitly specifies the model to check.
             * @param smtSolverFactory A factory used to create SMT solver when necessary.
             */
            explicit GameBasedMdpModelChecker(storm::prism::Program const& program, std::shared_ptr<storm::utility::solver::SmtSolverFactory> const& smtSolverFactory = std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>());
            
            virtual ~GameBasedMdpModelChecker() override;
                        
            virtual bool canHandle(CheckTask<storm::logic::Formula> const& checkTask) const override;
            
            virtual std::unique_ptr<CheckResult> computeUntilProbabilities(CheckTask<storm::logic::UntilFormula> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeReachabilityProbabilities(CheckTask<storm::logic::EventuallyFormula> const& checkTask) override;
            
        private:
            std::unique_ptr<CheckResult> performGameBasedAbstractionRefinement(CheckTask<storm::logic::Formula> const& checkTask, storm::expressions::Expression const& constraintExpression, storm::expressions::Expression const& targetStateExpression);
            
            std::pair<storm::utility::graph::GameProb01Result<Type>, storm::utility::graph::GameProb01Result<Type>> computeProb01States(storm::OptimizationDirection player1Direction, storm::OptimizationDirection player2Direction, storm::abstraction::MenuGame<Type, ValueType> const& game, storm::dd::Bdd<Type> const& transitionMatrixBdd, storm::dd::Bdd<Type> const& constraintStates, storm::dd::Bdd<Type> const& targetStates);
            
            storm::expressions::Expression getExpression(storm::logic::Formula const& formula);
            
            // The original program that was used to create this model checker.
            storm::prism::Program originalProgram;
            
            // The preprocessed program that contains only one module and otherwhise corresponds to the semantics of the
            // original program.
            storm::prism::Program preprocessedProgram;
            
            // A factory that is used for creating SMT solvers when needed.
            std::shared_ptr<storm::utility::solver::SmtSolverFactory> smtSolverFactory;
        };
    }
}

#endif /* STORM_MODELCHECKER_GAMEBASEDMDPMODELCHECKER_H_ */