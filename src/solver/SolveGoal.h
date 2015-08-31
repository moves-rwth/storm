
#ifndef SOLVEGOAL_H
#define	SOLVEGOAL_H

#include "src/solver/OptimizationDirection.h"
#include "src/logic/ComparisonType.h"
#include "src/logic/BoundInfo.h"
#include "src/storage/BitVector.h"

namespace storm {
    namespace storage {
        template<typename VT> class SparseMatrix;
    }
    namespace utility {
        namespace solver {
            template<typename VT> class MinMaxLinearEquationSolverFactory;
        }
    }
    
    
    namespace solver {
        template<typename VT> class MinMaxLinearEquationSolver;
        
        class SolveGoal {
        public:
            SolveGoal(bool minimize) : optDirection(minimize ? OptimizationDirection::Minimize : OptimizationDirection::Maximize) {}
            SolveGoal(OptimizationDirection d) : optDirection(d) {}
            virtual ~SolveGoal() {}
           
            bool minimize() const { return optDirection == OptimizationDirection::Minimize; }
            OptimizationDirection direction() const { return optDirection; }
            virtual bool isBounded() const { return false; }
           
        private:
            OptimizationDirection optDirection;
           
        };
        
        
        template<typename VT>
        class BoundedGoal : public SolveGoal {
        public:
            BoundedGoal(OptimizationDirection dir, storm::logic::ComparisonType ct, VT const& threshold, storm::storage::BitVector const& relColumns) : SolveGoal(dir), boundType(ct), threshold(threshold), relevantColumnVector(relColumns) {}
            BoundedGoal(OptimizationDirection dir, storm::logic::BoundInfo<VT> const& bi, storm::storage::BitVector const& relColumns) : SolveGoal(dir), boundType(bi.boundType), threshold(bi.bound), relevantColumnVector(relColumns) {}
            virtual ~BoundedGoal() {}
            
            bool isBounded() const override { return true; }
            
            bool boundIsALowerBound() const { 
                return (boundType == storm::logic::ComparisonType::Greater |
                        boundType == storm::logic::ComparisonType::GreaterEqual);                              ;
            }
            VT thresholdValue() const { return threshold; }
            storm::storage::BitVector relevantColumns() const { return relevantColumnVector; }
            
            storm::logic::ComparisonType boundType;
            VT threshold;
            storm::storage::BitVector relevantColumnVector;
        };
        template<typename VT>
        std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<VT>> configureMinMaxLinearEquationSolver(BoundedGoal<VT> const& goal, storm::utility::solver::MinMaxLinearEquationSolverFactory<VT> const& factory, storm::storage::SparseMatrix<VT> const&  matrix);
        template<typename VT> 
        std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<VT>> configureMinMaxLinearEquationSolver(SolveGoal const& goal, storm::utility::solver::MinMaxLinearEquationSolverFactory<VT> const& factory, storm::storage::SparseMatrix<VT> const&  matrix);
        
       
    }
}


#endif	/* SOLVEGOAL_H */

