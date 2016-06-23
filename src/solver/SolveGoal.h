#ifndef STORM_SOLVER_SOLVEGOAL_H_
#define	STORM_SOLVER_SOLVEGOAL_H_

#include <memory>

#include "src/solver/OptimizationDirection.h"
#include "src/logic/ComparisonType.h"
#include "src/logic/Bound.h"
#include "src/storage/BitVector.h"

#include "src/solver/LinearEquationSolver.h"

namespace storm {
    namespace storage {
        template<typename ValueType> class SparseMatrix;
    }
    
    namespace utility {
        namespace solver {
            template<typename ValueType> class MinMaxLinearEquationSolverFactory;
            template<typename ValueType> class LinearEquationSolverFactory;
        }
    }
    
    namespace solver {
        template<typename ValueType> class MinMaxLinearEquationSolver;
        template<typename ValueType> class LinearEquationSolver;
        
        class SolveGoal {
        public:
            SolveGoal(bool minimize) : optimizationDirection(minimize ? OptimizationDirection::Minimize : OptimizationDirection::Maximize) {
                // Intentionally left empty.
            }
            
            SolveGoal(OptimizationDirection d) : optimizationDirection(d) {
                // Intentionally left empty.
            }
            
            virtual ~SolveGoal() {
                // Intentionally left empty.
            }
           
            bool minimize() const {
                return optimizationDirection == OptimizationDirection::Minimize;
            }
            
            OptimizationDirection direction() const {
                return optimizationDirection;
            }
            
            virtual bool isBounded() const {
                return false;
            }
           
        private:
            OptimizationDirection optimizationDirection;
        };
        
        template<typename ValueType>
        class BoundedGoal : public SolveGoal {
        public:
            BoundedGoal(OptimizationDirection optimizationDirection, storm::logic::ComparisonType comparisonType, ValueType const& threshold, storm::storage::BitVector const& relevantValues) : SolveGoal(optimizationDirection), bound(comparisonType, threshold), relevantValueVector(relevantValues) {
                // Intentionally left empty.
            }
            
            BoundedGoal(OptimizationDirection optimizationDirection, storm::logic::Bound<ValueType> const& bound, storm::storage::BitVector const& relevantValues) : SolveGoal(optimizationDirection), bound(bound), relevantValueVector(relevantValues) {
                // Intentionally left empty.
            }
            
            virtual ~BoundedGoal() {
                // Intentionally left empty.
            }
            
            bool isBounded() const override {
                return true;
            }
            
            bool boundIsALowerBound() const { 
                return (bound.comparisonType == storm::logic::ComparisonType::Greater || bound.comparisonType == storm::logic::ComparisonType::GreaterEqual);
            }
            
            bool boundIsStrict() const {
                return (bound.comparisonType == storm::logic::ComparisonType::Greater || bound.comparisonType == storm::logic::ComparisonType::Less);
            }
            
            ValueType const& thresholdValue() const {
                return bound.threshold;
            }
            
            storm::storage::BitVector const& relevantValues() const {
                return relevantValueVector;
            }
            
        private:
            Bound<ValueType> bound;
            storm::storage::BitVector relevantValueVector;
        };
        
        template<typename ValueType>
        std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<ValueType>> configureMinMaxLinearEquationSolver(BoundedGoal<ValueType> const& goal, storm::utility::solver::MinMaxLinearEquationSolverFactory<ValueType> const& factory, storm::storage::SparseMatrix<ValueType> const& matrix);
        
        template<typename ValueType> 
        std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<ValueType>> configureMinMaxLinearEquationSolver(SolveGoal const& goal, storm::utility::solver::MinMaxLinearEquationSolverFactory<ValueType> const& factory, storm::storage::SparseMatrix<ValueType> const& matrix);

        template<typename ValueType>
        std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> configureLinearEquationSolver(BoundedGoal<ValueType> const& goal, storm::solver::LinearEquationSolverFactory<ValueType> const& factory, storm::storage::SparseMatrix<ValueType> const& matrix);
        
        template<typename ValueType>
        std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> configureLinearEquationSolver(SolveGoal const& goal, storm::solver::LinearEquationSolverFactory<ValueType> const& factory, storm::storage::SparseMatrix<ValueType> const& matrix);

    }
}


#endif	/* STORM_SOLVER_SOLVEGOAL_H_ */

