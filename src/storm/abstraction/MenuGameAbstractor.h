#pragma once

#include <map>
#include <vector>

#include "storm/storage/dd/DdType.h"

#include "storm/abstraction/MenuGame.h"
#include "storm/abstraction/RefinementCommand.h"

#include "storm/storage/expressions/Expression.h"

namespace storm {
    namespace dd {
        template <storm::dd::DdType DdType>
        class Bdd;
    }
    
    namespace abstraction {
        
        template <storm::dd::DdType DdType>
        class AbstractionInformation;
        
        template <storm::dd::DdType DdType, typename ValueType>
        class MenuGameAbstractor {
        public:
            virtual ~MenuGameAbstractor() = default;

            /// Retrieves the abstraction.
            virtual MenuGame<DdType, ValueType> abstract() = 0;

            /// Retrieves information about the abstraction.
            virtual AbstractionInformation<DdType> const& getAbstractionInformation() const = 0;
            virtual storm::expressions::Expression const& getGuard(uint64_t player1Choice) const = 0;
            virtual std::pair<uint64_t, uint64_t> getPlayer1ChoiceRange() const = 0;
            virtual std::map<storm::expressions::Variable, storm::expressions::Expression> getVariableUpdates(uint64_t player1Choice, uint64_t auxiliaryChoice) const = 0;
            virtual storm::expressions::Expression getInitialExpression() const = 0;
            
            /*!
             * Retrieves a BDD that characterizes the states corresponding to the given expression. For this to work,
             * appropriate predicates must have been used to refine the abstraction, otherwise this will fail.
             */
            virtual storm::dd::Bdd<DdType> getStates(storm::expressions::Expression const& expression) = 0;
            
            /// Methods to refine the abstraction.
            virtual void refine(RefinementCommand const& command) = 0;

            /// Exports a representation of the current abstraction state in the dot format.
            virtual void exportToDot(std::string const& filename, storm::dd::Bdd<DdType> const& highlightStatesBdd, storm::dd::Bdd<DdType> const& filter) const = 0;

            /// Retrieves the number of predicates currently in use.
            virtual uint64_t getNumberOfPredicates() const = 0;
            
        protected:
            void exportToDot(storm::abstraction::MenuGame<DdType, ValueType> const& currentGame, std::string const& filename, storm::dd::Bdd<DdType> const& highlightStatesBdd, storm::dd::Bdd<DdType> const& filter) const;
        };
        
    }
}
