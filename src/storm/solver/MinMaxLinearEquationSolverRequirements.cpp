#include "storm/solver/MinMaxLinearEquationSolverRequirements.h"

namespace storm {
    namespace solver {
        
        MinMaxLinearEquationSolverRequirements::MinMaxLinearEquationSolverRequirements() : noEndComponents(false), noZeroRewardEndComponents(false), validInitialScheduler(false), globalLowerBound(false), globalUpperBound(false) {
            // Intentionally left empty.
        }
        
        MinMaxLinearEquationSolverRequirements& MinMaxLinearEquationSolverRequirements::setNoEndComponents(bool value) {
            noEndComponents = value;
            return *this;
        }
        
        MinMaxLinearEquationSolverRequirements& MinMaxLinearEquationSolverRequirements::setNoZeroRewardEndComponents(bool value) {
            noZeroRewardEndComponents = value;
            return *this;
        }
        
        MinMaxLinearEquationSolverRequirements& MinMaxLinearEquationSolverRequirements::setValidInitialScheduler(bool value) {
            validInitialScheduler = value;
            return *this;
        }
        
        MinMaxLinearEquationSolverRequirements& MinMaxLinearEquationSolverRequirements::setGlobalLowerBound(bool value) {
            globalLowerBound = value;
            return *this;
        }
        
        MinMaxLinearEquationSolverRequirements& MinMaxLinearEquationSolverRequirements::setGlobalUpperBound(bool value) {
            globalUpperBound = value;
            return *this;
        }
        
        MinMaxLinearEquationSolverRequirements& MinMaxLinearEquationSolverRequirements::set(Element const& element, bool value) {
            switch (element) {
                case Element::NoEndComponents: noEndComponents = value; break;
                case Element::NoZeroRewardEndComponents: noZeroRewardEndComponents = value; break;
                case Element::ValidInitialScheduler: validInitialScheduler = value; break;
                case Element::GlobalLowerBound: globalLowerBound = value; break;
                case Element::GlobalUpperBound: globalUpperBound = value; break;
            }
            return *this;
        }
        
        bool MinMaxLinearEquationSolverRequirements::requires(Element const& element) {
            switch (element) {
                case Element::NoEndComponents: return noEndComponents; break;
                case Element::NoZeroRewardEndComponents: return noZeroRewardEndComponents; break;
                case Element::ValidInitialScheduler: return validInitialScheduler; break;
                case Element::GlobalLowerBound: return globalLowerBound; break;
                case Element::GlobalUpperBound: return globalUpperBound; break;
            }
        }
        
        bool MinMaxLinearEquationSolverRequirements::empty() const {
            return !noEndComponents && !noZeroRewardEndComponents && !validInitialScheduler && !globalLowerBound && !globalUpperBound;
        }
        
    }
}
