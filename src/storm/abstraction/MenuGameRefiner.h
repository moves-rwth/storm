#pragma once

#include <functional>
#include <vector>
#include <memory>

#include <boost/optional.hpp>

#include "storm/abstraction/RefinementCommand.h"
#include "storm/abstraction/QualitativeGameResultMinMax.h"
#include "storm/abstraction/QuantitativeGameResultMinMax.h"

#include "storm/storage/expressions/Expression.h"
#include "storm/storage/expressions/FullPredicateSplitter.h"
#include "storm/storage/expressions/EquivalenceChecker.h"

#include "storm/storage/dd/DdType.h"

#include "storm/settings/modules/AbstractionSettings.h"

#include "storm/utility/solver.h"

namespace storm {
    namespace abstraction {

        template <storm::dd::DdType Type, typename ValueType>
        class MenuGameAbstractor;
        
        template <storm::dd::DdType Type, typename ValueType>
        class MenuGame;
        
        class RefinementPredicates {
        public:
            enum class Source {
                WeakestPrecondition, InitialGuard, Guard, Interpolation, Manual
            };
            
            RefinementPredicates() = default;
            RefinementPredicates(Source const& source, std::vector<storm::expressions::Expression> const& predicates);
            
            Source getSource() const;
            std::vector<storm::expressions::Expression> const& getPredicates() const;
            void addPredicates(std::vector<storm::expressions::Expression> const& newPredicates);
            
        private:
            Source source;
            std::vector<storm::expressions::Expression> predicates;
        };

        template<storm::dd::DdType Type, typename ValueType>
        struct MostProbablePathsResult {
            MostProbablePathsResult() = default;
            MostProbablePathsResult(storm::dd::Add<Type, ValueType> const& maxProbabilities, storm::dd::Bdd<Type> const& spanningTree);
            
            storm::dd::Add<Type, ValueType> maxProbabilities;
            storm::dd::Bdd<Type> spanningTree;
        };
        
        template<storm::dd::DdType Type, typename ValueType>
        struct PivotStateResult {
            PivotStateResult(storm::dd::Bdd<Type> const& pivotState, storm::OptimizationDirection fromDirection, boost::optional<MostProbablePathsResult<Type, ValueType>> const& mostProbablePathsResult = boost::none);
            
            storm::dd::Bdd<Type> pivotState;
            storm::OptimizationDirection fromDirection;
            boost::optional<MostProbablePathsResult<Type, ValueType>> mostProbablePathsResult;
        };
        
        template<storm::dd::DdType Type, typename ValueType>
        class MenuGameRefiner {
        public:
            /*!
             * Creates a refiner for the provided abstractor.
             */
            MenuGameRefiner(MenuGameAbstractor<Type, ValueType>& abstractor, std::unique_ptr<storm::solver::SmtSolver>&& smtSolver);
            
            /*!
             * Refines the abstractor with the given predicates.
             *
             * @param predicates The predicates to use for refinement.
             */
            void refine(std::vector<storm::expressions::Expression> const& predicates) const;
            
            /*!
             * Refines the abstractor based on the qualitative result by trying to derive suitable predicates.
             *
             * @param True if predicates for refinement could be derived, false otherwise.
             */
            bool refine(storm::abstraction::MenuGame<Type, ValueType> const& game, storm::dd::Bdd<Type> const& transitionMatrixBdd, QualitativeGameResultMinMax<Type> const& qualitativeResult) const;
            
            /*!
             * Refines the abstractor based on the quantitative result by trying to derive suitable predicates.
             *
             * @param True if predicates for refinement could be derived, false otherwise.
             */
            bool refine(storm::abstraction::MenuGame<Type, ValueType> const& game, storm::dd::Bdd<Type> const& transitionMatrixBdd, QuantitativeGameResultMinMax<Type, ValueType> const& quantitativeResult) const;
            
            /*!
             * Retrieves whether all guards were added.
             */
            bool addedAllGuards() const;
            
        private:
            RefinementPredicates derivePredicatesFromDifferingChoices(storm::dd::Bdd<Type> const& player1Choice, storm::dd::Bdd<Type> const& lowerChoice, storm::dd::Bdd<Type> const& upperChoice) const;
            RefinementPredicates derivePredicatesFromPivotState(storm::abstraction::MenuGame<Type, ValueType> const& game, storm::dd::Bdd<Type> const& pivotState, storm::dd::Bdd<Type> const& minPlayer1Strategy, storm::dd::Bdd<Type> const& minPlayer2Strategy, storm::dd::Bdd<Type> const& maxPlayer1Strategy, storm::dd::Bdd<Type> const& maxPlayer2Strategy) const;
            
            /*!
             * Preprocesses the predicates.
             */
            std::vector<storm::expressions::Expression> preprocessPredicates(std::vector<storm::expressions::Expression> const& predicates, RefinementPredicates::Source const& source) const;
            
            /*!
             * Creates a set of refinement commands that amounts to splitting all player 1 choices with the given set of predicates.
             */
            std::vector<RefinementCommand> createGlobalRefinement(std::vector<storm::expressions::Expression> const& predicates) const;
            
            boost::optional<RefinementPredicates> derivePredicatesFromInterpolation(storm::abstraction::MenuGame<Type, ValueType> const& game, PivotStateResult<Type, ValueType> const& pivotStateResult, storm::dd::Bdd<Type> const& minPlayer1Strategy, storm::dd::Bdd<Type> const& minPlayer2Strategy, storm::dd::Bdd<Type> const& maxPlayer1Strategy, storm::dd::Bdd<Type> const& maxPlayer2Strategy) const;
            std::pair<std::vector<std::vector<storm::expressions::Expression>>, std::map<storm::expressions::Variable, storm::expressions::Expression>> buildTrace(storm::expressions::ExpressionManager& expressionManager, storm::abstraction::MenuGame<Type, ValueType> const& game, storm::dd::Bdd<Type> const& spanningTree, storm::dd::Bdd<Type> const& pivotState) const;
            
            void performRefinement(std::vector<RefinementCommand> const& refinementCommands) const;
            
            /// The underlying abstractor to refine.
            std::reference_wrapper<MenuGameAbstractor<Type, ValueType>> abstractor;
            
            /// A flag indicating whether interpolation shall be used to rule out spurious pivot blocks.
            bool useInterpolation;

            /// A flag indicating whether all predicates shall be split before using them for refinement.
            bool splitAll;
            
            /// A flag indicating whether predicates derived from weakest preconditions shall be split before using them for refinement.
            bool splitPredicates;
            
            /// A flag indicating whether all guards have been used to refine the abstraction.
            bool addedAllGuardsFlag;

            /// The heuristic to use for pivot block selection.
            storm::settings::modules::AbstractionSettings::PivotSelectionHeuristic pivotSelectionHeuristic;
            
            /// An object that can be used for splitting predicates.
            mutable storm::expressions::FullPredicateSplitter splitter;
            
            /// An object that can be used to determine whether predicates are equivalent.
            mutable storm::expressions::EquivalenceChecker equivalenceChecker;
        };
        
    }
}
