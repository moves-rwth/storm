#pragma once

namespace storm {
    namespace solver {
        
        class MinMaxLinearEquationSolverRequirements {
        public:
            enum class Element {
                NoEndComponents, NoZeroRewardEndComponents, ValidInitialScheduler, GlobalLowerBound, GlobalUpperBound
            };
            
            MinMaxLinearEquationSolverRequirements() : noEndComponents(false), noZeroRewardEndComponents(false), validInitialScheduler(false), globalLowerBound(false), globalUpperBound(false) {
                // Intentionally left empty.
            }
            
            MinMaxLinearEquationSolverRequirements& setNoEndComponents(bool value = true) {
                noEndComponents = value;
                return *this;
            }
            
            MinMaxLinearEquationSolverRequirements& setNoZeroRewardEndComponents(bool value = true) {
                noZeroRewardEndComponents = value;
                return *this;
            }
            
            MinMaxLinearEquationSolverRequirements& setValidInitialScheduler(bool value = true) {
                validInitialScheduler = value;
                return *this;
            }
            
            MinMaxLinearEquationSolverRequirements& setGlobalLowerBound(bool value = true) {
                globalLowerBound = value;
                return *this;
            }
            
            MinMaxLinearEquationSolverRequirements& setGlobalUpperBound(bool value = true) {
                globalUpperBound = value;
                return *this;
            }
            
            MinMaxLinearEquationSolverRequirements& set(Element const& element, bool value = true) {
                switch (element) {
                    case Element::NoEndComponents: noEndComponents = value; break;
                    case Element::NoZeroRewardEndComponents: noZeroRewardEndComponents = value; break;
                    case Element::ValidInitialScheduler: validInitialScheduler = value; break;
                    case Element::GlobalLowerBound: globalLowerBound = value; break;
                    case Element::GlobalUpperBound: globalUpperBound = value; break;
                }
                return *this;
            }
            
            bool requires(Element const& element) {
                switch (element) {
                    case Element::NoEndComponents: return noEndComponents; break;
                    case Element::NoZeroRewardEndComponents: return noZeroRewardEndComponents; break;
                    case Element::ValidInitialScheduler: return validInitialScheduler; break;
                    case Element::GlobalLowerBound: return globalLowerBound; break;
                    case Element::GlobalUpperBound: return globalUpperBound; break;
                }
            }
            
            bool empty() const {
                return !noEndComponents && !noZeroRewardEndComponents && !validInitialScheduler && !globalLowerBound && !globalUpperBound;
            }
            
        private:
            bool noEndComponents;
            bool noZeroRewardEndComponents;
            bool validInitialScheduler;
            bool globalLowerBound;
            bool globalUpperBound;
        };
        
    }
}
