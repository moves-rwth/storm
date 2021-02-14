#pragma once

#include "JaniLocalEliminator.h"

namespace storm{
    namespace jani{
        namespace elimination_actions{
        class UnfoldAction : public JaniLocalEliminator::Action {
            public:
                explicit UnfoldAction(const std::string &automatonName, const std::string &variableName);

                std::string getDescription() override;
                void doAction(JaniLocalEliminator::Session &session) override;

                std::string automatonName;
                std::string variableName;
            };
        }
    }
}
