#pragma once

#include "storm/solver/MinMaxLinearEquationSolver.h"

#include "storm/solver/SolverSelectionOptions.h"
#include "storm/storage/StronglyConnectedComponentDecomposition.h"

namespace storm {
    
    class Environment;
    
    namespace solver {
        
        template<typename ValueType>
        class TopologicalMinMaxLinearEquationSolver : public MinMaxLinearEquationSolver<ValueType> {
        public:
            TopologicalMinMaxLinearEquationSolver();
            TopologicalMinMaxLinearEquationSolver(storm::storage::SparseMatrix<ValueType> const& A);
            TopologicalMinMaxLinearEquationSolver(storm::storage::SparseMatrix<ValueType>&& A);
            
            virtual void setMatrix(storm::storage::SparseMatrix<ValueType> const& A) override;
            virtual void setMatrix(storm::storage::SparseMatrix<ValueType>&& A) override;
            
            virtual void clearCache() const override;

            virtual void repeatedMultiply(Environment const& env, OptimizationDirection d, std::vector<ValueType>& x, std::vector<ValueType> const* b, uint_fast64_t n = 1) const override;
            virtual MinMaxLinearEquationSolverRequirements getRequirements(Environment const& env, boost::optional<storm::solver::OptimizationDirection> const& direction = boost::none, bool const& hasInitialScheduler = false) const override ;

        protected:
            
            virtual bool internalSolveEquations(storm::Environment const& env, OptimizationDirection d, std::vector<ValueType>& x, std::vector<ValueType> const& b) const override;

        private:
            storm::Environment getEnvironmentForUnderlyingSolver(storm::Environment const& env, bool adaptPrecision = false) const;
            
            // Creates an SCC decomposition and sorts the SCCs according to a topological sort.
            void createSortedSccDecomposition(bool needLongestChainSize) const;
            
            // Solves the SCC with the given index
            // ... for the case that the SCC is trivial
            bool solveTrivialScc(uint64_t const& sccState, OptimizationDirection d, std::vector<ValueType>& globalX, std::vector<ValueType> const& globalB) const;
            // ... for the case that there is just one large SCC
            bool solveFullyConnectedEquationSystem(storm::Environment const& sccSolverEnvironment, OptimizationDirection d, std::vector<ValueType>& x, std::vector<ValueType> const& b) const;
            // ... for the remaining cases (1 < scc.size() < x.size())
            bool solveScc(storm::Environment const& sccSolverEnvironment, OptimizationDirection d, storm::storage::BitVector const& sccRowGroups, storm::storage::BitVector const& sccRows, std::vector<ValueType>& globalX, std::vector<ValueType> const& globalB) const;

            // If the solver takes posession of the matrix, we store the moved matrix in this member, so it gets deleted
            // when the solver is destructed.
            std::unique_ptr<storm::storage::SparseMatrix<ValueType>> localA;
            
            // A pointer to the original sparse matrix given to this solver. If the solver takes posession of the matrix
            // the pointer refers to localA.
            storm::storage::SparseMatrix<ValueType> const* A;
            
            // cached auxiliary data
            mutable std::unique_ptr<storm::storage::StronglyConnectedComponentDecomposition<ValueType>> sortedSccDecomposition;
            mutable boost::optional<uint64_t> longestSccChainSize;
            mutable std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<ValueType>> sccSolver;
            mutable std::unique_ptr<std::vector<ValueType>> auxiliaryRowGroupVector; // A.rowGroupCount() entries
        };
        
        template<typename ValueType>
        class TopologicalMinMaxLinearEquationSolverFactory : public MinMaxLinearEquationSolverFactory<ValueType> {
        public:
            using MinMaxLinearEquationSolverFactory<ValueType>::create;
            
            virtual std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<ValueType>> create(Environment const& env) const override;

        };
        
    }
}
