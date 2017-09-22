#pragma once

namespace storm {
    namespace solver {
        
        class MinMaxLinearEquationSolverRequirements {
        public:
            enum class Element {
                NoEndComponents, NoZeroRewardEndComponents, ValidInitialScheduler, GlobalLowerBound, GlobalUpperBound
            };
            
            MinMaxLinearEquationSolverRequirements();
            
            MinMaxLinearEquationSolverRequirements& setNoEndComponents(bool value = true);
            MinMaxLinearEquationSolverRequirements& setNoZeroRewardEndComponents(bool value = true);
            MinMaxLinearEquationSolverRequirements& setValidInitialScheduler(bool value = true);
            MinMaxLinearEquationSolverRequirements& setGlobalLowerBound(bool value = true);
            MinMaxLinearEquationSolverRequirements& setGlobalUpperBound(bool value = true);
            MinMaxLinearEquationSolverRequirements& set(Element const& element, bool value = true);
            
            bool requires(Element const& element);
            
            bool empty() const;
            
        private:
            bool noEndComponents;
            bool noZeroRewardEndComponents;
            bool validInitialScheduler;
            bool globalLowerBound;
            bool globalUpperBound;
        };
        
    }
}
