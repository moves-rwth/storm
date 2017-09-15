#include "storm/solver/LinearEquationSolverRequirements.h"

namespace storm {
    namespace solver {
        
        LinearEquationSolverRequirements::LinearEquationSolverRequirements() : globalLowerBound(false), globalUpperBound(false) {
            // Intentionally left empty.
        }
        
        LinearEquationSolverRequirements& LinearEquationSolverRequirements::requireGlobalLowerBound() {
            globalLowerBound = true;
            return *this;
        }
        
        LinearEquationSolverRequirements& LinearEquationSolverRequirements::requireGlobalUpperBound() {
            globalUpperBound = true;
            return *this;
        }
        
        bool LinearEquationSolverRequirements::requiresGlobalLowerBound() const {
            return globalLowerBound;
        }
        
        bool LinearEquationSolverRequirements::requiresGlobalUpperBound() const {
            return globalUpperBound;
        }
        
        bool LinearEquationSolverRequirements::requires(Element const& element) const {
            switch (element) {
                case Element::GlobalLowerBound: return globalLowerBound; break;
                case Element::GlobalUpperBound: return globalUpperBound; break;
            }
        }
        
        void LinearEquationSolverRequirements::clearGlobalLowerBound() {
            globalLowerBound = false;
        }
        
        void LinearEquationSolverRequirements::clearGlobalUpperBound() {
            globalUpperBound = false;
        }
        
        bool LinearEquationSolverRequirements::empty() const {
            return !globalLowerBound && !globalUpperBound;
        }
        
    }
}
