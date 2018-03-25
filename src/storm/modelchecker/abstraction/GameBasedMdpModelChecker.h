#ifndef STORM_MODELCHECKER_GAMEBASEDMDPMODELCHECKER_H_
#define STORM_MODELCHECKER_GAMEBASEDMDPMODELCHECKER_H_

#include "storm/modelchecker/AbstractModelChecker.h"

#include "storm/storage/prism/Program.h"

#include "storm/storage/dd/DdType.h"

#include "storm/storage/SymbolicModelDescription.h"

#include "storm/abstraction/SymbolicQualitativeGameResult.h"
#include "storm/abstraction/SymbolicQualitativeGameResultMinMax.h"

#include "storm/logic/Bound.h"

#include "storm/settings/modules/AbstractionSettings.h"

#include "storm/utility/ConstantsComparator.h"
#include "storm/utility/solver.h"
#include "storm/utility/graph.h"

namespace storm {
    namespace abstraction {
        template<storm::dd::DdType Type, typename ValueType>
        class MenuGame;

        template<storm::dd::DdType Type, typename ValueType>
        class MenuGameAbstractor;
        
        template<storm::dd::DdType Type, typename ValueType>
        class MenuGameRefiner;
        
        template<storm::dd::DdType Type>
        class SymbolicQualitativeGameResultMinMax;
        
        template<storm::dd::DdType Type, typename ValueType>
        class SymbolicQuantitativeGameResult;
        
        class ExplicitQualitativeGameResult;
        class ExplicitQualitativeGameResultMinMax;
    }
    
    namespace modelchecker {
        
        using storm::abstraction::SymbolicQualitativeGameResult;
        using storm::abstraction::SymbolicQualitativeGameResultMinMax;
        using storm::abstraction::ExplicitQualitativeGameResult;
        using storm::abstraction::ExplicitQualitativeGameResultMinMax;
        
        template<storm::dd::DdType Type, typename ModelType>
        class GameBasedMdpModelChecker : public AbstractModelChecker<ModelType> {
        public:
            typedef typename ModelType::ValueType ValueType;
            
            /*!
             * Constructs a model checker whose underlying model is implicitly given by the provided program. All
             * verification calls will be answererd with respect to this model.
             *
             * @param model The model description that (symbolically) specifies the model to check.
             * @param smtSolverFactory A factory used to create SMT solver when necessary.
             */
            explicit GameBasedMdpModelChecker(storm::storage::SymbolicModelDescription const& model, std::shared_ptr<storm::utility::solver::SmtSolverFactory> const& smtSolverFactory = std::make_shared<storm::utility::solver::MathsatSmtSolverFactory>());
            
            /// Overridden methods from super class.
            virtual bool canHandle(CheckTask<storm::logic::Formula> const& checkTask) const override;
            virtual std::unique_ptr<CheckResult> computeUntilProbabilities(Environment const& env, CheckTask<storm::logic::UntilFormula> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> computeReachabilityProbabilities(Environment const& env, CheckTask<storm::logic::EventuallyFormula> const& checkTask) override;
            
        private:
            /*!
             * Performs the core part of the abstraction-refinement loop.
             */
            std::unique_ptr<CheckResult> performGameBasedAbstractionRefinement(Environment const& env, CheckTask<storm::logic::Formula> const& checkTask, storm::expressions::Expression const& constraintExpression, storm::expressions::Expression const& targetStateExpression);
            
            std::unique_ptr<CheckResult> performSymbolicAbstractionSolutionStep(Environment const& env, CheckTask<storm::logic::Formula> const& checkTask, storm::abstraction::MenuGame<Type, ValueType> const& game, storm::OptimizationDirection player1Direction, storm::dd::Bdd<Type> const& initialStates, storm::dd::Bdd<Type> const& constraintStates, storm::dd::Bdd<Type> const& targetStates, storm::abstraction::MenuGameRefiner<Type, ValueType> const& refiner, boost::optional<SymbolicQualitativeGameResultMinMax<Type>>& previousQualitativeResult, boost::optional<abstraction::SymbolicQuantitativeGameResult<Type, ValueType>>& previousMinQuantitativeResult);
            std::unique_ptr<CheckResult> performExplicitAbstractionSolutionStep(Environment const& env, CheckTask<storm::logic::Formula> const& checkTask, storm::abstraction::MenuGame<Type, ValueType> const& game, storm::OptimizationDirection player1Direction, storm::dd::Bdd<Type> const& initialStates, storm::dd::Bdd<Type> const& constraintStates, storm::dd::Bdd<Type> const& targetStates, storm::abstraction::MenuGameRefiner<Type, ValueType> const& refiner, boost::optional<ExplicitQualitativeGameResultMinMax>& previousQualitativeResult);

            /*!
             * Retrieves the initial predicates for the abstraction.
             */
            std::vector<storm::expressions::Expression> getInitialPredicates(storm::expressions::Expression const& constraintExpression, storm::expressions::Expression const& targetStateExpression);
            
            /*!
             * Derives the optimization direction of player 1.
             */
            storm::OptimizationDirection getPlayer1Direction(CheckTask<storm::logic::Formula> const& checkTask);
            
            /*!
             * Performs a qualitative check on the the given game to compute the (player 1) states that have probability
             * 0 or 1, respectively, to reach a target state and only visiting constraint states before.
             */
            SymbolicQualitativeGameResultMinMax<Type> computeProb01States(boost::optional<SymbolicQualitativeGameResultMinMax<Type>> const& previousQualitativeResult, storm::abstraction::MenuGame<Type, ValueType> const& game, storm::OptimizationDirection player1Direction, storm::dd::Bdd<Type> const& transitionMatrixBdd, storm::dd::Bdd<Type> const& constraintStates, storm::dd::Bdd<Type> const& targetStates);
            
            ExplicitQualitativeGameResultMinMax computeProb01States(boost::optional<ExplicitQualitativeGameResultMinMax> const& previousQualitativeResult, storm::OptimizationDirection player1Direction, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<uint64_t> const& player1RowGrouping, storm::storage::SparseMatrix<ValueType> const& player1BackwardTransitions, std::vector<uint64_t> const& player2BackwardTransitions, storm::storage::BitVector const& constraintStates, storm::storage::BitVector const& targetStates);
            
            void printStatistics(storm::abstraction::MenuGameAbstractor<Type, ValueType> const& abstractor, storm::abstraction::MenuGame<Type, ValueType> const& game) const;
            
            /*
             * Retrieves the expression characterized by the formula. The formula needs to be propositional.
             */
            storm::expressions::Expression getExpression(storm::logic::Formula const& formula);
            
            /// The preprocessed model that contains only one module/automaton and otherwhise corresponds to the semantics
            /// of the original model description.
            storm::storage::SymbolicModelDescription preprocessedModel;
            
            /// A factory that is used for creating SMT solvers when needed.
            std::shared_ptr<storm::utility::solver::SmtSolverFactory> smtSolverFactory;
            
            /// A comparator that can be used for detecting convergence.
            storm::utility::ConstantsComparator<ValueType> comparator;
            
            /// A flag indicating whether to reuse the qualitative results.
            bool reuseQualitativeResults;
            
            /// A flag indicating whether to reuse the quantitative results.
            bool reuseQuantitativeResults;
            
            /// The maximal number of abstractions to perform.
            uint64_t maximalNumberOfAbstractions;
            
            /// The mode selected for solving the abstraction.
            storm::settings::modules::AbstractionSettings::SolveMode solveMode;
        };
    }
}

#endif /* STORM_MODELCHECKER_GAMEBASEDMDPMODELCHECKER_H_ */
