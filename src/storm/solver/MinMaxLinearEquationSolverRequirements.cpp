#include "storm/solver/MinMaxLinearEquationSolverRequirements.h"

namespace storm {
    namespace solver {
        
        MinMaxLinearEquationSolverRequirements::MinMaxLinearEquationSolverRequirements(LinearEquationSolverRequirements const& linearEquationSolverRequirements) : noEndComponents(false), validInitialScheduler(false), lowerBounds(linearEquationSolverRequirements.requiresLowerBounds()), upperBounds(linearEquationSolverRequirements.requiresUpperBounds()) {
            // Intentionally left empty.
        }
        
        MinMaxLinearEquationSolverRequirements& MinMaxLinearEquationSolverRequirements::requireNoEndComponents() {
            noEndComponents = true;
            return *this;
        }
        
        MinMaxLinearEquationSolverRequirements& MinMaxLinearEquationSolverRequirements::requireValidInitialScheduler() {
            validInitialScheduler = true;
            return *this;
        }
        
        MinMaxLinearEquationSolverRequirements& MinMaxLinearEquationSolverRequirements::requireLowerBounds() {
            lowerBounds = true;
            return *this;
        }
        
        MinMaxLinearEquationSolverRequirements& MinMaxLinearEquationSolverRequirements::requireUpperBounds() {
            upperBounds = true;
            return *this;
        }
        
        bool MinMaxLinearEquationSolverRequirements::requiresNoEndComponents() const {
            return noEndComponents;
        }
        
        bool MinMaxLinearEquationSolverRequirements::requiresValidIntialScheduler() const {
            return validInitialScheduler;
        }
        
        bool MinMaxLinearEquationSolverRequirements::requiresLowerBounds() const {
            return lowerBounds;
        }
        
        bool MinMaxLinearEquationSolverRequirements::requiresUpperBounds() const {
            return upperBounds;
        }
        
        bool MinMaxLinearEquationSolverRequirements::requires(Element const& element) const {
            switch (element) {
                case Element::NoEndComponents: return noEndComponents; break;
                case Element::ValidInitialScheduler: return validInitialScheduler; break;
                case Element::LowerBounds: return lowerBounds; break;
                case Element::UpperBounds: return upperBounds; break;
            }
        }
        
        void MinMaxLinearEquationSolverRequirements::clearNoEndComponents() {
            noEndComponents = false;
            validInitialScheduler = false;
        }
        
        void MinMaxLinearEquationSolverRequirements::clearValidInitialScheduler() {
            validInitialScheduler = false;
        }
        
        void MinMaxLinearEquationSolverRequirements::clearLowerBounds() {
            lowerBounds = false;
        }
        
        void MinMaxLinearEquationSolverRequirements::clearUpperBounds() {
            upperBounds = false;
        }
        
        void MinMaxLinearEquationSolverRequirements::clearBounds() {
            clearLowerBounds();
            clearUpperBounds();
        }
        
        bool MinMaxLinearEquationSolverRequirements::empty() const {
            return !noEndComponents && !validInitialScheduler && !lowerBounds && !upperBounds;
        }
        
    }
}
