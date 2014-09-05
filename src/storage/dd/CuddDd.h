#ifndef STORM_STORAGE_DD_CUDDDD_H_
#define STORM_STORAGE_DD_CUDDDD_H_

#include <map>
#include <set>
#include <memory>
#include <iostream>

#include "src/storage/dd/Dd.h"
#include "src/storage/dd/CuddDdForwardIterator.h"
#include "src/storage/SparseMatrix.h"
#include "src/storage/expressions/Expression.h"
#include "src/utility/OsDetection.h"

// Include the C++-interface of CUDD.
#include "cuddObj.hh"

namespace storm {
    namespace dd {
        // Forward-declare some classes.
        template<DdType Type> class DdManager;
        template<DdType Type> class Odd;
        
        template<>
        class Dd<DdType::CUDD> {
        public:
            // Declare the DdManager and DdIterator class as friend so it can access the internals of a DD.
            friend class DdManager<DdType::CUDD>;
            friend class DdForwardIterator<DdType::CUDD>;
            friend class Odd<DdType::CUDD>;
            
            // Instantiate all copy/move constructors/assignments with the default implementation.
            Dd() = default;
            Dd(Dd<DdType::CUDD> const& other) = default;
			Dd& operator=(Dd<DdType::CUDD> const& other) = default;
#ifndef WINDOWS
            Dd(Dd<DdType::CUDD>&& other) = default;
            Dd& operator=(Dd<DdType::CUDD>&& other) = default;
#endif
            
            /*!
             * Retrieves whether the two DDs represent the same function.
             *
             * @param other The DD that is to be compared with the current one.
             * @return True if the DDs represent the same function.
             */
            bool operator==(Dd<DdType::CUDD> const& other) const;
            
            /*!
             * Retrieves whether the two DDs represent different functions.
             *
             * @param other The DD that is to be compared with the current one.
             * @return True if the DDs represent the different functions.
             */
            bool operator!=(Dd<DdType::CUDD> const& other) const;
            
            /*!
             * Performs an if-then-else with the given operands, i.e. maps all valuations that are mapped to a non-zero
             * function value to the function values specified by the first DD and all others to the function values
             * specified by the second DD.
             */
            Dd<DdType::CUDD> ite(Dd<DdType::CUDD> const& thenDd, Dd<DdType::CUDD> const& elseDd) const;
            
            /*!
             * Performs a logical or of the current and the given DD.
             *
             * @return The logical or of the operands.
             */
            Dd<DdType::CUDD> operator||(Dd<DdType::CUDD> const& other) const;
            
            /*!
             * Performs a logical and of the current and the given DD.
             *
             * @return The logical and of the operands.
             */
            Dd<DdType::CUDD> operator&&(Dd<DdType::CUDD> const& other) const;
            
            /*!
             * Adds the two DDs.
             *
             * @param other The DD to add to the current one.
             * @return The result of the addition.
             */
            Dd<DdType::CUDD> operator+(Dd<DdType::CUDD> const& other) const;
            
            /*!
             * Adds the given DD to the current one.
             *
             * @param other The DD to add to the current one.
             * @return A reference to the current DD after the operation.
             */
            Dd<DdType::CUDD>& operator+=(Dd<DdType::CUDD> const& other);
            
            /*!
             * Multiplies the two DDs.
             *
             * @param other The DD to multiply with the current one.
             * @return The result of the multiplication.
             */
            Dd<DdType::CUDD> operator*(Dd<DdType::CUDD> const& other) const;
            
            /*!
             * Multiplies the given DD with the current one and assigns the result to the current DD.
             *
             * @param other The DD to multiply with the current one.
             * @return A reference to the current DD after the operation.
             */
            Dd<DdType::CUDD>& operator*=(Dd<DdType::CUDD> const& other);
            
            /*!
             * Subtracts the given DD from the current one.
             *
             * @param other The DD to subtract from the current one.
             * @return The result of the subtraction.
             */
            Dd<DdType::CUDD> operator-(Dd<DdType::CUDD> const& other) const;
            
            /*!
             * Subtracts the DD from the constant zero function.
             *
             * @return The resulting function represented as a DD.
             */
            Dd<DdType::CUDD> operator-() const;
            
            /*!
             * Subtracts the given DD from the current one and assigns the result to the current DD.
             *
             * @param other The DD to subtract from the current one.
             * @return A reference to the current DD after the operation.
             */
            Dd<DdType::CUDD>& operator-=(Dd<DdType::CUDD> const& other);
            
            /*!
             * Divides the current DD by the given one.
             *
             * @param other The DD by which to divide the current one.
             * @return The result of the division.
             */
            Dd<DdType::CUDD> operator/(Dd<DdType::CUDD> const& other) const;
            
            /*!
             * Divides the current DD by the given one and assigns the result to the current DD.
             *
             * @param other The DD by which to divide the current one.
             * @return A reference to the current DD after the operation.
             */
            Dd<DdType::CUDD>& operator/=(Dd<DdType::CUDD> const& other);
            
            /*!
             * Retrieves the logical complement of the current DD. The result will map all encodings with a value
             * unequal to zero to false and all others to true.
             *
             * @return The logical complement of the current DD.
             */
            Dd<DdType::CUDD> operator!() const;
            
            /*!
             * Logically complements the current DD. The result will map all encodings with a value
             * unequal to zero to false and all others to true.
             *
             * @return A reference to the current DD after the operation.
             */
            Dd<DdType::CUDD>& complement();
            
            /*!
             * Retrieves the function that maps all evaluations to one that have an identical function values.
             *
             * @param other The DD with which to perform the operation.
             * @return The resulting function represented as a DD.
             */
            Dd<DdType::CUDD> equals(Dd<DdType::CUDD> const& other) const;
            
            /*!
             * Retrieves the function that maps all evaluations to one that have distinct function values.
             *
             * @param other The DD with which to perform the operation.
             * @return The resulting function represented as a DD.
             */
            Dd<DdType::CUDD> notEquals(Dd<DdType::CUDD> const& other) const;
            
            /*!
             * Retrieves the function that maps all evaluations to one whose function value in the first DD are less
             * than the one in the given DD.
             *
             * @param other The DD with which to perform the operation.
             * @return The resulting function represented as a DD.
             */
            Dd<DdType::CUDD> less(Dd<DdType::CUDD> const& other) const;
            
            /*!
             * Retrieves the function that maps all evaluations to one whose function value in the first DD are less or
             * equal than the one in the given DD.
             *
             * @param other The DD with which to perform the operation.
             * @return The resulting function represented as a DD.
             */
            Dd<DdType::CUDD> lessOrEqual(Dd<DdType::CUDD> const& other) const;
            
            /*!
             * Retrieves the function that maps all evaluations to one whose function value in the first DD are greater
             * than the one in the given DD.
             *
             * @param other The DD with which to perform the operation.
             * @return The resulting function represented as a DD.
             */
            Dd<DdType::CUDD> greater(Dd<DdType::CUDD> const& other) const;
            
            /*!
             * Retrieves the function that maps all evaluations to one whose function value in the first DD are greater
             * or equal than the one in the given DD.
             *
             * @param other The DD with which to perform the operation.
             * @return The resulting function represented as a DD.
             */
            Dd<DdType::CUDD> greaterOrEqual(Dd<DdType::CUDD> const& other) const;
            
            /*!
             * Retrieves the function that maps all evaluations to the minimum of the function values of the two DDs.
             *
             * @param other The DD with which to perform the operation.
             * @return The resulting function represented as a DD.
             */
            Dd<DdType::CUDD> minimum(Dd<DdType::CUDD> const& other) const;

            /*!
             * Retrieves the function that maps all evaluations to the maximum of the function values of the two DDs.
             *
             * @param other The DD with which to perform the operation.
             * @return The resulting function represented as a DD.
             */
            Dd<DdType::CUDD> maximum(Dd<DdType::CUDD> const& other) const;

            /*!
             * Existentially abstracts from the given meta variables.
             *
             * @param metaVariableNames The names of all meta variables from which to abstract.
             */
            Dd<DdType::CUDD> existsAbstract(std::set<std::string> const& metaVariableNames) const;
            
            /*!
             * Universally abstracts from the given meta variables.
             *
             * @param metaVariableNames The names of all meta variables from which to abstract.
             */
            Dd<DdType::CUDD> universalAbstract(std::set<std::string> const& metaVariableNames) const;
            
            /*!
             * Sum-abstracts from the given meta variables.
             *
             * @param metaVariableNames The names of all meta variables from which to abstract.
             */
            Dd<DdType::CUDD> sumAbstract(std::set<std::string> const& metaVariableNames) const;
            
            /*!
             * Min-abstracts from the given meta variables.
             *
             * @param metaVariableNames The names of all meta variables from which to abstract.
             */
            Dd<DdType::CUDD> minAbstract(std::set<std::string> const& metaVariableNames) const;
            
            /*!
             * Max-abstracts from the given meta variables.
             *
             * @param metaVariableNames The names of all meta variables from which to abstract.
             */
            Dd<DdType::CUDD> maxAbstract(std::set<std::string> const& metaVariableNames) const;
            
            /*!
             * Checks whether the current and the given DD represent the same function modulo some given precision.
             *
             * @param other The DD with which to compare.
             * @param precision An upper bound on the maximal difference between any two function values that is to be
             * tolerated.
             * @param relative If set to true, not the absolute values have to be within the precision, but the relative
             * values.
             */
            bool equalModuloPrecision(Dd<DdType::CUDD> const& other, double precision, bool relative = true) const;
            
            /*!
             * Swaps the given pairs of meta variables in the DD. The pairs of meta variables must be guaranteed to have
             * the same number of underlying DD variables.
             *
             * @param metaVariablePairs A vector of meta variable pairs that are to be swapped for one another.
             */
            void swapVariables(std::vector<std::pair<std::string, std::string>> const& metaVariablePairs);
            
            /*!
             * Multiplies the current DD (representing a matrix) with the given matrix by summing over the given meta
             * variables.
             *
             * @param otherMatrix The matrix with which to multiply.
             * @param summationMetaVariableNames The names of the meta variables over which to sum during the matrix-
             * matrix multiplication.
             * @return A DD representing the result of the matrix-matrix multiplication.
             */
            Dd<DdType::CUDD> multiplyMatrix(Dd<DdType::CUDD> const& otherMatrix, std::set<std::string> const& summationMetaVariableNames) const;
            
            /*!
             * Computes a DD that represents the function in which all assignments with a function value strictly larger
             * than the given value are mapped to one and all others to zero.
             *
             * @param value The value used for the comparison.
             * @return The resulting DD.
             */
            Dd<DdType::CUDD> greater(double value) const;

            /*!
             * Computes a DD that represents the function in which all assignments with a function value larger or equal
             * to the given value are mapped to one and all others to zero.
             *
             * @param value The value used for the comparison.
             * @return The resulting DD.
             */
            Dd<DdType::CUDD> greaterOrEqual(double value) const;
            
            /*!
             * Computes a DD that represents the function in which all assignments with a function value unequal to zero
             * are mapped to one and all others to zero.
             *
             * @return The resulting DD.
             */
            Dd<DdType::CUDD> notZero() const;
            
            /*!
             * Computes the constraint of the current DD with the given constraint. That is, the function value of the
             * resulting DD will be the same as the current ones for all assignments mapping to one in the constraint
             * and may be different otherwise.
             *
             * @param constraint The constraint to use for the operation.
             * @return The resulting DD.
             */
            Dd<DdType::CUDD> constrain(Dd<DdType::CUDD> const& constraint) const;
            
            /*!
             * Computes the restriction of the current DD with the given constraint. That is, the function value of the
             * resulting DD will be the same as the current ones for all assignments mapping to one in the constraint
             * and may be different otherwise.
             *
             * @param constraint The constraint to use for the operation.
             * @return The resulting DD.
             */
            Dd<DdType::CUDD> restrict(Dd<DdType::CUDD> const& constraint) const;
            
            /*!
             * Retrieves the support of the current DD.
             *
             * @return The support represented as a DD.
             */
            Dd<DdType::CUDD> getSupport() const;
            
            /*!
             * Retrieves the number of encodings that are mapped to a non-zero value.
             *
             * @return The number of encodings that are mapped to a non-zero value.
             */
            uint_fast64_t getNonZeroCount() const;
            
            /*!
             * Retrieves the number of leaves of the DD.
             *
             * @return The number of leaves of the DD.
             */
            uint_fast64_t getLeafCount() const;
            
            /*!
             * Retrieves the number of nodes necessary to represent the DD.
             *
             * @return The number of nodes in this DD.
             */
            uint_fast64_t getNodeCount() const;
            
            /*!
             * Retrieves the lowest function value of any encoding.
             *
             * @return The lowest function value of any encoding.
             */
            double getMin() const;
            
            /*!
             * Retrieves the highest function value of any encoding.
             *
             * @return The highest function value of any encoding.
             */
            double getMax() const;
            
            /*!
             * Sets the function values of all encodings that have the given value of the meta variable to the given
             * target value.
             *
             * @param metaVariableName The name of the meta variable that has to be equal to the given value.
             * @param variableValue The value that the meta variable is supposed to have. This must be within the range
             * of the meta variable.
             * @param targetValue The new function value of the modified encodings.
             */
            void setValue(std::string const& metaVariableName, int_fast64_t variableValue, double targetValue);
            
            /*!
             * Sets the function values of all encodings that have the given values of the two meta variables to the
             * given target value.
             *
             * @param metaVariableName1 The name of the first meta variable that has to be equal to the first given
             * value.
             * @param variableValue1 The value that the first meta variable is supposed to have. This must be within the
             * range of the meta variable.
             * @param metaVariableName2 The name of the first meta variable that has to be equal to the second given
             * value.
             * @param variableValue2 The value that the second meta variable is supposed to have. This must be within
             * the range of the meta variable.
             * @param targetValue The new function value of the modified encodings.
             */
            void setValue(std::string const& metaVariableName1, int_fast64_t variableValue1, std::string const& metaVariableName2, int_fast64_t variableValue2, double targetValue);
            
            /*!
             * Sets the function values of all encodings that have the given values of the given meta variables to the
             * given target value.
             *
             * @param metaVariableNameToValueMap A mapping of meta variable names to the values they are supposed to
             * have. All values must be within the range of the respective meta variable.
             * @param targetValue The new function value of the modified encodings.
             */
            void setValue(std::map<std::string, int_fast64_t> const& metaVariableNameToValueMap = std::map<std::string, int_fast64_t>(), double targetValue = 0);
            
            /*!
             * Retrieves the value of the function when all meta variables are assigned the values of the given mapping.
             * Note that the mapping must specify values for all meta variables contained in the DD.
             *
             * @param metaVariableNameToValueMap A mapping of meta variable names to their values.
             * @return The value of the function evaluated with the given input.
             */
            double getValue(std::map<std::string, int_fast64_t> const& metaVariableNameToValueMap = std::map<std::string, int_fast64_t>()) const;
            
            /*!
             * Retrieves whether this DD represents the constant one function.
             *
             * @return True if this DD represents the constant one function.
             */
            bool isOne() const;
            
            /*!
             * Retrieves whether this DD represents the constant zero function.
             *
             * @return True if this DD represents the constant zero function.
             */
            bool isZero() const;
            
            /*!
             * Retrieves whether this DD represents a constant function.
             *
             * @return True if this DD represents a constants function.
             */
            bool isConstant() const;
            
            /*!
             * Retrieves the index of the topmost variable in the DD.
             *
             * @return The index of the topmost variable in DD.
             */
            uint_fast64_t getIndex() const;
            
            /*!
             * Converts the DD to a vector.
             *
             * @return The double vector that is represented by this DD.
             */
            template<typename ValueType>
            std::vector<ValueType> toVector() const;

            /*!
             * Converts the DD to a vector. The given offset-labeled DD is used to determine the correct row of
             * each entry.
             *
             * @param rowOdd The ODD used for determining the correct row.
             * @return The double vector that is represented by this DD.
             */
            template<typename ValueType>
            std::vector<ValueType> toVector(storm::dd::Odd<DdType::CUDD> const& rowOdd) const;
            
            /*!
             * Converts the DD to a (sparse) double matrix. All contained non-primed variables are assumed to encode the
             * row, whereas all primed variables are assumed to encode the column.
             *
             * @return The matrix that is represented by this DD.
             */
            storm::storage::SparseMatrix<double> toMatrix() const;

            /*!
             * Converts the DD to a (sparse) double matrix. All contained non-primed variables are assumed to encode the
             * row, whereas all primed variables are assumed to encode the column. The given offset-labeled DDs are used
             * to determine the correct row and column, respectively, for each entry.
             *
             * @param rowOdd The ODD used for determining the correct row.
             * @param columnOdd The ODD used for determining the correct column.
             * @return The matrix that is represented by this DD.
             */
            storm::storage::SparseMatrix<double> toMatrix(storm::dd::Odd<DdType::CUDD> const& rowOdd, storm::dd::Odd<DdType::CUDD> const& columnOdd) const;

            /*!
             * Converts the DD to a (sparse) double matrix. The given offset-labeled DDs are used to determine the
             * correct row and column, respectively, for each entry.
             *
             * @param rowMetaVariables The meta variables that encode the rows of the matrix.
             * @param columnMetaVariables The meta variables that encode the columns of the matrix.
             * @param rowOdd The ODD used for determining the correct row.
             * @param columnOdd The ODD used for determining the correct column.
             * @return The matrix that is represented by this DD.
             */
            storm::storage::SparseMatrix<double> toMatrix(std::set<std::string> const& rowMetaVariables, std::set<std::string> const& columnMetaVariables, storm::dd::Odd<DdType::CUDD> const& rowOdd, storm::dd::Odd<DdType::CUDD> const& columnOdd) const;
            
            /*!
             * Converts the DD to a row-grouped (sparse) double matrix. The given offset-labeled DDs are used to
             * determine the correct row and column, respectively, for each entry. Note: this function assumes that
             * the meta variables used to distinguish different row groups are at the very top of the DD.
             *
             * @param rowMetaVariables The meta variables that encode the rows of the matrix.
             * @param columnMetaVariables The meta variables that encode the columns of the matrix.
             * @param groupMetaVariables The meta variables that are used to distinguish different row groups.
             * @param rowOdd The ODD used for determining the correct row.
             * @param columnOdd The ODD used for determining the correct column.
             * @return The matrix that is represented by this DD.
             */
            storm::storage::SparseMatrix<double> toMatrix(std::set<std::string> const& rowMetaVariables, std::set<std::string> const& columnMetaVariables, std::set<std::string> const& groupMetaVariables, storm::dd::Odd<DdType::CUDD> const& rowOdd, storm::dd::Odd<DdType::CUDD> const& columnOdd) const;
            
            /*!
             * Retrieves whether the given meta variable is contained in the DD.
             *
             * @param metaVariableName The name of the meta variable for which to query membership.
             * @return True iff the meta variable is contained in the DD.
             */
            bool containsMetaVariable(std::string const& metaVariableName) const;
            
            /*!
             * Retrieves whether the given meta variables are all contained in the DD.
             *
             * @param metaVariableNames The names of the meta variable for which to query membership.
             * @return True iff all meta variables are contained in the DD.
             */
            bool containsMetaVariables(std::set<std::string> metaVariableNames) const;
            
            /*!
             * Retrieves the set of all names of meta variables contained in the DD.
             *
             * @return The set of names of all meta variables contained in the DD.
             */
            std::set<std::string> const& getContainedMetaVariableNames() const;
            
            /*!
             * Retrieves the set of all names of meta variables contained in the DD.
             *
             * @return The set of names of all meta variables contained in the DD.
             */
            std::set<std::string>& getContainedMetaVariableNames();
            
            /*!
             * Exports the DD to the given file in the dot format.
             *
             * @param filename The name of the file to which the DD is to be exported.
             */
            void exportToDot(std::string const& filename = "") const;
            
            /*!
             * Retrieves the manager that is responsible for this DD.
             *
             * A pointer to the manager that is responsible for this DD.
             */
            std::shared_ptr<DdManager<DdType::CUDD>> getDdManager() const;
            
            /*!
             * Retrieves an iterator that points to the first meta variable assignment with a non-zero function value.
             *
             * @param enumerateDontCareMetaVariables If set to true, all meta variable assignments are enumerated, even
             * if a meta variable does not at all influence the the function value.
             * @return An iterator that points to the first meta variable assignment with a non-zero function value.
             */
            DdForwardIterator<DdType::CUDD> begin(bool enumerateDontCareMetaVariables = true) const;
            
            /*!
             * Retrieves an iterator that points past the end of the container.
             *
             * @param enumerateDontCareMetaVariables If set to true, all meta variable assignments are enumerated, even
             * if a meta variable does not at all influence the the function value.
             * @return An iterator that points past the end of the container.
             */
            DdForwardIterator<DdType::CUDD> end(bool enumerateDontCareMetaVariables = true) const;
            
            /*!
             * Converts the DD into a (heavily nested) if-then-else expression that represents the very same function.
             * The variable names used in the expression are derived from the meta variable name and are extended with a
             * suffix ".i" if the meta variable is integer-valued, expressing that the variable is the i-th bit of the
             * meta variable.
             *
             * @return The resulting expression.
             */
            storm::expressions::Expression toExpression() const;

            /*!
             * Converts the DD into a (heavily nested) if-then-else (with negations) expression that evaluates to true
             * if and only if the assignment is minterm of the DD. The variable names used in the expression are derived
             * from the meta variable name and are extended with a suffix ".i" if the meta variable is integer-valued,
             * expressing that the variable is the i-th bit of the meta variable.
             *
             * @return The resulting expression.
             */
            storm::expressions::Expression getMintermExpression() const;
            
            friend std::ostream & operator<<(std::ostream& out, const Dd<DdType::CUDD>& dd);
        private:
            /*!
             * Retrieves a reference to the CUDD ADD object associated with this DD.
             *
             * @return The CUDD ADD object associated with this DD.
             */
            ADD getCuddAdd();
            
            /*!
             * Retrieves the CUDD ADD object associated with this DD.
             *
             * @return The CUDD ADD object assoicated with this DD.
             */
            ADD const& getCuddAdd() const;
            
            /*!
             * Adds the given meta variable name to the set of meta variables that are contained in this DD.
             *
             * @param metaVariableName The name of the meta variable to add.
             */
            void addContainedMetaVariable(std::string const& metaVariableName);
            
            /*!
             * Removes the given meta variable name to the set of meta variables that are contained in this DD.
             *
             * @param metaVariableName The name of the meta variable to remove.
             */
            void removeContainedMetaVariable(std::string const& metaVariableName);
            
            /*!
             * Performs the recursive step of toExpression on the given DD.
             *
             * @param dd The dd to translate into an expression.
             * @param variableNames The names of the variables to use in the expression.
             * @return The resulting expression.
             */
            static storm::expressions::Expression toExpressionRecur(DdNode const* dd, std::vector<std::string> const& variableNames);
            
            /*!
             * Performs the recursive step of getMintermExpression on the given DD.
             *
             * @param manager The manager of the DD.
             * @param dd The dd whose minterms to translate into an expression.
             * @param variableNames The names of the variables to use in the expression.
             * @return The resulting expression.
             */
            static storm::expressions::Expression getMintermExpressionRecur(::DdManager* manager, DdNode const* dd, std::vector<std::string> const& variableNames);
            
            /*!
             * Creates a DD that encapsulates the given CUDD ADD.
             *
             * @param ddManager The manager responsible for this DD.
             * @param cuddAdd The CUDD ADD to store.
             * @param containedMetaVariableNames The names of the meta variables that appear in the DD.
             */
            Dd(std::shared_ptr<DdManager<DdType::CUDD>> ddManager, ADD cuddAdd, std::set<std::string> const& containedMetaVariableNames = std::set<std::string>());
            
            /*!
             * Helper function to convert the DD into a (sparse) matrix.
             *
             * @param dd The DD to convert.
             * @param rowIndications A vector indicating at which position in the columnsAndValues vector the entries
             * of row i start. Note: this vector is modified in the computation. More concretely, each entry i in the
             * vector will be increased by the number of entries in the row. This can be used to count the number
             * of entries in each row. If the values are not to be modified, a copy needs to be provided or the entries
             * need to be restored afterwards.
             * @param columnsAndValues The vector that will hold the columns and values of non-zero entries upon successful
             * completion.
             * @param rowGroupOffsets The row offsets at which a given row group starts.
             * @param rowOdd The ODD used for the row translation.
             * @param columnOdd The ODD used for the column translation.
             * @param currentRowLevel The currently considered row level in the DD.
             * @param currentColumnLevel The currently considered row level in the DD.
             * @param maxLevel The number of levels that need to be considered.
             * @param currentRowOffset The current row offset.
             * @param currentColumnOffset The current row offset.
             * @param ddRowVariableIndices The (sorted) indices of all DD row variables that need to be considered.
             * @param ddColumnVariableIndices The (sorted) indices of all DD row variables that need to be considered.
             * @param generateValues If set to true, the vector columnsAndValues is filled with the actual entries, which
             * only works if the offsets given in rowIndications are already correct. If they need to be computed first,
             * this flag needs to be false.
             */
            void toMatrixRec(DdNode const* dd, std::vector<uint_fast64_t>& rowIndications, std::vector<storm::storage::MatrixEntry<uint_fast64_t, double>>& columnsAndValues, std::vector<uint_fast64_t> const& rowGroupOffsets, Odd<DdType::CUDD> const& rowOdd, Odd<DdType::CUDD> const& columnOdd, uint_fast64_t currentRowLevel, uint_fast64_t currentColumnLevel, uint_fast64_t maxLevel, uint_fast64_t currentRowOffset, uint_fast64_t currentColumnOffset, std::vector<uint_fast64_t> const& ddRowVariableIndices, std::vector<uint_fast64_t> const& ddColumnVariableIndices, bool generateValues = true) const;
            
            /*!
             * Splits the given matrix DD into the groups using the given group variables.
             *
             * @param dd The DD to split.
             * @param groups A vector that is to be filled with the DDs for the individual groups.
             * @param ddGroupVariableIndices The (sorted) indices of all DD group variables that need to be considered.
             * @param currentLevel The currently considered level in the DD.
             * @param maxLevel The number of levels that need to be considered.
             * @param remainingMetaVariables The meta variables that remain in the DDs after the groups have been split.
             */
            void splitGroupsRec(DdNode* dd, std::vector<Dd<DdType::CUDD>>& groups, std::vector<uint_fast64_t> const& ddGroupVariableIndices, uint_fast64_t currentLevel, uint_fast64_t maxLevel, std::set<std::string> const& remainingMetaVariables) const;
            
            /*!
             * Performs a recursive step to add the given DD-based vector to the given explicit vector.
             *
             * @param dd The DD to add to the explicit vector.
             * @param currentLevel The currently considered level in the DD.
             * @param maxLevel The number of levels that need to be considered.
             * @param currentOffset The current offset.
             * @param odd The ODD used for the translation.
             * @param ddVariableIndices The (sorted) indices of all DD variables that need to be considered.
             * @param targetVector The vector to which the translated DD-based vector is to be added.
             */
            template<typename ValueType>
            void addToVectorRec(DdNode const* dd, uint_fast64_t currentLevel, uint_fast64_t maxLevel, uint_fast64_t currentOffset, Odd<DdType::CUDD> const& odd, std::vector<uint_fast64_t> const& ddVariableIndices, std::vector<ValueType>& targetVector) const;
            
            /*!
             * Retrieves the indices of all DD variables that are contained in this DD (not necessarily in the support,
             * because they could be "don't cares"). Additionally, the indices are sorted to allow for easy access.
             *
             * @return The (sorted) indices of all DD variables that are contained in this DD.
             */
            std::vector<uint_fast64_t> getSortedVariableIndices() const;
            
            // A pointer to the manager responsible for this DD.
            std::shared_ptr<DdManager<DdType::CUDD>> ddManager;
            
            // The ADD created by CUDD.
            ADD cuddAdd;
            
            // The names of all meta variables that appear in this DD.
            std::set<std::string> containedMetaVariableNames;
        };
    }
}

#endif /* STORM_STORAGE_DD_CUDDDD_H_ */