#pragma once

#include <map>
#include <vector>

#include "storm/storage/dd/DdType.h"

#include "storm/abstraction/MenuGame.h"
#include "storm/abstraction/RefinementCommand.h"

#include "storm/storage/expressions/Expression.h"

namespace storm {
    namespace abstraction {
        
        template <storm::dd::DdType DdType>
        class AbstractionInformation;
        
        template <storm::dd::DdType DdType, typename ValueType>
        class MenuGameAbstractor {
        public:
            /// Retrieves the abstraction.
            virtual MenuGame<DdType, ValueType> abstract() = 0;

            /// Retrieves information about the abstraction.
            virtual AbstractionInformation<DdType> const& getAbstractionInformation() const = 0;
            virtual storm::expressions::Expression const& getGuard(uint64_t player1Choice) const = 0;
            virtual std::pair<uint64_t, uint64_t> getPlayer1ChoiceRange() const = 0;
            virtual std::map<storm::expressions::Variable, storm::expressions::Expression> getVariableUpdates(uint64_t player1Choice, uint64_t auxiliaryChoice) const = 0;
            
            /// Methods to refine the abstraction.
            virtual void refine(RefinementCommand const& command) = 0;
            
            /// Exports a representation of the current abstraction state in the dot format.
            virtual void exportToDot(std::string const& filename, storm::dd::Bdd<DdType> const& highlightStates, storm::dd::Bdd<DdType> const& filter) const = 0;
        };
        
    }
}
