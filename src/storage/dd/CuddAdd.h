#ifndef STORM_STORAGE_DD_CUDDADD_H_
#define STORM_STORAGE_DD_CUDDADD_H_

#include "src/storage/dd/Add.h"
#include "src/storage/dd/CuddDd.h"
#include "src/storage/dd/CuddDdForwardIterator.h"
#include "src/storage/SparseMatrix.h"
#include "src/storage/expressions/Variable.h"
#include "src/utility/OsDetection.h"

// Include the C++-interface of CUDD.
#include "cuddObj.hh"

namespace storm {
    namespace dd {
        // Forward-declare some classes.
        template<DdType Type> class DdManager;
        template<DdType Type> class Odd;
        template<DdType Type> class Bdd;
        
        template<>
        class Add<DdType::CUDD> : public Dd<DdType::CUDD> {
        public:
            // Declare the DdManager and DdIterator class as friend so it can access the internals of a DD.
            friend class DdManager<DdType::CUDD>;
            friend class DdForwardIterator<DdType::CUDD>;
            friend class Bdd<DdType::CUDD>;
            friend class Odd<DdType::CUDD>;
            
            // Instantiate all copy/move constructors/assignments with the default implementation.
            Add() = default;
            Add(Add<DdType::CUDD> const& other) = default;
            Add& operator=(Add<DdType::CUDD> const& other) = default;
#ifndef WINDOWS
            Add(Add<DdType::CUDD>&& other) = default;
            Add& operator=(Add<DdType::CUDD>&& other) = default;
#endif
            
            /*!
             * Retrieves whether the two DDs represent the same function.
             *
             * @param other The DD that is to be compared with the current one.
             * @return True if the DDs represent the same function.
             */
            bool operator==(Add<DdType::CUDD> const& other) const;
            
            /*!
             * Retrieves whether the two DDs represent different functions.
             *
             * @param other The DD that is to be compared with the current one.
             * @return True if the DDs represent the different functions.
             */
            bool operator!=(Add<DdType::CUDD> const& other) const;
            
            /*!
             * Performs an if-then-else with the given operands, i.e. maps all valuations that are mapped to a non-zero
             * function value to the function values specified by the first DD and all others to the function values
             * specified by the second DD.
             *
             * @param thenDd The ADD specifying the 'then' part.
             * @param elseDd The ADD specifying the 'else' part.
             * @return The ADD corresponding to the if-then-else of the operands.
             */
            Add<DdType::CUDD> ite(Add<DdType::CUDD> const& thenDd, Add<DdType::CUDD> const& elseDd) const;
            
            /*!
             * Logically inverts the current ADD. That is, all inputs yielding non-zero values will be mapped to zero in
             * the result and vice versa.
             *
             * @return The resulting ADD.
             */
            Add<DdType::CUDD> operator!() const;
            
            /*!
             * Performs a logical or of the current anBd the given ADD. As a prerequisite, the operand ADDs need to be
             * 0/1 ADDs.
             *
             * @param other The second ADD used for the operation.
             * @return The logical or of the operands.
             */
            Add<DdType::CUDD> operator||(Add<DdType::CUDD> const& other) const;
            
            /*!
             * Performs a logical or of the current and the given ADD and assigns it to the current ADD. As a
             * prerequisite, the operand ADDs need to be 0/1 ADDs.
             *
             * @param other The second ADD used for the operation.
             * @return A reference to the current ADD after the operation
             */
            Add<DdType::CUDD>& operator|=(Add<DdType::CUDD> const& other);
            
            /*!
             * Adds the two ADDs.
             *
             * @param other The ADD to add to the current one.
             * @return The result of the addition.
             */
            Add<DdType::CUDD> operator+(Add<DdType::CUDD> const& other) const;
            
            /*!
             * Adds the given ADD to the current one.
             *
             * @param other The ADD to add to the current one.
             * @return A reference to the current ADD after the operation.
             */
            Add<DdType::CUDD>& operator+=(Add<DdType::CUDD> const& other);
            
            /*!
             * Multiplies the two ADDs.
             *
             * @param other The ADD to multiply with the current one.
             * @return The result of the multiplication.
             */
            Add<DdType::CUDD> operator*(Add<DdType::CUDD> const& other) const;
            
            /*!
             * Multiplies the given ADD with the current one and assigns the result to the current ADD.
             *
             * @param other The ADD to multiply with the current one.
             * @return A reference to the current ADD after the operation.
             */
            Add<DdType::CUDD>& operator*=(Add<DdType::CUDD> const& other);
            
            /*!
             * Subtracts the given ADD from the current one.
             *
             * @param other The ADD to subtract from the current one.
             * @return The result of the subtraction.
             */
            Add<DdType::CUDD> operator-(Add<DdType::CUDD> const& other) const;
            
            /*!
             * Subtracts the ADD from the constant zero function.
             *
             * @return The resulting function represented as a ADD.
             */
            Add<DdType::CUDD> operator-() const;
            
            /*!
             * Subtracts the given ADD from the current one and assigns the result to the current ADD.
             *
             * @param other The ADD to subtract from the current one.
             * @return A reference to the current ADD after the operation.
             */
            Add<DdType::CUDD>& operator-=(Add<DdType::CUDD> const& other);
            
            /*!
             * Divides the current ADD by the given one.
             *
             * @param other The ADD by which to divide the current one.
             * @return The result of the division.
             */
            Add<DdType::CUDD> operator/(Add<DdType::CUDD> const& other) const;
            
            /*!
             * Divides the current ADD by the given one and assigns the result to the current ADD.
             *
             * @param other The ADD by which to divide the current one.
             * @return A reference to the current ADD after the operation.
             */
            Add<DdType::CUDD>& operator/=(Add<DdType::CUDD> const& other);
            
            /*!
             * Retrieves the function that maps all evaluations to one that have identical function values.
             *
             * @param other The ADD with which to perform the operation.
             * @return The resulting function represented as an ADD.
             */
            Add<DdType::CUDD> equals(Add<DdType::CUDD> const& other) const;
            
            /*!
             * Retrieves the function that maps all evaluations to one that have distinct function values.
             *
             * @param other The ADD with which to perform the operation.
             * @return The resulting function represented as an ADD.
             */
            Add<DdType::CUDD> notEquals(Add<DdType::CUDD> const& other) const;
            
            /*!
             * Retrieves the function that maps all evaluations to one whose function value in the first ADD are less
             * than the one in the given ADD.
             *
             * @param other The ADD with which to perform the operation.
             * @return The resulting function represented as an ADD.
             */
            Add<DdType::CUDD> less(Add<DdType::CUDD> const& other) const;
            
            /*!
             * Retrieves the function that maps all evaluations to one whose function value in the first ADD are less or
             * equal than the one in the given ADD.
             *
             * @param other The DD with which to perform the operation.
             * @return The resulting function represented as an ADD.
             */
            Add<DdType::CUDD> lessOrEqual(Add<DdType::CUDD> const& other) const;
            
            /*!
             * Retrieves the function that maps all evaluations to one whose function value in the first ADD are greater
             * than the one in the given ADD.
             *
             * @param other The ADD with which to perform the operation.
             * @return The resulting function represented as an ADD.
             */
            Add<DdType::CUDD> greater(Add<DdType::CUDD> const& other) const;
            
            /*!
             * Retrieves the function that maps all evaluations to one whose function value in the first ADD are greater
             * or equal than the one in the given ADD.
             *
             * @param other The ADD with which to perform the operation.
             * @return The resulting function represented as an ADD.
             */
            Add<DdType::CUDD> greaterOrEqual(Add<DdType::CUDD> const& other) const;
            
            /*!
             * Retrieves the function that maps all evaluations to the minimum of the function values of the two ADDs.
             *
             * @param other The ADD with which to perform the operation.
             * @return The resulting function represented as an ADD.
             */
            Add<DdType::CUDD> minimum(Add<DdType::CUDD> const& other) const;
            
            /*!
             * Retrieves the function that maps all evaluations to the maximum of the function values of the two ADDs.
             *
             * @param other The ADD with which to perform the operation.
             * @return The resulting function represented as an ADD.
             */
            Add<DdType::CUDD> maximum(Add<DdType::CUDD> const& other) const;
            
            /*!
             * Sum-abstracts from the given meta variables.
             *
             * @param metaVariables The meta variables from which to abstract.
             */
            Add<DdType::CUDD> sumAbstract(std::set<storm::expressions::Variable> const& metaVariables) const;
            
            /*!
             * Min-abstracts from the given meta variables.
             *
             * @param metaVariables The meta variables from which to abstract.
             */
            Add<DdType::CUDD> minAbstract(std::set<storm::expressions::Variable> const& metaVariables) const;
            
            /*!
             * Max-abstracts from the given meta variables.
             *
             * @param metaVariables The meta variables from which to abstract.
             */
            Add<DdType::CUDD> maxAbstract(std::set<storm::expressions::Variable> const& metaVariables) const;
            
            /*!
             * Checks whether the current and the given ADD represent the same function modulo some given precision.
             *
             * @param other The ADD with which to compare.
             * @param precision An upper bound on the maximal difference between any two function values that is to be
             * tolerated.
             * @param relative If set to true, not the absolute values have to be within the precision, but the relative
             * values.
             */
            bool equalModuloPrecision(Add<DdType::CUDD> const& other, double precision, bool relative = true) const;
            
            /*!
             * Swaps the given pairs of meta variables in the ADD. The pairs of meta variables must be guaranteed to have
             * the same number of underlying ADD variables.
             *
             * @param metaVariablePairs A vector of meta variable pairs that are to be swapped for one another.
             * @return The resulting ADD.
             */
            Add<DdType::CUDD> swapVariables(std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& metaVariablePairs);
            
            /*!
             * Multiplies the current ADD (representing a matrix) with the given matrix by summing over the given meta
             * variables.
             *
             * @param otherMatrix The matrix with which to multiply.
             * @param summationMetaVariables The names of the meta variables over which to sum during the matrix-
             * matrix multiplication.
             * @return An ADD representing the result of the matrix-matrix multiplication.
             */
            Add<DdType::CUDD> multiplyMatrix(Add<DdType::CUDD> const& otherMatrix, std::set<storm::expressions::Variable> const& summationMetaVariables) const;
            
            /*!
             * Computes a BDD that represents the function in which all assignments with a function value strictly
             * larger than the given value are mapped to one and all others to zero.
             *
             * @param value The value used for the comparison.
             * @return The resulting BDD.
             */
            Bdd<DdType::CUDD> greater(double value) const;
            
            /*!
             * Computes a BDD that represents the function in which all assignments with a function value larger or equal
             * to the given value are mapped to one and all others to zero.
             *
             * @param value The value used for the comparison.
             * @return The resulting BDD.
             */
            Bdd<DdType::CUDD> greaterOrEqual(double value) const;
            
            /*!
             * Computes a BDD that represents the function in which all assignments with a function value unequal to
             * zero are mapped to one and all others to zero.
             *
             * @return The resulting DD.
             */
            Bdd<DdType::CUDD> notZero() const;
            
            /*!
             * Computes the constraint of the current ADD with the given constraint. That is, the function value of the
             * resulting ADD will be the same as the current ones for all assignments mapping to one in the constraint
             * and may be different otherwise.
             *
             * @param constraint The constraint to use for the operation.
             * @return The resulting ADD.
             */
            Add<DdType::CUDD> constrain(Add<DdType::CUDD> const& constraint) const;
            
            /*!
             * Computes the restriction of the current ADD with the given constraint. That is, the function value of the
             * resulting DD will be the same as the current ones for all assignments mapping to one in the constraint
             * and may be different otherwise.
             *
             * @param constraint The constraint to use for the operation.
             * @return The resulting ADD.
             */
            Add<DdType::CUDD> restrict(Add<DdType::CUDD> const& constraint) const;
            
            /*!
             * Retrieves the support of the current ADD.
             *
             * @return The support represented as a BDD.
             */
            Bdd<DdType::CUDD> getSupport() const;
            
            /*!
             * Retrieves the number of encodings that are mapped to a non-zero value.
             *
             * @return The number of encodings that are mapped to a non-zero value.
             */
            virtual uint_fast64_t getNonZeroCount() const override;
            
            /*!
             * Retrieves the number of leaves of the ADD.
             *
             * @return The number of leaves of the ADD.
             */
            virtual uint_fast64_t getLeafCount() const override;
            
            /*!
             * Retrieves the number of nodes necessary to represent the DD.
             *
             * @return The number of nodes in this DD.
             */
            virtual uint_fast64_t getNodeCount() const override;
            
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
             * @param metaVariable The meta variable that has to be equal to the given value.
             * @param variableValue The value that the meta variable is supposed to have. This must be within the range
             * of the meta variable.
             * @param targetValue The new function value of the modified encodings.
             */
            void setValue(storm::expressions::Variable const& metaVariable, int_fast64_t variableValue, double targetValue);
            
            /*!
             * Sets the function values of all encodings that have the given values of the two meta variables to the
             * given target value.
             *
             * @param metaVariable1 The first meta variable that has to be equal to the first given
             * value.
             * @param variableValue1 The value that the first meta variable is supposed to have. This must be within the
             * range of the meta variable.
             * @param metaVariable2 The second meta variable that has to be equal to the second given
             * value.
             * @param variableValue2 The value that the second meta variable is supposed to have. This must be within
             * the range of the meta variable.
             * @param targetValue The new function value of the modified encodings.
             */
            void setValue(storm::expressions::Variable const& metaVariable1, int_fast64_t variableValue1, storm::expressions::Variable const& metaVariable2, int_fast64_t variableValue2, double targetValue);
            
            /*!
             * Sets the function values of all encodings that have the given values of the given meta variables to the
             * given target value.
             *
             * @param metaVariableToValueMap A mapping of meta variables to the values they are supposed to have. All
             * values must be within the range of the respective meta variable.
             * @param targetValue The new function value of the modified encodings.
             */
            void setValue(std::map<storm::expressions::Variable, int_fast64_t> const& metaVariableToValueMap = std::map<storm::expressions::Variable, int_fast64_t>(), double targetValue = 0);
            
            /*!
             * Retrieves the value of the function when all meta variables are assigned the values of the given mapping.
             * Note that the mapping must specify values for all meta variables contained in the DD.
             *
             * @param metaVariableToValueMap A mapping of meta variables to their values.
             * @return The value of the function evaluated with the given input.
             */
            double getValue(std::map<storm::expressions::Variable, int_fast64_t> const& metaVariableToValueMap = std::map<storm::expressions::Variable, int_fast64_t>()) const;
            
            /*!
             * Retrieves whether this ADD represents the constant one function.
             *
             * @return True if this ADD represents the constant one function.
             */
            bool isOne() const;
            
            /*!
             * Retrieves whether this ADD represents the constant zero function.
             *
             * @return True if this ADD represents the constant zero function.
             */
            bool isZero() const;
            
            /*!
             * Retrieves whether this ADD represents a constant function.
             *
             * @return True if this ADD represents a constants function.
             */
            bool isConstant() const;
            
            /*!
             * Retrieves the index of the topmost variable in the DD.
             *
             * @return The index of the topmost variable in DD.
             */
            virtual uint_fast64_t getIndex() const override;
            
            /*!
             * Converts the ADD to a vector.
             *
             * @return The double vector that is represented by this ADD.
             */
            template<typename ValueType>
            std::vector<ValueType> toVector() const;
            
            /*!
             * Converts the ADD to a vector. The given offset-labeled DD is used to determine the correct row of
             * each entry.
             *
             * @param rowOdd The ODD used for determining the correct row.
             * @return The double vector that is represented by this ADD.
             */
            template<typename ValueType>
            std::vector<ValueType> toVector(storm::dd::Odd<DdType::CUDD> const& rowOdd) const;
            
            /*!
             * Converts the ADD to a (sparse) double matrix. All contained non-primed variables are assumed to encode the
             * row, whereas all primed variables are assumed to encode the column.
             *
             * @return The matrix that is represented by this ADD.
             */
            storm::storage::SparseMatrix<double> toMatrix() const;
            
            /*!
             * Converts the ADD to a (sparse) double matrix. All contained non-primed variables are assumed to encode the
             * row, whereas all primed variables are assumed to encode the column. The given offset-labeled DDs are used
             * to determine the correct row and column, respectively, for each entry.
             *
             * @param rowOdd The ODD used for determining the correct row.
             * @param columnOdd The ODD used for determining the correct column.
             * @return The matrix that is represented by this ADD.
             */
            storm::storage::SparseMatrix<double> toMatrix(storm::dd::Odd<DdType::CUDD> const& rowOdd, storm::dd::Odd<DdType::CUDD> const& columnOdd) const;
            
            /*!
             * Converts the ADD to a (sparse) double matrix. The given offset-labeled DDs are used to determine the
             * correct row and column, respectively, for each entry.
             *
             * @param rowMetaVariables The meta variables that encode the rows of the matrix.
             * @param columnMetaVariables The meta variables that encode the columns of the matrix.
             * @param rowOdd The ODD used for determining the correct row.
             * @param columnOdd The ODD used for determining the correct column.
             * @return The matrix that is represented by this ADD.
             */
            storm::storage::SparseMatrix<double> toMatrix(std::set<storm::expressions::Variable> const& rowMetaVariables, std::set<storm::expressions::Variable> const& columnMetaVariables, storm::dd::Odd<DdType::CUDD> const& rowOdd, storm::dd::Odd<DdType::CUDD> const& columnOdd) const;
            
            /*!
             * Converts the ADD to a row-grouped (sparse) double matrix. The given offset-labeled DDs are used to
             * determine the correct row and column, respectively, for each entry. Note: this function assumes that
             * the meta variables used to distinguish different row groups are at the very top of the ADD.
             *
             * @param rowMetaVariables The meta variables that encode the rows of the matrix.
             * @param columnMetaVariables The meta variables that encode the columns of the matrix.
             * @param groupMetaVariables The meta variables that are used to distinguish different row groups.
             * @param rowOdd The ODD used for determining the correct row.
             * @param columnOdd The ODD used for determining the correct column.
             * @return The matrix that is represented by this ADD.
             */
            storm::storage::SparseMatrix<double> toMatrix(std::set<storm::expressions::Variable> const& rowMetaVariables, std::set<storm::expressions::Variable> const& columnMetaVariables, std::set<storm::expressions::Variable> const& groupMetaVariables, storm::dd::Odd<DdType::CUDD> const& rowOdd, storm::dd::Odd<DdType::CUDD> const& columnOdd) const;
            
            /*!
             * Exports the DD to the given file in the dot format.
             *
             * @param filename The name of the file to which the DD is to be exported.
             */
            void exportToDot(std::string const& filename = "") const;
            
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
            
            friend std::ostream & operator<<(std::ostream& out, const Add<DdType::CUDD>& add);
            
            /*!
             * Converts the ADD to a BDD by mapping all values unequal to zero to 1. This effectively does the same as
             * a call to notZero().
             *
             * @return The corresponding BDD.
             */
            Bdd<DdType::CUDD> toBdd() const;
            
        private:
            
            /*!
             * Retrieves the CUDD ADD object associated with this ADD.
             *
             * @return The CUDD ADD object associated with this ADD.
             */
            ADD getCuddAdd() const;
            
            /*!
             * Retrieves the raw DD node of CUDD associated with this ADD.
             *
             * @return The DD node of CUDD associated with this ADD.
             */
            DdNode* getCuddDdNode() const;
            
            /*!
             * Creates an ADD that encapsulates the given CUDD ADD.
             *
             * @param ddManager The manager responsible for this DD.
             * @param cuddAdd The CUDD ADD to store.
             * @param containedMetaVariables The meta variables that appear in the DD.
             */
            Add(std::shared_ptr<DdManager<DdType::CUDD> const> ddManager, ADD cuddAdd, std::set<storm::expressions::Variable> const& containedMetaVariables = std::set<storm::expressions::Variable>());
            
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
            void splitGroupsRec(DdNode* dd, std::vector<Add<DdType::CUDD>>& groups, std::vector<uint_fast64_t> const& ddGroupVariableIndices, uint_fast64_t currentLevel, uint_fast64_t maxLevel, std::set<storm::expressions::Variable> const& remainingMetaVariables) const;
            
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
            
            // The ADD created by CUDD.
            ADD cuddAdd;
        };
    }
}

#endif /* STORM_STORAGE_DD_CUDDADD_H_ */