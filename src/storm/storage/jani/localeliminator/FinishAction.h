#pragma once

#include "JaniLocalEliminator.h"

namespace storm{
    namespace jani{
        namespace elimination_actions{
            class FinishAction : public JaniLocalEliminator::Action {
            public:
                explicit FinishAction();
                std::string getDescription() override;
                void doAction(JaniLocalEliminator::Session &session) override;
            };
        }

    }
}
