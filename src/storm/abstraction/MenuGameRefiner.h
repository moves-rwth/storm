#pragma once

#include <functional>
#include <vector>
#include <memory>

#include "storm/abstraction/QualitativeResultMinMax.h"
#include "storm/abstraction/QuantitativeResultMinMax.h"

#include "storm/storage/expressions/Expression.h"
#include "storm/storage/expressions/PredicateSplitter.h"
#include "storm/storage/expressions/EquivalenceChecker.h"

#include "storm/storage/dd/DdType.h"

#include "storm/utility/solver.h"

namespace storm {
    namespace abstraction {

        template <storm::dd::DdType DdType, typename ValueType>
        class MenuGameAbstractor;
        
        template <storm::dd::DdType DdType, typename ValueType>
        class MenuGame;
        
        template<storm::dd::DdType Type, typename ValueType>
        class MenuGameRefiner {
        public:
            /*!
             * Creates a refiner for the provided abstractor.
             */
            MenuGameRefiner(MenuGameAbstractor<Type, ValueType>& abstractor, std::unique_ptr<storm::solver::SmtSolver>&& smtSolver);
            
            /*!
             * Refines the abstractor with the given set of predicates.
             */
            void refine(std::vector<storm::expressions::Expression> const& predicates) const;
            
            /*!
             * Refines the abstractor based on the qualitative result by trying to derive suitable predicates.
             *
             * @param True if predicates for refinement could be derived, false otherwise.
             */
            bool refine(storm::abstraction::MenuGame<Type, ValueType> const& game, storm::dd::Bdd<Type> const& transitionMatrixBdd, QualitativeResultMinMax<Type> const& qualitativeResult) const;
            
            /*!
             * Refines the abstractor based on the quantitative result by trying to derive suitable predicates.
             *
             * @param True if predicates for refinement could be derived, false otherwise.
             */
            bool refine(storm::abstraction::MenuGame<Type, ValueType> const& game, storm::dd::Bdd<Type> const& transitionMatrixBdd, QuantitativeResultMinMax<Type, ValueType> const& quantitativeResult) const;
            
        private:
            /*!
             * Takes the given predicates, preprocesses them and then refines the abstractor.
             */
            bool performRefinement(std::vector<storm::expressions::Expression> const& predicates) const;
            
            /// The underlying abstractor to refine.
            std::reference_wrapper<MenuGameAbstractor<Type, ValueType>> abstractor;
            
            /// An object that can be used for splitting predicates.
            storm::expressions::PredicateSplitter splitter;
            
            /// An object that can be used to determine whether predicates are equivalent.
            storm::expressions::EquivalenceChecker equivalenceChecker;
        };
        
    }
}
