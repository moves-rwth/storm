#ifndef STORM_SOLVER_SOLVEGOAL_H_
#define	STORM_SOLVER_SOLVEGOAL_H_

#include <memory>

#include "src/solver/OptimizationDirection.h"
#include "src/logic/ComparisonType.h"
#include "src/logic/BoundInfo.h"
#include "src/storage/BitVector.h"

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
            BoundedGoal(OptimizationDirection optimizationDirection, storm::logic::ComparisonType ct, ValueType const& threshold, storm::storage::BitVector const& relColumns) : SolveGoal(optimizationDirection), boundType(ct), threshold(threshold), relevantColumnVector(relColumns) {
                // Intentionally left empty.
            }
            
            BoundedGoal(OptimizationDirection optimizationDirection, storm::logic::BoundInfo<ValueType> const& boundInfo, storm::storage::BitVector const& relevantColumns) : SolveGoal(optimizationDirection), boundType(boundInfo.boundType), threshold(boundInfo.bound), relevantColumnVector(relevantColumns) {
                // Intentionally left empty.
            }
            
            virtual ~BoundedGoal() {
                // Intentionally left empty.
            }
            
            bool isBounded() const override {
                return true;
            }
            
            bool boundIsALowerBound() const { 
                return (boundType == storm::logic::ComparisonType::Greater || boundType == storm::logic::ComparisonType::GreaterEqual);
            }
            
            ValueType const& thresholdValue() const {
                return threshold;
            }
            
            storm::storage::BitVector const& relevantColumns() const {
                return relevantColumnVector;
            }
            
        private:
            storm::logic::ComparisonType boundType;
            ValueType threshold;
            storm::storage::BitVector relevantColumnVector;
        };
        
        template<typename ValueType>
        std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<ValueType>> configureMinMaxLinearEquationSolver(BoundedGoal<ValueType> const& goal, storm::utility::solver::MinMaxLinearEquationSolverFactory<ValueType> const& factory, storm::storage::SparseMatrix<ValueType> const& matrix);
        
        template<typename ValueType> 
        std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<ValueType>> configureMinMaxLinearEquationSolver(SolveGoal const& goal, storm::utility::solver::MinMaxLinearEquationSolverFactory<ValueType> const& factory, storm::storage::SparseMatrix<ValueType> const& matrix);

        template<typename ValueType>
        std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> configureLinearEquationSolver(BoundedGoal<ValueType> const& goal, storm::utility::solver::LinearEquationSolverFactory<ValueType> const& factory, storm::storage::SparseMatrix<ValueType> const& matrix);
        
        template<typename ValueType>
        std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> configureLinearEquationSolver(SolveGoal const& goal, storm::utility::solver::LinearEquationSolverFactory<ValueType> const& factory, storm::storage::SparseMatrix<ValueType> const& matrix);

    }
}


#endif	/* STORM_SOLVER_SOLVEGOAL_H_ */

