#ifndef STORM_STORAGE_DD_CUDD_INTERNALSYLVANADD_H_
#define STORM_STORAGE_DD_CUDD_INTERNALSYLVANADD_H_

#include <set>
#include <unordered_map>

#include "src/storage/dd/DdType.h"
#include "src/storage/dd/InternalAdd.h"
#include "src/storage/dd/Odd.h"

#include "src/storage/dd/sylvan/InternalSylvanBdd.h"
#include "src/storage/dd/sylvan/SylvanAddIterator.h"

#include "src/storage/expressions/Variable.h"

namespace storm {
    namespace storage {
        template<typename T>
        class SparseMatrix;
        
        class BitVector;
        
        template<typename E, typename V>
        class MatrixEntry;
    }
    
    namespace dd {
        template<DdType LibraryType>
        class DdManager;
        
        template<DdType LibraryType>
        class InternalDdManager;
        
        template<DdType LibraryType>
        class InternalBdd;
        
        template<DdType LibraryType, typename ValueType>
        class AddIterator;
        
        template<typename ValueType>
        class InternalAdd<DdType::Sylvan, ValueType> {
        public:
            /*!
             * Creates an ADD that encapsulates the given Sylvan MTBDD.
             *
             * @param ddManager The manager responsible for this DD.
             * @param sylvanMtbdd The sylvan MTBDD to store.
             */
            InternalAdd(InternalDdManager<DdType::Sylvan> const* ddManager, sylvan::Mtbdd const& sylvanMtbdd);
            
            // Instantiate all copy/move constructors/assignments with the default implementation.
            InternalAdd() = default;
            InternalAdd(InternalAdd<DdType::Sylvan, ValueType> const& other) = default;
            InternalAdd& operator=(InternalAdd<DdType::Sylvan, ValueType> const& other) = default;
            InternalAdd(InternalAdd<DdType::Sylvan, ValueType>&& other) = default;
            InternalAdd& operator=(InternalAdd<DdType::Sylvan, ValueType>&& other) = default;
                        
            /*!
             * Retrieves whether the two DDs represent the same function.
             *
             * @param other The DD that is to be compared with the current one.
             * @return True if the DDs represent the same function.
             */
            bool operator==(InternalAdd<DdType::Sylvan, ValueType> const& other) const;
            
            /*!
             * Retrieves whether the two DDs represent different functions.
             *
             * @param other The DD that is to be compared with the current one.
             * @return True if the DDs represent the different functions.
             */
            bool operator!=(InternalAdd<DdType::Sylvan, ValueType> const& other) const;
            
            /*!
             * Performs an if-then-else with the given operands, i.e. maps all valuations that are mapped to a non-zero
             * function value to the function values specified by the first DD and all others to the function values
             * specified by the second DD.
             *
             * @param thenDd The ADD specifying the 'then' part.
             * @param elseDd The ADD specifying the 'else' part.
             * @return The ADD corresponding to the if-then-else of the operands.
             */
            InternalAdd<DdType::Sylvan, ValueType> ite(InternalAdd<DdType::Sylvan, ValueType> const& thenAdd, InternalAdd<DdType::Sylvan, ValueType> const& elseAdd) const;
                        
            /*!
             * Adds the two ADDs.
             *
             * @param other The ADD to add to the current one.
             * @return The result of the addition.
             */
            InternalAdd<DdType::Sylvan, ValueType> operator+(InternalAdd<DdType::Sylvan, ValueType> const& other) const;
            
            /*!
             * Adds the given ADD to the current one.
             *
             * @param other The ADD to add to the current one.
             * @return A reference to the current ADD after the operation.
             */
            InternalAdd<DdType::Sylvan, ValueType>& operator+=(InternalAdd<DdType::Sylvan, ValueType> const& other);
            
            /*!
             * Multiplies the two ADDs.
             *
             * @param other The ADD to multiply with the current one.
             * @return The result of the multiplication.
             */
            InternalAdd<DdType::Sylvan, ValueType> operator*(InternalAdd<DdType::Sylvan, ValueType> const& other) const;
            
            /*!
             * Multiplies the given ADD with the current one and assigns the result to the current ADD.
             *
             * @param other The ADD to multiply with the current one.
             * @return A reference to the current ADD after the operation.
             */
            InternalAdd<DdType::Sylvan, ValueType>& operator*=(InternalAdd<DdType::Sylvan, ValueType> const& other);
            
            /*!
             * Subtracts the given ADD from the current one.
             *
             * @param other The ADD to subtract from the current one.
             * @return The result of the subtraction.
             */
            InternalAdd<DdType::Sylvan, ValueType> operator-(InternalAdd<DdType::Sylvan, ValueType> const& other) const;
            
            /*!
             * Subtracts the given ADD from the current one and assigns the result to the current ADD.
             *
             * @param other The ADD to subtract from the current one.
             * @return A reference to the current ADD after the operation.
             */
            InternalAdd<DdType::Sylvan, ValueType>& operator-=(InternalAdd<DdType::Sylvan, ValueType> const& other);
            
            /*!
             * Divides the current ADD by the given one.
             *
             * @param other The ADD by which to divide the current one.
             * @return The result of the division.
             */
            InternalAdd<DdType::Sylvan, ValueType> operator/(InternalAdd<DdType::Sylvan, ValueType> const& other) const;
            
            /*!
             * Divides the current ADD by the given one and assigns the result to the current ADD.
             *
             * @param other The ADD by which to divide the current one.
             * @return A reference to the current ADD after the operation.
             */
            InternalAdd<DdType::Sylvan, ValueType>& operator/=(InternalAdd<DdType::Sylvan, ValueType> const& other);
            
            /*!
             * Retrieves the function that maps all evaluations to one that have identical function values.
             *
             * @param other The ADD with which to perform the operation.
             * @return The resulting function represented as an ADD.
             */
            InternalBdd<DdType::Sylvan> equals(InternalAdd<DdType::Sylvan, ValueType> const& other) const;
            
            /*!
             * Retrieves the function that maps all evaluations to one that have distinct function values.
             *
             * @param other The ADD with which to perform the operation.
             * @return The resulting function represented as an ADD.
             */
            InternalBdd<DdType::Sylvan> notEquals(InternalAdd<DdType::Sylvan, ValueType> const& other) const;
            
            /*!
             * Retrieves the function that maps all evaluations to one whose function value in the first ADD are less
             * than the one in the given ADD.
             *
             * @param other The ADD with which to perform the operation.
             * @return The resulting function represented as an ADD.
             */
            InternalBdd<DdType::Sylvan> less(InternalAdd<DdType::Sylvan, ValueType> const& other) const;
            
            /*!
             * Retrieves the function that maps all evaluations to one whose function value in the first ADD are less or
             * equal than the one in the given ADD.
             *
             * @param other The DD with which to perform the operation.
             * @return The resulting function represented as an ADD.
             */
            InternalBdd<DdType::Sylvan> lessOrEqual(InternalAdd<DdType::Sylvan, ValueType> const& other) const;
            
            /*!
             * Retrieves the function that maps all evaluations to one whose function value in the first ADD are greater
             * than the one in the given ADD.
             *
             * @param other The ADD with which to perform the operation.
             * @return The resulting function represented as an ADD.
             */
            InternalBdd<DdType::Sylvan> greater(InternalAdd<DdType::Sylvan, ValueType> const& other) const;
            
            /*!
             * Retrieves the function that maps all evaluations to one whose function value in the first ADD are greater
             * or equal than the one in the given ADD.
             *
             * @param other The ADD with which to perform the operation.
             * @return The resulting function represented as an ADD.
             */
            InternalBdd<DdType::Sylvan> greaterOrEqual(InternalAdd<DdType::Sylvan, ValueType> const& other) const;
            
            /*!
             * Retrieves the function that represents the current ADD to the power of the given ADD.
             *
             * @other The exponent function (given as an ADD).
             * @retur The resulting ADD.
             */
            InternalAdd<DdType::Sylvan, ValueType> pow(InternalAdd<DdType::Sylvan, ValueType> const& other) const;
            
            /*!
             * Retrieves the function that represents the current ADD modulo the given ADD.
             *
             * @other The modul function (given as an ADD).
             * @retur The resulting ADD.
             */
            InternalAdd<DdType::Sylvan, ValueType> mod(InternalAdd<DdType::Sylvan, ValueType> const& other) const;
            
            /*!
             * Retrieves the function that represents the logarithm of the current ADD to the bases given by the second
             * ADD.
             *
             * @other The base function (given as an ADD).
             * @retur The resulting ADD.
             */
            InternalAdd<DdType::Sylvan, ValueType> logxy(InternalAdd<DdType::Sylvan, ValueType> const& other) const;
            
            /*!
             * Retrieves the function that floors all values in the current ADD.
             *
             * @retur The resulting ADD.
             */
            InternalAdd<DdType::Sylvan, ValueType> floor() const;
            
            /*!
             * Retrieves the function that ceils all values in the current ADD.
             *
             * @retur The resulting ADD.
             */
            InternalAdd<DdType::Sylvan, ValueType> ceil() const;
            
            /*!
             * Retrieves the function that maps all evaluations to the minimum of the function values of the two ADDs.
             *
             * @param other The ADD with which to perform the operation.
             * @return The resulting function represented as an ADD.
             */
            InternalAdd<DdType::Sylvan, ValueType> minimum(InternalAdd<DdType::Sylvan, ValueType> const& other) const;
            
            /*!
             * Retrieves the function that maps all evaluations to the maximum of the function values of the two ADDs.
             *
             * @param other The ADD with which to perform the operation.
             * @return The resulting function represented as an ADD.
             */
            InternalAdd<DdType::Sylvan, ValueType> maximum(InternalAdd<DdType::Sylvan, ValueType> const& other) const;
            
            /*!
             * Sum-abstracts from the given cube.
             *
             * @param cube The cube from which to abstract.
             */
            InternalAdd<DdType::Sylvan, ValueType> sumAbstract(InternalBdd<DdType::Sylvan> const& cube) const;
            
            /*!
             * Min-abstracts from the given cube.
             *
             * @param cube The cube from which to abstract.
             */
            InternalAdd<DdType::Sylvan, ValueType> minAbstract(InternalBdd<DdType::Sylvan> const& cube) const;
            
            /*!
             * Max-abstracts from the given cube.
             *
             * @param cube The cube from which to abstract.
             */
            InternalAdd<DdType::Sylvan, ValueType> maxAbstract(InternalBdd<DdType::Sylvan> const& cube) const;
            
            /*!
             * Checks whether the current and the given ADD represent the same function modulo some given precision.
             *
             * @param other The ADD with which to compare.
             * @param precision An upper bound on the maximal difference between any two function values that is to be
             * tolerated.
             * @param relative If set to true, not the absolute values have to be within the precision, but the relative
             * values.
             */
            bool equalModuloPrecision(InternalAdd<DdType::Sylvan, ValueType> const& other, double precision, bool relative = true) const;
            
            /*!
             * Swaps the given pairs of DD variables in the ADD. The pairs of meta variables have to be represented by
             * ADDs must have equal length.
             *
             * @param from The vector that specifies the 'from' part of the variable renaming.
             * @param to The vector that specifies the 'to' part of the variable renaming.
             * @return The resulting ADD.
             */
            InternalAdd<DdType::Sylvan, ValueType> swapVariables(std::vector<InternalBdd<DdType::Sylvan>> const& from, std::vector<InternalBdd<DdType::Sylvan>> const& to) const;
            
            /*!
             * Multiplies the current ADD (representing a matrix) with the given matrix by summing over the given meta
             * variables.
             *
             * @param otherMatrix The matrix with which to multiply.
             * @param summationDdVariables The DD variables (represented as ADDs) over which to sum.
             * @return An ADD representing the result of the matrix-matrix multiplication.
             */
            InternalAdd<DdType::Sylvan, ValueType> multiplyMatrix(InternalAdd<DdType::Sylvan, ValueType> const& otherMatrix, std::vector<InternalAdd<DdType::Sylvan, ValueType>> const& summationDdVariables) const;
            
            /*!
             * Computes a BDD that represents the function in which all assignments with a function value strictly
             * larger than the given value are mapped to one and all others to zero.
             *
             * @param value The value used for the comparison.
             * @return The resulting BDD.
             */
            InternalBdd<DdType::Sylvan> greater(ValueType const& value) const;
            
            /*!
             * Computes a BDD that represents the function in which all assignments with a function value larger or equal
             * to the given value are mapped to one and all others to zero.
             *
             * @param value The value used for the comparison.
             * @return The resulting BDD.
             */
            InternalBdd<DdType::Sylvan> greaterOrEqual(ValueType const& value) const;
            
            /*!
             * Computes a BDD that represents the function in which all assignments with a function value strictly
             * lower than the given value are mapped to one and all others to zero.
             *
             * @param value The value used for the comparison.
             * @return The resulting BDD.
             */
            InternalBdd<DdType::Sylvan> less(ValueType const& value) const;
            
            /*!
             * Computes a BDD that represents the function in which all assignments with a function value less or equal
             * to the given value are mapped to one and all others to zero.
             *
             * @param value The value used for the comparison.
             * @return The resulting BDD.
             */
            InternalBdd<DdType::Sylvan> lessOrEqual(ValueType const& value) const;
            
            /*!
             * Computes a BDD that represents the function in which all assignments with a function value unequal to
             * zero are mapped to one and all others to zero.
             *
             * @return The resulting DD.
             */
            InternalBdd<DdType::Sylvan> notZero() const;
            
            /*!
             * Computes the constraint of the current ADD with the given constraint. That is, the function value of the
             * resulting ADD will be the same as the current ones for all assignments mapping to one in the constraint
             * and may be different otherwise.
             *
             * @param constraint The constraint to use for the operation.
             * @return The resulting ADD.
             */
            InternalAdd<DdType::Sylvan, ValueType> constrain(InternalAdd<DdType::Sylvan, ValueType> const& constraint) const;
            
            /*!
             * Computes the restriction of the current ADD with the given constraint. That is, the function value of the
             * resulting DD will be the same as the current ones for all assignments mapping to one in the constraint
             * and may be different otherwise.
             *
             * @param constraint The constraint to use for the operation.
             * @return The resulting ADD.
             */
            InternalAdd<DdType::Sylvan, ValueType> restrict(InternalAdd<DdType::Sylvan, ValueType> const& constraint) const;
            
            /*!
             * Retrieves the support of the current ADD.
             *
             * @return The support represented as a BDD.
             */
            InternalBdd<DdType::Sylvan> getSupport() const;
            
            /*!
             * Retrieves the number of encodings that are mapped to a non-zero value.
             *
             * @param numberOfDdVariables The number of DD variables contained in this BDD.
             * @return The number of encodings that are mapped to a non-zero value.
             */
            virtual uint_fast64_t getNonZeroCount(uint_fast64_t numberOfDdVariables) const;
            
            /*!
             * Retrieves the number of leaves of the ADD.
             *
             * @return The number of leaves of the ADD.
             */
            virtual uint_fast64_t getLeafCount() const;
            
            /*!
             * Retrieves the number of nodes necessary to represent the DD.
             *
             * @return The number of nodes in this DD.
             */
            virtual uint_fast64_t getNodeCount() const;
            
            /*!
             * Retrieves the lowest function value of any encoding.
             *
             * @return The lowest function value of any encoding.
             */
            ValueType getMin() const;
            
            /*!
             * Retrieves the highest function value of any encoding.
             *
             * @return The highest function value of any encoding.
             */
            ValueType getMax() const;
                        
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
            virtual uint_fast64_t getIndex() const;
            
            /*!
             * Exports the DD to the given file in the dot format.
             *
             * @param filename The name of the file to which the DD is to be exported.
             * @param ddVariableNamesAsString The names of the DD variables to display in the dot file.
             */
            void exportToDot(std::string const& filename, std::vector<std::string> const& ddVariableNamesAsStrings) const;
            
            /*!
             * Retrieves an iterator that points to the first meta variable assignment with a non-zero function value.
             *
             * @param fullDdManager The DD manager responsible for this ADD.
             * @param metaVariables The meta variables contained in the ADD.
             * @param enumerateDontCareMetaVariables If set to true, all meta variable assignments are enumerated, even
             * if a meta variable does not at all influence the the function value.
             * @return An iterator that points to the first meta variable assignment with a non-zero function value.
             */
            AddIterator<DdType::Sylvan, ValueType> begin(std::shared_ptr<DdManager<DdType::Sylvan> const> fullDdManager, std::set<storm::expressions::Variable> const& metaVariables, bool enumerateDontCareMetaVariables = true) const;
            
            /*!
             * Retrieves an iterator that points past the end of the container.
             *
             * @param fullDdManager The DD manager responsible for this ADD.
             * @param enumerateDontCareMetaVariables If set to true, all meta variable assignments are enumerated, even
             * if a meta variable does not at all influence the the function value.
             * @return An iterator that points past the end of the container.
             */
            AddIterator<DdType::Sylvan, ValueType> end(std::shared_ptr<DdManager<DdType::Sylvan> const> fullDdManager, bool enumerateDontCareMetaVariables = true) const;
            
            /*!
             * Composes the ADD with an explicit vector by performing a specified function between the entries of this
             * ADD and the explicit vector.
             *
             * @param odd The ODD to use for the translation from symbolic to explicit positions.
             * @param ddVariableIndices The indices of the DD variables present in this ADD.
             * @param targetVector The explicit vector that is to be composed with the ADD. The results are written to
             * this vector again.
             * @param function The function to perform in the composition.
             */
            void composeWithExplicitVector(Odd const& odd, std::vector<uint_fast64_t> const& ddVariableIndices, std::vector<ValueType>& targetVector, std::function<ValueType (ValueType const&, ValueType const&)> const& function) const;
            
            /*!
             * Composes the (row-grouped) ADD with an explicit vector by performing a specified function between the
             * entries of this ADD and the explicit vector.
             *
             * @param odd The ODD to use for the translation from symbolic to explicit positions.
             * @param ddVariableIndices The indices of the DD variables present in this ADD.
             * @param offsets The offsets
             * @param targetVector The explicit vector that is to be composed with the ADD. The results are written to
             * this vector again.
             * @param function The function to perform in the composition.
             */
            void composeWithExplicitVector(Odd const& odd, std::vector<uint_fast64_t> const& ddVariableIndices, std::vector<uint_fast64_t> const& offsets, std::vector<ValueType>& targetVector, std::function<ValueType (ValueType const&, ValueType const&)> const& function) const;
            
            /*!
             * Splits the ADD into several ADDs that differ in the encoding of the given group variables (given via indices).
             *
             * @param ddGroupVariableIndices The indices of the variables that are used to distinguish the groups.
             * @return A vector of ADDs that are the separate groups (wrt. to the encoding of the given variables).
             */
            std::vector<InternalAdd<DdType::Sylvan, ValueType>> splitIntoGroups(std::vector<uint_fast64_t> const& ddGroupVariableIndices) const;
            
            /*!
             * Simultaneously splits the ADD and the given vector ADD into several ADDs that differ in the encoding of
             * the given group variables (given via indices).
             *
             * @param vector The vector to split (in addition to the current ADD).
             * @param ddGroupVariableIndices The indices of the variables that are used to distinguish the groups.
             * @return A vector of pairs of ADDs that are the separate groups of the current ADD and the vector,
             * respectively (wrt. to the encoding of the given variables).
             */
            std::vector<std::pair<InternalAdd<DdType::Sylvan, ValueType>, InternalAdd<DdType::Sylvan, ValueType>>> splitIntoGroups(InternalAdd<DdType::Sylvan, ValueType> vector, std::vector<uint_fast64_t> const& ddGroupVariableIndices) const;
            
            /*!
             * Translates the ADD into the components needed for constructing a matrix.
             *
             * @param rowGroupIndices The row group indices.
             * @param rowIndications The vector that is to be filled with the row indications.
             * @param columnsAndValues The vector that is to be filled with the non-zero entries of the matrix.
             * @param rowOdd The ODD used for translating the rows.
             * @param columnOdd The ODD used for translating the columns.
             * @param ddRowVariableIndices The variable indices of the row variables.
             * @param ddColumnVariableIndices The variable indices of the column variables.
             * @param writeValues A flag that indicates whether or not to write to the entry vector. If this is not set,
             * only the row indications are modified.
             */
            void toMatrixComponents(std::vector<uint_fast64_t> const& rowGroupIndices, std::vector<uint_fast64_t>& rowIndications, std::vector<storm::storage::MatrixEntry<uint_fast64_t, ValueType>>& columnsAndValues, Odd const& rowOdd, Odd const& columnOdd, std::vector<uint_fast64_t> const& ddRowVariableIndices, std::vector<uint_fast64_t> const& ddColumnVariableIndices, bool writeValues) const;
            
            /*!
             * Creates an ADD from the given explicit vector.
             *
             * @param ddManager The manager to use to built the ADD.
             * @param values The explicit vector to encode.
             * @param odd The ODD to use for the translation.
             * @param ddVariableIndices The indices of the variables to use in the ADD.
             */
            static InternalAdd<DdType::Sylvan, ValueType> fromVector(InternalDdManager<DdType::Sylvan> const* ddManager, std::vector<ValueType> const& values, storm::dd::Odd const& odd, std::vector<uint_fast64_t> const& ddVariableIndices);
            
            /*!
             * Creates an ODD based on the current ADD.
             *
             * @return The corresponding ODD.
             */
            Odd createOdd(std::vector<uint_fast64_t> const& ddVariableIndices) const;
            
        private:
            InternalDdManager<DdType::Sylvan> const* ddManager;
            
            sylvan::Mtbdd sylvanMtbdd;
        };
    }
}

#endif /* STORM_STORAGE_DD_CUDD_INTERNALSYLVANADD_H_ */