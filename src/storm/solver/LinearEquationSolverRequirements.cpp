#include "storm/solver/LinearEquationSolverRequirements.h"

namespace storm {
    namespace solver {
        
        LinearEquationSolverRequirements::LinearEquationSolverRequirements()  {
            // Intentionally left empty.
        }
        
        LinearEquationSolverRequirements& LinearEquationSolverRequirements::requireLowerBounds(bool critical) {
            lowerBoundsRequirement.enable(critical);
            return *this;
        }
        
        LinearEquationSolverRequirements& LinearEquationSolverRequirements::requireUpperBounds(bool critical) {
            upperBoundsRequirement.enable(critical);
            return *this;
        }
        
        LinearEquationSolverRequirements& LinearEquationSolverRequirements::requireBounds(bool critical) {
            requireLowerBounds(critical);
            requireUpperBounds(critical);
            return *this;
        }
        
        SolverRequirement const& LinearEquationSolverRequirements::lowerBounds() const {
            return lowerBoundsRequirement;
        }
        
        SolverRequirement const& LinearEquationSolverRequirements::upperBounds() const {
            return upperBoundsRequirement;
        }
        
        SolverRequirement const& LinearEquationSolverRequirements::get(Element const& element) const {
            switch (element) {
                case Element::LowerBounds: return lowerBounds(); break;
                case Element::UpperBounds: return upperBounds(); break;
            }
        }
        
        void LinearEquationSolverRequirements::clearLowerBounds() {
            lowerBoundsRequirement.clear();
        }
        
        void LinearEquationSolverRequirements::clearUpperBounds() {
            upperBoundsRequirement.clear();
        }
        
        bool LinearEquationSolverRequirements::hasEnabledRequirement() const {
            return lowerBoundsRequirement || upperBoundsRequirement;
        }
        
        bool LinearEquationSolverRequirements::hasEnabledCriticalRequirement() const {
            return lowerBoundsRequirement.isCritical() || upperBoundsRequirement.isCritical();
        }
        
        
        std::string LinearEquationSolverRequirements::getEnabledRequirementsAsString() const {
            std::string res = "[";
            bool first = true;
            if (lowerBounds()) {
                if (!first) { res += ", "; } else {first = false;}
                res += "lowerBounds";
                if (lowerBounds().isCritical()) {
                    res += "(mandatory)";
                }
            }
            if (upperBounds()) {
                if (!first) { res += ", "; } else {first = false;}
                res += "upperBounds";
                if (upperBounds().isCritical()) {
                    res += "(mandatory)";
                }
            }
            res += "]";
            return res;
        }
      
    }
}
