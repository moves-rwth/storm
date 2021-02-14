#pragma once

#include "JaniLocalEliminator.h"

namespace storm{
    namespace jani{
        namespace elimination_actions{
        class AutomaticAction : public JaniLocalEliminator::Action {
            public:
                explicit AutomaticAction();
                std::string getDescription() override;
                void doAction(JaniLocalEliminator::Session &session) override;
            };
        }
    }
}
