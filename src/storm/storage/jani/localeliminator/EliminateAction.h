#pragma once

#include "JaniLocalEliminator.h"

namespace storm{
    namespace jani{
        namespace elimination_actions{
        class EliminateAction : public JaniLocalEliminator::Action {
            public:
                explicit EliminateAction(const std::string &automatonName, const std::string &locationName);

                std::string getDescription() override;
                void doAction(JaniLocalEliminator::Session &session) override;
            private:
                void eliminateDestination(JaniLocalEliminator::Session &session, Automaton &automaton, Edge &edge, uint64_t destIndex, detail::Edges &outgoing);

                std::string automatonName;
                std::string locationName;
            };
        }
    }
}
