#pragma once

#include "storm/solver/MultiplicationStyle.h"

#include "storm/utility/NumberTraits.h"

#include "storm/solver/LinearEquationSolver.h"
#include "storm/solver/Multiplier.h"
#include "storm/solver/StandardMinMaxLinearEquationSolver.h"
#include "storm/solver/helper/SoundValueIterationHelper.h"

#include "storm/solver/SolverStatus.h"

namespace storm {
    
    class Environment;
    
    namespace solver {
        
        template<typename ValueType>
        class IterativeMinMaxLinearEquationSolver : public StandardMinMaxLinearEquationSolver<ValueType> {
        public:
            IterativeMinMaxLinearEquationSolver(std::unique_ptr<LinearEquationSolverFactory<ValueType>>&& linearEquationSolverFactory);
            IterativeMinMaxLinearEquationSolver(storm::storage::SparseMatrix<ValueType> const& A, std::unique_ptr<LinearEquationSolverFactory<ValueType>>&& linearEquationSolverFactory);
            IterativeMinMaxLinearEquationSolver(storm::storage::SparseMatrix<ValueType>&& A, std::unique_ptr<LinearEquationSolverFactory<ValueType>>&& linearEquationSolverFactory);
            
            virtual bool internalSolveEquations(Environment const& env, OptimizationDirection dir, std::vector<ValueType>& x, std::vector<ValueType> const& b) const override;

            virtual void clearCache() const override;
            
            virtual MinMaxLinearEquationSolverRequirements getRequirements(Environment const& env, boost::optional<storm::solver::OptimizationDirection> const& direction = boost::none, bool const& hasInitialScheduler = false) const override;
            
        private:
            
            MinMaxMethod getMethod(Environment const& env, bool isExactMode) const;
            
            bool solveInducedEquationSystem(Environment const& env, std::unique_ptr<LinearEquationSolver<ValueType>>& linearEquationSolver, std::vector<uint64_t> const& scheduler, std::vector<ValueType>& x, std::vector<ValueType>& subB, std::vector<ValueType> const& originalB) const;
            bool solveEquationsPolicyIteration(Environment const& env, OptimizationDirection dir, std::vector<ValueType>& x, std::vector<ValueType> const& b) const;
            bool performPolicyIteration(Environment const& env, OptimizationDirection dir, std::vector<ValueType>& x, std::vector<ValueType> const& b, std::vector<storm::storage::sparse::state_type>&& initialPolicy) const;
            bool valueImproved(OptimizationDirection dir, ValueType const& value1, ValueType const& value2) const;

            bool solveEquationsValueIteration(Environment const& env, OptimizationDirection dir, std::vector<ValueType>& x, std::vector<ValueType> const& b) const;
            bool solveEquationsOptimisticValueIteration(Environment const& env, OptimizationDirection dir, std::vector<ValueType>& x, std::vector<ValueType> const& b) const;
            bool solveEquationsIntervalIteration(Environment const& env, OptimizationDirection dir, std::vector<ValueType>& x, std::vector<ValueType> const& b) const;
            bool solveEquationsSoundValueIteration(Environment const& env, OptimizationDirection dir, std::vector<ValueType>& x, std::vector<ValueType> const& b) const;
            bool solveEquationsViToPi(Environment const& env, OptimizationDirection dir, std::vector<ValueType>& x, std::vector<ValueType> const& b) const;

            bool solveEquationsRationalSearch(Environment const& env, OptimizationDirection dir, std::vector<ValueType>& x, std::vector<ValueType> const& b) const;
            
            template<typename RationalType, typename ImpreciseType>
            bool solveEquationsRationalSearchHelper(Environment const& env, OptimizationDirection dir, IterativeMinMaxLinearEquationSolver<ImpreciseType> const& impreciseSolver, storm::storage::SparseMatrix<RationalType> const& rationalA, std::vector<RationalType>& rationalX, std::vector<RationalType> const& rationalB, storm::storage::SparseMatrix<ImpreciseType> const& A, std::vector<ImpreciseType>& x, std::vector<ImpreciseType> const& b, std::vector<ImpreciseType>& tmpX) const;
            template<typename ImpreciseType>
            typename std::enable_if<std::is_same<ValueType, ImpreciseType>::value && !NumberTraits<ValueType>::IsExact, bool>::type solveEquationsRationalSearchHelper(Environment const& env, OptimizationDirection dir, std::vector<ValueType>& x, std::vector<ValueType> const& b) const;
            template<typename ImpreciseType>
            typename std::enable_if<std::is_same<ValueType, ImpreciseType>::value && NumberTraits<ValueType>::IsExact, bool>::type solveEquationsRationalSearchHelper(Environment const& env, OptimizationDirection dir, std::vector<ValueType>& x, std::vector<ValueType> const& b) const;
            template<typename ImpreciseType>
            typename std::enable_if<!std::is_same<ValueType, ImpreciseType>::value, bool>::type solveEquationsRationalSearchHelper(Environment const& env, OptimizationDirection dir, std::vector<ValueType>& x, std::vector<ValueType> const& b) const;
            template<typename RationalType, typename ImpreciseType>
            static bool sharpen(storm::OptimizationDirection dir, uint64_t precision, storm::storage::SparseMatrix<RationalType> const& A, std::vector<ImpreciseType> const& x, std::vector<RationalType> const& b, std::vector<RationalType>& tmp);
            static bool isSolution(storm::OptimizationDirection dir, storm::storage::SparseMatrix<ValueType> const& matrix, std::vector<ValueType> const& values, std::vector<ValueType> const& b);

            void computeOptimalValueForRowGroup(uint_fast64_t group, OptimizationDirection dir, std::vector<ValueType>& x, std::vector<ValueType> const& b, uint_fast64_t* choice = nullptr) const;
                        
            struct ValueIterationResult {
                ValueIterationResult(uint64_t iterations, SolverStatus status) : iterations(iterations), status(status) {
                    // Intentionally left empty.
                }
                
                uint64_t iterations;
                SolverStatus status;
            };
            
            template <typename ValueTypePrime>
            friend class IterativeMinMaxLinearEquationSolver;
            
            ValueIterationResult performValueIteration(Environment const& env, OptimizationDirection dir, std::vector<ValueType>*& currentX, std::vector<ValueType>*& newX, std::vector<ValueType> const& b, ValueType const& precision, bool relative, SolverGuarantee const& guarantee, uint64_t currentIterations, uint64_t  maximalNumberOfIterations, storm::solver::MultiplicationStyle const& multiplicationStyle) const;
            
            void createLinearEquationSolver(Environment const& env) const;
            
            /// The factory used to obtain linear equation solvers.
            std::unique_ptr<LinearEquationSolverFactory<ValueType>> linearEquationSolverFactory;
            
            // possibly cached data
            mutable std::unique_ptr<storm::solver::Multiplier<ValueType>> multiplierA;
            mutable std::unique_ptr<std::vector<ValueType>> auxiliaryRowGroupVector; // A.rowGroupCount() entries
            mutable std::unique_ptr<std::vector<ValueType>> auxiliaryRowGroupVector2; // A.rowGroupCount() entries
            mutable std::unique_ptr<storm::solver::helper::SoundValueIterationHelper<ValueType>> soundValueIterationHelper;
            
        };
        
    }
}
