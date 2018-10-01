#pragma once

#include <string>

#include "storm/solver/LinearEquationSolverRequirements.h"
#include "storm/solver/SolverRequirement.h"

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
                LowerBounds,
                UpperBounds
            };
            
            // The type of a requirement.
            
            MinMaxLinearEquationSolverRequirements(LinearEquationSolverRequirements const& linearEquationSolverRequirements = LinearEquationSolverRequirements());
            
            MinMaxLinearEquationSolverRequirements& requireNoEndComponents(bool critical = true);
            MinMaxLinearEquationSolverRequirements& requireValidInitialScheduler(bool critical = true);
            MinMaxLinearEquationSolverRequirements& requireLowerBounds(bool critical = true);
            MinMaxLinearEquationSolverRequirements& requireUpperBounds(bool critical = true);
            MinMaxLinearEquationSolverRequirements& requireBounds(bool critical = true);

            SolverRequirement const& noEndComponents() const;
            SolverRequirement const& validInitialScheduler() const;
            SolverRequirement const& lowerBounds() const;
            SolverRequirement const& upperBounds() const;
            SolverRequirement const& get(Element const& element) const;
            
            void clearNoEndComponents();
            void clearValidInitialScheduler();
            void clearLowerBounds();
            void clearUpperBounds();
            void clearBounds();
            
            bool hasEnabledRequirement() const;
            bool hasEnabledCriticalRequirement() const;
            
            /*!
             * Returns a string that enumerates the enabled requirements
             */
            std::string getEnabledRequirementsAsString() const;
            
        private:
            SolverRequirement noEndComponentsRequirement;
            SolverRequirement validInitialSchedulerRequirement;
            SolverRequirement lowerBoundsRequirement;
            SolverRequirement upperBoundsRequirement;
        };
        
    }
}
