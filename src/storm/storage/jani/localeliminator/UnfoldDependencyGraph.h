#pragma once

#include "storm/storage/jani/Model.h"
#include <vector>
#include <string>
#include <set>

namespace storm {
    namespace jani {
        namespace elimination_actions {
            class UnfoldDependencyGraph{
            public:
                class VariableInfo{
                public:
                    std::string expressionVariableName;
                    std::string janiVariableName;
                    bool isGlobal;
                    std::string automatonName;
                    bool isConstBoundedInteger;
                    int domainSize;

                    VariableInfo(std::string expressionVariableName, std::string janiVariableName, bool isGlobal, std::string automatonName, bool isConstBoundedInteger, int domainSize);
                };

                class VariableGroup{
                public:
                    std::vector<VariableInfo> variables;
                    uint32_t domainSize;
                    bool allVariablesUnfoldable;
                    bool unfolded;
                    std::set<uint32_t> dependencies;

                    VariableGroup();
                    void addVariable(VariableInfo variable);
                };

                std::vector<VariableGroup> variableGroups;

                explicit UnfoldDependencyGraph(Model &model);

                bool canUnfold(uint32_t groupIndex);
                void markUnfolded(uint32_t groupIndex);
                uint32_t findGroupIndex(std::string variableName);

                std::vector<uint32_t> getOrderedDependencies(uint32_t groupIndex);
                uint32_t getTotalBlowup(std::vector<uint32_t> groups);

                std::set<uint32_t> getGroupsWithNoDependencies();
                void printGroups();


            private:
                void buildGroups(Model &model);

            };
        }
    }
}