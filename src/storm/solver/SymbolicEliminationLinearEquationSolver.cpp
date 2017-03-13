#include "storm/solver/SymbolicEliminationLinearEquationSolver.h"

#include "storm/storage/dd/DdManager.h"
#include "storm/storage/dd/Add.h"

#include "storm/utility/dd.h"

namespace storm {
    namespace solver {
     
        template<storm::dd::DdType DdType, typename ValueType>
        SymbolicEliminationLinearEquationSolver<DdType, ValueType>::SymbolicEliminationLinearEquationSolver(storm::dd::Add<DdType, ValueType> const& A, storm::dd::Bdd<DdType> const& allRows, std::set<storm::expressions::Variable> const& rowMetaVariables, std::set<storm::expressions::Variable> const& columnMetaVariables, std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& rowColumnMetaVariablePairs) : SymbolicLinearEquationSolver<DdType, ValueType>(A, allRows, rowMetaVariables, columnMetaVariables, rowColumnMetaVariablePairs) {
            // Intentionally left empty.
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        SymbolicEliminationLinearEquationSolver<DdType, ValueType>::SymbolicEliminationLinearEquationSolver(storm::dd::Add<DdType, ValueType> const& A, storm::dd::Bdd<DdType> const& allRows, std::set<storm::expressions::Variable> const& rowMetaVariables, std::set<storm::expressions::Variable> const& columnMetaVariables, std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& rowColumnMetaVariablePairs, double precision, uint_fast64_t maximalNumberOfIterations, bool relative) : SymbolicLinearEquationSolver<DdType, ValueType>(A, allRows, rowMetaVariables, columnMetaVariables, rowColumnMetaVariablePairs, precision, maximalNumberOfIterations, relative) {
            // Intentionally left empty.
        }
        
        template<storm::dd::DdType DdType, typename ValueType>
        storm::dd::Add<DdType, ValueType> SymbolicEliminationLinearEquationSolver<DdType, ValueType>::solveEquations(storm::dd::Add<DdType, ValueType> const& x, storm::dd::Add<DdType, ValueType> const& b) const {
            storm::dd::Bdd<DdType> diagonal = storm::utility::dd::getRowColumnDiagonal(x.getDdManager(), this->rowColumnMetaVariablePairs);
            diagonal &= this->allRows;
            
            storm::dd::Add<DdType, ValueType> rowsAdd = this->allRows.template toAdd<ValueType>();
            storm::dd::Add<DdType, ValueType> diagonalAdd = diagonal.template toAdd<ValueType>();
            
            // Revert the conversion to an equation system.
            storm::dd::Add<DdType, ValueType> matrix = diagonalAdd - this->A;
            
            storm::dd::Add<DdType, ValueType> solution = b;
            
            // As long as there are transitions, we eliminate them.
            while (!matrix.isZero()) {
                // Determine inverse loop probabilies
                storm::dd::Add<DdType, ValueType> inverseLoopProbabilities = rowsAdd / (rowsAdd - (diagonalAdd * matrix).sumAbstract(this->columnMetaVariables));
                
                inverseLoopProbabilities.swapVariables(this->rowColumnMetaVariablePairs);
                
                // Scale all transitions with the inverse loop probabilities.
                matrix *= inverseLoopProbabilities;
                
                // Delete diagonal elements, i.e. remove self-loops.
                matrix = diagonal.ite(x.getDdManager().template getAddZero<ValueType>(), matrix);
            
                // Update the one-step probabilities.
                solution += (matrix * solution.swapVariables(this->rowColumnMetaVariablePairs)).sumAbstract(this->columnMetaVariables);
                
                // Now eliminate all direct transitions of all states.
                storm::dd::Add<DdType, ValueType> matrixWithRemoved;
            }
            
            std::cout << "here" << std::endl;
            solution.exportToDot("solution.dot");
            
            exit(-1);
        }

        template class SymbolicEliminationLinearEquationSolver<storm::dd::DdType::CUDD, double>;
        template class SymbolicEliminationLinearEquationSolver<storm::dd::DdType::Sylvan, double>;
        
    }
}
