#pragma once

#include "storm/storage/dd/DdType.h"
#include "storm/storage/dd/Add.h"
#include "storm/storage/dd/Bdd.h"

namespace storm {
    namespace abstraction {

        template<storm::dd::DdType Type, typename ValueType>
        struct QuantitativeGameResult {
            QuantitativeGameResult() = default;

            QuantitativeGameResult(storm::dd::Add<Type, ValueType> const& values) : values(values) {
                // Intentionally left empty.
            }
            
            QuantitativeGameResult(boost::optional<std::pair<ValueType, ValueType>> const& initialStatesRange, storm::dd::Add<Type, ValueType> const& values, boost::optional<storm::dd::Bdd<Type>> const& player1Strategy, boost::optional<storm::dd::Bdd<Type>> const& player2Strategy) : initialStatesRange(initialStatesRange), values(values), player1Strategy(player1Strategy), player2Strategy(player2Strategy) {
                // Intentionally left empty.
            }
            
            bool hasPlayer1Strategy() const {
                return static_cast<bool>(player1Strategy);
            }

            storm::dd::Bdd<Type> const& getPlayer1Strategy() const {
                return player1Strategy.get();
            }

            storm::dd::Bdd<Type>& getPlayer1Strategy() {
                return player1Strategy.get();
            }

            bool hasPlayer2Strategy() const {
                return static_cast<bool>(player2Strategy);
            }

            storm::dd::Bdd<Type> const& getPlayer2Strategy() const {
                return player2Strategy.get();
            }

            storm::dd::Bdd<Type>& getPlayer2Strategy() {
                return player2Strategy.get();
            }

            bool hasInitialStatesRange() const {
                return static_cast<bool>(initialStatesRange);
            }
            
            std::pair<ValueType, ValueType> const& getInitialStatesRange() const {
                return initialStatesRange.get();
            }

            boost::optional<std::pair<ValueType, ValueType>> initialStatesRange;
            storm::dd::Add<Type, ValueType> values;
            boost::optional<storm::dd::Bdd<Type>> player1Strategy;
            boost::optional<storm::dd::Bdd<Type>> player2Strategy;
        };
        
    }
}
