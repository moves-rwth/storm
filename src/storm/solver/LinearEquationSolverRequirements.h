#pragma once

namespace storm {
    namespace solver {
        
        class LinearEquationSolverRequirements {
        public:
            // The different requirements a solver can have.
            enum class Element {
                // Requirements that are related to bounds for the actual solution.
                GlobalLowerBound,
                GlobalUpperBound
            };
            
            LinearEquationSolverRequirements();
            
            LinearEquationSolverRequirements& requireGlobalLowerBound();
            LinearEquationSolverRequirements& requireGlobalUpperBound();
            
            bool requiresGlobalLowerBound() const;
            bool requiresGlobalUpperBound() const;
            bool requires(Element const& element) const;
            
            void clearGlobalLowerBound();
            void clearGlobalUpperBound();
            
            bool empty() const;
            
        private:
            bool globalLowerBound;
            bool globalUpperBound;
        };
        
    }
}
