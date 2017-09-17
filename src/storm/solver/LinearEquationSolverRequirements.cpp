#include "storm/solver/LinearEquationSolverRequirements.h"

namespace storm {
    namespace solver {
        
        LinearEquationSolverRequirements::LinearEquationSolverRequirements() : lowerBounds(false), upperBounds(false) {
            // Intentionally left empty.
        }
        
        LinearEquationSolverRequirements& LinearEquationSolverRequirements::requireLowerBounds() {
            lowerBounds = true;
            return *this;
        }
        
        LinearEquationSolverRequirements& LinearEquationSolverRequirements::requireUpperBounds() {
            upperBounds = true;
            return *this;
        }
        
        bool LinearEquationSolverRequirements::requiresLowerBounds() const {
            return lowerBounds;
        }
        
        bool LinearEquationSolverRequirements::requiresUpperBounds() const {
            return upperBounds;
        }
        
        bool LinearEquationSolverRequirements::requires(Element const& element) const {
            switch (element) {
                case Element::LowerBounds: return lowerBounds; break;
                case Element::UpperBounds: return upperBounds; break;
            }
        }
        
        void LinearEquationSolverRequirements::clearLowerBounds() {
            lowerBounds = false;
        }
        
        void LinearEquationSolverRequirements::clearUpperBounds() {
            upperBounds = false;
        }
        
        bool LinearEquationSolverRequirements::empty() const {
            return !lowerBounds && !upperBounds;
        }
        
    }
}
