#pragma once

#include "JaniLocalEliminator.h"
#include "storm/storage/jani/localeliminator/FinishAction.h"
#include <boost/graph/adjacency_list.hpp>
#include <vector>

using namespace boost;

namespace storm{
    namespace jani{
        namespace elimination_actions {
            class AutomaticAction : public JaniLocalEliminator::Action {
            public:
                explicit AutomaticAction();

                std::string getDescription() override;

                void doAction(JaniLocalEliminator::Session &session) override;

            private:
                adjacency_list<vecS, vecS, directedS> buildVariableGraph(JaniLocalEliminator::Session &session);
                std::vector<std::string> collectExpressionVariableNames(JaniLocalEliminator::Session &session);
            };
        }
    }
}
