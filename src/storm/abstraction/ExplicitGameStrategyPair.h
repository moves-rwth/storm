#pragma once

#include <cstdint>

#include "storm/abstraction/ExplicitGameStrategy.h"

namespace storm {
    namespace abstraction {
        
        class ExplicitGameStrategyPair {
        public:
            ExplicitGameStrategyPair(uint64_t numberOfPlayer1States, uint64_t numberOfPlayer2States);
            ExplicitGameStrategyPair(ExplicitGameStrategy&& player1Strategy, ExplicitGameStrategy&& player2Strategy);

            ExplicitGameStrategy& getPlayer1Strategy();
            ExplicitGameStrategy const& getPlayer1Strategy() const;
            ExplicitGameStrategy& getPlayer2Strategy();
            ExplicitGameStrategy const& getPlayer2Strategy() const;

        private:
            ExplicitGameStrategy player1Strategy;
            ExplicitGameStrategy player2Strategy;
        };
        
    }
}
