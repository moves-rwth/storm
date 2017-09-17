#pragma once

namespace storm {
    namespace solver {
        
        class LinearEquationSolverRequirements {
        public:
            // The different requirements a solver can have.
            enum class Element {
                // Requirements that are related to bounds for the actual solution.
                LowerBounds,
                UpperBounds
            };
            
            LinearEquationSolverRequirements();
            
            LinearEquationSolverRequirements& requireLowerBounds();
            LinearEquationSolverRequirements& requireUpperBounds();
            
            bool requiresLowerBounds() const;
            bool requiresUpperBounds() const;
            bool requires(Element const& element) const;
            
            void clearLowerBounds();
            void clearUpperBounds();
            
            bool empty() const;
            
        private:
            bool lowerBounds;
            bool upperBounds;
        };
        
    }
}
