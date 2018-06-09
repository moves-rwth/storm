#pragma once

#include <string>

#include "storm/solver/SolverRequirement.h"

namespace storm {
    namespace solver {
        
        class LinearEquationSolverRequirements {
        public:
            // The different requirements a solver can have.
            enum class Element {
                // Requirements that are related to bounds for the actual solution.
                LowerBounds,
                UpperBounds
            };
            
            LinearEquationSolverRequirements();
            
            LinearEquationSolverRequirements& requireLowerBounds(bool critical = true);
            LinearEquationSolverRequirements& requireUpperBounds(bool critical = true);
            LinearEquationSolverRequirements& requireBounds(bool critical = true);
            
            SolverRequirement const& lowerBounds() const;
            SolverRequirement const& upperBounds() const;
            SolverRequirement const& get(Element const& element) const;
            
            void clearLowerBounds();
            void clearUpperBounds();
            
            bool hasEnabledRequirement() const;
            bool hasEnabledCriticalRequirement() const;
            
            /*!
             * Checks whether there are no critical requirements left.
             * In case there is a critical requirement left an exception is thrown.
             */
            std::string getEnabledRequirementsAsString() const;
            
        private:
            SolverRequirement lowerBoundsRequirement;
            SolverRequirement upperBoundsRequirement;
        };
        
    }
}
