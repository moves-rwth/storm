#pragma once

#include "storm/storage/dd/DdType.h"
#include "storm/storage/dd/Add.h"
#include "storm/storage/dd/Bdd.h"

namespace storm {
    namespace abstraction {

        template<storm::dd::DdType Type, typename ValueType>
        struct QuantitativeGameResult {
            QuantitativeGameResult() = default;
            
            QuantitativeGameResult(std::pair<ValueType, ValueType> initialStatesRange, storm::dd::Add<Type, ValueType> const& values, storm::dd::Bdd<Type> const& player1Strategy, storm::dd::Bdd<Type> const& player2Strategy) : initialStatesRange(initialStatesRange), values(values), player1Strategy(player1Strategy), player2Strategy(player2Strategy) {
                // Intentionally left empty.
            }
            
            std::pair<ValueType, ValueType> initialStatesRange;
            storm::dd::Add<Type, ValueType> values;
            storm::dd::Bdd<Type> player1Strategy;
            storm::dd::Bdd<Type> player2Strategy;
        };
        
    }
}
