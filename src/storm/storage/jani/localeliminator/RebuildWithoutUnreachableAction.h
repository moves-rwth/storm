#pragma once

#include "JaniLocalEliminator.h"

namespace storm{
    namespace jani{
        namespace elimination_actions{
        class RebuildWithoutUnreachableAction : public JaniLocalEliminator::Action {
            public:
                explicit RebuildWithoutUnreachableAction();
                std::string getDescription() override;
                void doAction(JaniLocalEliminator::Session &session) override;
            };
        }
    }
}
