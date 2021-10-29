#pragma once

#include "JaniLocalEliminator.h"
#include "storm/storage/jani/localeliminator/FinishAction.h"
#include <vector>
#include "UnfoldDependencyGraph.h"
#include <boost/optional.hpp>

using namespace boost;

namespace storm{
    namespace jani{
        namespace elimination_actions {
            class AutomaticAction : public JaniLocalEliminator::Action {
            public:
                explicit AutomaticAction();
                explicit AutomaticAction(uint64_t locationLimit, uint64_t newTransitionLimit, uint64_t maxDomainSize = 100, bool flatten = true);

                std::string getDescription() override;

                void doAction(JaniLocalEliminator::Session &session) override;

            private:
                bool flatten;
                uint64_t maxDomainSize;
                uint64_t locationLimit;
                uint64_t newTransitionLimit;

                void processAutomaton(JaniLocalEliminator::Session &session, std::string const& autName);

                void unfoldGroupAndDependencies(JaniLocalEliminator::Session &session, std::string autName,
                                                UnfoldDependencyGraph &dependencyGraph, uint32_t groupIndex);
                bool unfoldPropertyVariable(JaniLocalEliminator::Session &session, std::string const& autName, UnfoldDependencyGraph& dependencyGraph);

                boost::optional<uint32_t> chooseNextUnfold(JaniLocalEliminator::Session &session, std::string const& automatonName, UnfoldDependencyGraph &dependencyGraph, bool onlyPropertyVariables);
                std::map<std::string, double> getAssignmentCountByVariable(JaniLocalEliminator::Session &session, std::string const& automatonName);
            };
        }
    }
}
