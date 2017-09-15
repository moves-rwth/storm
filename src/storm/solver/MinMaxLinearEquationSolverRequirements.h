#pragma once

#include "storm/solver/LinearEquationSolverRequirements.h"

namespace storm {
    namespace solver {
        
        class MinMaxLinearEquationSolverRequirements {
        public:
            // The different requirements a solver can have.
            enum class Element {
                // Requirements that are related to the graph structure of the system. Note that the requirements in this
                // category are to be interpreted incrementally in the following sense: whenever the system has no end
                // components then automatically both requirements are fulfilled.
                NoEndComponents,
                ValidInitialScheduler,
                
                // Requirements that are related to bounds for the actual solution.
                GlobalLowerBound,
                GlobalUpperBound
            };
            
            MinMaxLinearEquationSolverRequirements(LinearEquationSolverRequirements const& linearEquationSolverRequirements = LinearEquationSolverRequirements());
            
            MinMaxLinearEquationSolverRequirements& requireNoEndComponents();
            MinMaxLinearEquationSolverRequirements& requireValidInitialScheduler();
            MinMaxLinearEquationSolverRequirements& requireGlobalLowerBound();
            MinMaxLinearEquationSolverRequirements& requireGlobalUpperBound();

            bool requiresNoEndComponents() const;
            bool requiresValidIntialScheduler() const;
            bool requiresGlobalLowerBound() const;
            bool requiresGlobalUpperBound() const;
            bool requires(Element const& element) const;
            
            void clearNoEndComponents();
            void clearValidInitialScheduler();
            void clearGlobalLowerBound();
            void clearGlobalUpperBound();
            
            bool empty() const;
            
        private:
            bool noEndComponents;
            bool validInitialScheduler;
            bool globalLowerBound;
            bool globalUpperBound;
        };
        
    }
}
