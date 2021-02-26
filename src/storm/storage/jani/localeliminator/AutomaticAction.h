#pragma once

#include "JaniLocalEliminator.h"
#include "storm/storage/jani/localeliminator/FinishAction.h"
#include <boost/graph/adjacency_list.hpp>
#include <vector>
#include "UnfoldDependencyGraph.h"

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
                void unfoldGroupAndDependencies(JaniLocalEliminator::Session &session, std::string autName,
                                                UnfoldDependencyGraph &dependencyGraph, uint32_t groupIndex);
            };
        }
    }
}
