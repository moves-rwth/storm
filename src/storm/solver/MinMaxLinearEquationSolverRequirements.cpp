#include "storm/solver/MinMaxLinearEquationSolverRequirements.h"

namespace storm {
    namespace solver {
        
        MinMaxLinearEquationSolverRequirements::MinMaxLinearEquationSolverRequirements(LinearEquationSolverRequirements const& linearEquationSolverRequirements) : noEndComponents(false), validInitialScheduler(false), globalLowerBound(linearEquationSolverRequirements.requiresGlobalLowerBound()), globalUpperBound(linearEquationSolverRequirements.requiresGlobalUpperBound()) {
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
        
        MinMaxLinearEquationSolverRequirements& MinMaxLinearEquationSolverRequirements::requireGlobalLowerBound() {
            globalLowerBound = true;
            return *this;
        }
        
        MinMaxLinearEquationSolverRequirements& MinMaxLinearEquationSolverRequirements::requireGlobalUpperBound() {
            globalUpperBound = true;
            return *this;
        }
        
        bool MinMaxLinearEquationSolverRequirements::requiresNoEndComponents() const {
            return noEndComponents;
        }
        
        bool MinMaxLinearEquationSolverRequirements::requiresValidIntialScheduler() const {
            return validInitialScheduler;
        }
        
        bool MinMaxLinearEquationSolverRequirements::requiresGlobalLowerBound() const {
            return globalLowerBound;
        }
        
        bool MinMaxLinearEquationSolverRequirements::requiresGlobalUpperBound() const {
            return globalUpperBound;
        }
        
        bool MinMaxLinearEquationSolverRequirements::requires(Element const& element) const {
            switch (element) {
                case Element::NoEndComponents: return noEndComponents; break;
                case Element::ValidInitialScheduler: return validInitialScheduler; break;
                case Element::GlobalLowerBound: return globalLowerBound; break;
                case Element::GlobalUpperBound: return globalUpperBound; break;
            }
        }
        
        void MinMaxLinearEquationSolverRequirements::clearNoEndComponents() {
            noEndComponents = false;
            validInitialScheduler = false;
        }
        
        void MinMaxLinearEquationSolverRequirements::clearValidInitialScheduler() {
            validInitialScheduler = false;
        }
        
        void MinMaxLinearEquationSolverRequirements::clearGlobalLowerBound() {
            globalLowerBound = false;
        }
        
        void MinMaxLinearEquationSolverRequirements::clearGlobalUpperBound() {
            globalUpperBound = false;
        }
        
        bool MinMaxLinearEquationSolverRequirements::empty() const {
            return !noEndComponents && !validInitialScheduler && !globalLowerBound && !globalUpperBound;
        }
        
    }
}
