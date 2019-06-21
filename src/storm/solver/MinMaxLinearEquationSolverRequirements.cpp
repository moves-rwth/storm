#include "storm/solver/MinMaxLinearEquationSolverRequirements.h"

namespace storm {
    namespace solver {
        
        MinMaxLinearEquationSolverRequirements::MinMaxLinearEquationSolverRequirements(LinearEquationSolverRequirements const& linearEquationSolverRequirements) : lowerBoundsRequirement(linearEquationSolverRequirements.lowerBounds()), upperBoundsRequirement(linearEquationSolverRequirements.upperBounds()) {
            // Intentionally left empty.
        }
        
        MinMaxLinearEquationSolverRequirements& MinMaxLinearEquationSolverRequirements::requireUniqueSolution(bool critical) {
            uniqueSolutionRequirement.enable(critical);
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
        
        SolverRequirement const&  MinMaxLinearEquationSolverRequirements::uniqueSolution() const {
            return uniqueSolutionRequirement;
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
                case Element::UniqueSolution: return uniqueSolution(); break;
                case Element::ValidInitialScheduler: return validInitialScheduler(); break;
                case Element::LowerBounds: return lowerBounds(); break;
                case Element::UpperBounds: return upperBounds(); break;
            }
        }
        
        void MinMaxLinearEquationSolverRequirements::clearUniqueSolution() {
            uniqueSolutionRequirement.clear();
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
            return uniqueSolutionRequirement || validInitialSchedulerRequirement || lowerBoundsRequirement || upperBoundsRequirement;
        }
        
        bool MinMaxLinearEquationSolverRequirements::hasEnabledCriticalRequirement() const {
            return uniqueSolutionRequirement.isCritical() || validInitialSchedulerRequirement.isCritical() || lowerBoundsRequirement.isCritical() || upperBoundsRequirement.isCritical();
        }
        
        std::string MinMaxLinearEquationSolverRequirements::getEnabledRequirementsAsString() const {
            std::string res = "[";
            bool first = true;
            if (uniqueSolution()) {
                if (!first) { res += ", "; } else {first = false;}
                res += "UniqueSolution";
                if (uniqueSolution().isCritical()) {
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
