#ifndef STORM_SOLVER_SYMBOLICMINMAXLINEAREQUATIONSOLVER_H_
#define STORM_SOLVER_SYMBOLICMINMAXLINEAREQUATIONSOLVER_H_

#include <memory>
#include <set>
#include <vector>
#include <boost/variant.hpp>

#include "src/storage/expressions/Variable.h"
#include "src/storage/dd/DdType.h"

namespace storm {
    namespace dd {
        template<storm::dd::DdType Type, typename ValueType>
        class Add;
        
        template<storm::dd::DdType T>
        class Bdd;
    }
    
    namespace solver {
        /*!
         * An interface that represents an abstract symbolic linear equation solver. In addition to solving a system of
         * linear equations, the functionality to repeatedly multiply a matrix with a given vector is provided.
         */
        template<storm::dd::DdType DdType, typename ValueType>
        class SymbolicMinMaxLinearEquationSolver {
        public:
            /*!
             * Constructs a symbolic min/max linear equation solver with the given meta variable sets and pairs.
             *
             * @param A The matrix defining the coefficients of the linear equation system.
             * @param diagonal An ADD characterizing the elements on the diagonal of the matrix.
             * @param allRows A BDD characterizing all rows of the equation system.
             * @param illegalMask A mask that characterizes all illegal choices (that are therefore not to be taken).
             * @param rowMetaVariables The meta variables used to encode the rows of the matrix.
             * @param columnMetaVariables The meta variables used to encode the columns of the matrix.
             * @param choiceVariables The variables encoding the choices of each row group.
             * @param rowColumnMetaVariablePairs The pairs of row meta variables and the corresponding column meta
             * variables.
             */
            SymbolicMinMaxLinearEquationSolver(storm::dd::Add<DdType, ValueType> const& A, storm::dd::Bdd<DdType> const& allRows, storm::dd::Bdd<DdType> const& illegalMask, std::set<storm::expressions::Variable> const& rowMetaVariables, std::set<storm::expressions::Variable> const& columnMetaVariables, std::set<storm::expressions::Variable> const& choiceVariables, std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& rowColumnMetaVariablePairs);
            
            /*!
             * Constructs a symbolic linear equation solver with the given meta variable sets and pairs.
             *
             * @param A The matrix defining the coefficients of the linear equation system.
             * @param allRows A BDD characterizing all rows of the equation system.
             * @param illegalMask A mask that characterizes all illegal choices (that are therefore not to be taken).
             * @param rowMetaVariables The meta variables used to encode the rows of the matrix.
             * @param columnMetaVariables The meta variables used to encode the columns of the matrix.
             * @param choiceVariables The variables encoding the choices of each row group.
             * @param rowColumnMetaVariablePairs The pairs of row meta variables and the corresponding column meta
             * variables.
             * @param precision The precision to achieve.
             * @param maximalNumberOfIterations The maximal number of iterations to perform when solving a linear
             * equation system iteratively.
             * @param relative Sets whether or not to use a relativ stopping criterion rather than an absolute one.
             */
            SymbolicMinMaxLinearEquationSolver(storm::dd::Add<DdType, ValueType> const& A, storm::dd::Bdd<DdType> const& allRows, storm::dd::Bdd<DdType> const& illegalMask, std::set<storm::expressions::Variable> const& rowMetaVariables, std::set<storm::expressions::Variable> const& columnMetaVariables, std::set<storm::expressions::Variable> const& choiceVariables, std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& rowColumnMetaVariablePairs, double precision, uint_fast64_t maximalNumberOfIterations, bool relative);
            
            /*!
             * Solves the equation system A*x = b. The matrix A is required to be square and have a unique solution.
             * The solution of the set of linear equations will be written to the vector x. Note that the matrix A has
             * to be given upon construction time of the solver object.
             *
             * @param minimize If set, all the value of a group of rows is the taken as the minimum over all rows and as
             * the maximum otherwise.
             * @param x The initual guess for the solution vector. Its length must be equal to the number of row
             * groups of A.
             * @param b The right-hand side of the equation system. Its length must be equal to the number of row groups
             * of A.
             * @return The solution of the equation system.
             */
            virtual storm::dd::Add<DdType, ValueType> solveEquations(bool minimize, storm::dd::Add<DdType, ValueType> const& x, storm::dd::Add<DdType, ValueType> const& b) const;
            
            /*!
             * Performs repeated matrix-vector multiplication, using x[0] = x and x[i + 1] = A*x[i] + b. After
             * performing the necessary multiplications, the result is written to the input vector x. Note that the
             * matrix A has to be given upon construction time of the solver object.
             *
             * @param minimize If set, all the value of a group of rows is the taken as the minimum over all rows and as
             * the maximum otherwise.
             * @param x The initial vector with which to perform matrix-vector multiplication. Its length must be equal
             * to the number of row groups of A.
             * @param b If non-null, this vector is added after each multiplication. If given, its length must be equal
             * to the number of row groups of A.
             * @return The solution of the equation system.
             */
            virtual storm::dd::Add<DdType, ValueType> multiply(bool minimize, storm::dd::Add<DdType, ValueType> const& x, storm::dd::Add<DdType, ValueType> const* b = nullptr, uint_fast64_t n = 1) const;
            
        protected:
            // The matrix defining the coefficients of the linear equation system.
            storm::dd::Add<DdType, ValueType> A;
            
            // A BDD characterizing all rows of the equation system.
            storm::dd::Bdd<DdType> allRows;
            
            // An ADD characterizing the illegal choices.
            storm::dd::Add<DdType, ValueType> illegalMaskAdd;
            
            // The row variables.
            std::set<storm::expressions::Variable> rowMetaVariables;
            
            // The column variables.
            std::set<storm::expressions::Variable> columnMetaVariables;
            
            // The choice variables
            std::set<storm::expressions::Variable> choiceVariables;
            
            // The pairs of meta variables used for renaming.
            std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> rowColumnMetaVariablePairs;
            
            // The precision to achive.
            double precision;
            
            // The maximal number of iterations to perform.
            uint_fast64_t maximalNumberOfIterations;
            
            // A flag indicating whether a relative or an absolute stopping criterion is to be used.
            bool relative;
        };
        
    } // namespace solver
} // namespace storm

#endif /* STORM_SOLVER_SYMBOLICMINMAXLINEAREQUATIONSOLVER_H_ */
