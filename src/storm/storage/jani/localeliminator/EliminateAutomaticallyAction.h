#pragma once

#include "JaniLocalEliminator.h"

namespace storm{
    namespace jani{
        namespace elimination_actions{
        class EliminateAutomaticallyAction : public JaniLocalEliminator::Action {
            public:
                enum class EliminationOrder {
                    Arbitrary,
                    NewTransitionCount
                };

                explicit EliminateAutomaticallyAction(const std::string &automatonName, EliminationOrder eliminationOrder, uint32_t transitionCountThreshold = 1000);
                std::string getDescription() override;
                void doAction(JaniLocalEliminator::Session &session) override;
            private:
                std::string automatonName;
                EliminationOrder eliminationOrder;
                std::string find_next_location(JaniLocalEliminator::Session &session);
                uint32_t transitionCountThreshold;
            };
        }
    }
}
