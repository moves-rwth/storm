#include "storm/solver/MinMaxLinearEquationSolverRequirements.h"

namespace storm {
    namespace solver {
        
        MinMaxLinearEquationSolverRequirements::MinMaxLinearEquationSolverRequirements(LinearEquationSolverRequirements const& linearEquationSolverRequirements) : lowerBoundsRequirement(linearEquationSolverRequirements.lowerBounds()), upperBoundsRequirement(linearEquationSolverRequirements.upperBounds()) {
            // Intentionally left empty.
        }
        
        MinMaxLinearEquationSolverRequirements& MinMaxLinearEquationSolverRequirements::requireNoEndComponents(bool critical) {
            noEndComponentsRequirement.enable(critical);
            return *this;
        }
        
        MinMaxLinearEquationSolverRequirements& MinMaxLinearEquationSolverRequirements::requireValidInitialScheduler(bool critical) {
            validInitialSchedulerRequirement.enable(critical);
            return *this;
        }
        
        MinMaxLinearEquationSolverRequirements& MinMaxLinearEquationSolverRequirements::requireLowerBounds(bool critical) {
            lowerBoundsRequirement.enable(critical);
            return *this;
        }
        
        MinMaxLinearEquationSolverRequirements& MinMaxLinearEquationSolverRequirements::requireUpperBounds(bool critical) {
            upperBoundsRequirement.enable(critical);
            return *this;
        }
        
        MinMaxLinearEquationSolverRequirements& MinMaxLinearEquationSolverRequirements::requireBounds(bool critical) {
            requireLowerBounds(critical);
            requireUpperBounds(critical);
            return *this;
        }
        
        SolverRequirement const&  MinMaxLinearEquationSolverRequirements::noEndComponents() const {
            return noEndComponentsRequirement;
        }
        
        SolverRequirement const&  MinMaxLinearEquationSolverRequirements::validInitialScheduler() const {
            return validInitialSchedulerRequirement;
        }
        
        SolverRequirement const&  MinMaxLinearEquationSolverRequirements::lowerBounds() const {
            return lowerBoundsRequirement;
        }
        
        SolverRequirement const&  MinMaxLinearEquationSolverRequirements::upperBounds() const {
            return upperBoundsRequirement;
        }
        
        SolverRequirement const&  MinMaxLinearEquationSolverRequirements::get(Element const& element) const {
            switch (element) {
                case Element::NoEndComponents: return noEndComponents(); break;
                case Element::ValidInitialScheduler: return validInitialScheduler(); break;
                case Element::LowerBounds: return lowerBounds(); break;
                case Element::UpperBounds: return upperBounds(); break;
            }
        }
        
        void MinMaxLinearEquationSolverRequirements::clearNoEndComponents() {
            noEndComponentsRequirement.clear();
            validInitialSchedulerRequirement.clear();
        }
        
        void MinMaxLinearEquationSolverRequirements::clearValidInitialScheduler() {
            validInitialSchedulerRequirement.clear();
        }
        
        void MinMaxLinearEquationSolverRequirements::clearLowerBounds() {
            lowerBoundsRequirement.clear();
        }
        
        void MinMaxLinearEquationSolverRequirements::clearUpperBounds() {
            upperBoundsRequirement.clear();
        }
        
        void MinMaxLinearEquationSolverRequirements::clearBounds() {
            clearLowerBounds();
            clearUpperBounds();
        }
        
        bool MinMaxLinearEquationSolverRequirements::hasEnabledRequirement() const {
            return noEndComponentsRequirement || validInitialSchedulerRequirement || lowerBoundsRequirement || upperBoundsRequirement;
        }
        
        bool MinMaxLinearEquationSolverRequirements::hasEnabledCriticalRequirement() const {
            return noEndComponentsRequirement.isCritical() || validInitialSchedulerRequirement.isCritical() || lowerBoundsRequirement.isCritical() || upperBoundsRequirement.isCritical();
        }
        
        std::string MinMaxLinearEquationSolverRequirements::getEnabledRequirementsAsString() const {
            std::string res = "[";
            bool first = true;
            if (noEndComponents()) {
                if (!first) { res += ", "; } else {first = false;}
                res += "NoEndComponents";
                if (noEndComponents().isCritical()) {
                    res += "(mandatory)";
                }
            }
            if (validInitialScheduler()) {
                if (!first) { res += ", "; } else {first = false;}
                res += "validInitialScheduler";
                if (validInitialScheduler().isCritical()) {
                    res += "(mandatory)";
                }
            }
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
