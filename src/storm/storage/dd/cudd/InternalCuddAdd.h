#ifndef STORM_STORAGE_DD_CUDD_INTERNALCUDDADD_H_
#define STORM_STORAGE_DD_CUDD_INTERNALCUDDADD_H_

#include <functional>
#include <memory>
#include <set>
#include <unordered_map>

#include "storm/adapters/RationalNumberAdapter.h"

#include "storm/storage/dd/DdType.h"
#include "storm/storage/dd/InternalAdd.h"
#include "storm/storage/dd/Odd.h"

#include "storm/storage/expressions/Variable.h"

// Include the C++-interface of CUDD.
#include "cuddObj.hh"

namespace storm {
namespace storage {
template<typename T>
class SparseMatrix;

class BitVector;

template<typename E, typename V>
class MatrixEntry;
}  // namespace storage

namespace dd {
template<DdType LibraryType>
class DdManager;

template<DdType LibraryType>
class InternalDdManager;

template<DdType LibraryType>
class InternalBdd;

template<DdType LibraryType, typename ValueType>
class AddIterator;

namespace bisimulation {
template<DdType LibraryType, typename ValueType>
class InternalSignatureRefiner;
}

template<typename ValueType>
class InternalAdd<DdType::CUDD, ValueType> {
   public:
    friend class InternalBdd<DdType::CUDD>;

    friend class bisimulation::InternalSignatureRefiner<DdType::CUDD, ValueType>;

    /*!
     * Creates an ADD that encapsulates the given CUDD ADD.
     *
     * @param ddManager The manager responsible for this DD.
     * @param cuddAdd The CUDD ADD to store.
     */
    InternalAdd(InternalDdManager<DdType::CUDD> const* ddManager, cudd::ADD cuddAdd);

    // Instantiate all copy/move constructors/assignments with the default implementation.
    InternalAdd() = default;
    InternalAdd(InternalAdd<DdType::CUDD, ValueType> const& other) = default;
    InternalAdd& operator=(InternalAdd<DdType::CUDD, ValueType> const& other) = default;
    InternalAdd(InternalAdd<DdType::CUDD, ValueType>&& other) = default;
    InternalAdd& operator=(InternalAdd<DdType::CUDD, ValueType>&& other) = default;
    virtual ~InternalAdd() = default;

    /*!
     * Retrieves whether the two DDs represent the same function.
     *
     * @param other The DD that is to be compared with the current one.
     * @return True if the DDs represent the same function.
     */
    bool operator==(InternalAdd<DdType::CUDD, ValueType> const& other) const;

    /*!
     * Retrieves whether the two DDs represent different functions.
     *
     * @param other The DD that is to be compared with the current one.
     * @return True if the DDs represent the different functions.
     */
    bool operator!=(InternalAdd<DdType::CUDD, ValueType> const& other) const;

    /*!
     * Adds the two ADDs.
     *
     * @param other The ADD to add to the current one.
     * @return The result of the addition.
     */
    InternalAdd<DdType::CUDD, ValueType> operator+(InternalAdd<DdType::CUDD, ValueType> const& other) const;

    /*!
     * Adds the given ADD to the current one.
     *
     * @param other The ADD to add to the current one.
     * @return A reference to the current ADD after the operation.
     */
    InternalAdd<DdType::CUDD, ValueType>& operator+=(InternalAdd<DdType::CUDD, ValueType> const& other);

    /*!
     * Multiplies the two ADDs.
     *
     * @param other The ADD to multiply with the current one.
     * @return The result of the multiplication.
     */
    InternalAdd<DdType::CUDD, ValueType> operator*(InternalAdd<DdType::CUDD, ValueType> const& other) const;

    /*!
     * Multiplies the given ADD with the current one and assigns the result to the current ADD.
     *
     * @param other The ADD to multiply with the current one.
     * @return A reference to the current ADD after the operation.
     */
    InternalAdd<DdType::CUDD, ValueType>& operator*=(InternalAdd<DdType::CUDD, ValueType> const& other);

    /*!
     * Subtracts the given ADD from the current one.
     *
     * @param other The ADD to subtract from the current one.
     * @return The result of the subtraction.
     */
    InternalAdd<DdType::CUDD, ValueType> operator-(InternalAdd<DdType::CUDD, ValueType> const& other) const;

    /*!
     * Subtracts the given ADD from the current one and assigns the result to the current ADD.
     *
     * @param other The ADD to subtract from the current one.
     * @return A reference to the current ADD after the operation.
     */
    InternalAdd<DdType::CUDD, ValueType>& operator-=(InternalAdd<DdType::CUDD, ValueType> const& other);

    /*!
     * Divides the current ADD by the given one.
     *
     * @param other The ADD by which to divide the current one.
     * @return The result of the division.
     */
    InternalAdd<DdType::CUDD, ValueType> operator/(InternalAdd<DdType::CUDD, ValueType> const& other) const;

    /*!
     * Divides the current ADD by the given one and assigns the result to the current ADD.
     *
     * @param other The ADD by which to divide the current one.
     * @return A reference to the current ADD after the operation.
     */
    InternalAdd<DdType::CUDD, ValueType>& operator/=(InternalAdd<DdType::CUDD, ValueType> const& other);

    /*!
     * Retrieves the function that maps all evaluations to one that have identical function values.
     *
     * @param other The ADD with which to perform the operation.
     * @return The resulting function represented as an ADD.
     */
    InternalBdd<DdType::CUDD> equals(InternalAdd<DdType::CUDD, ValueType> const& other) const;

    /*!
     * Retrieves the function that maps all evaluations to one that have distinct function values.
     *
     * @param other The ADD with which to perform the operation.
     * @return The resulting function represented as an ADD.
     */
    InternalBdd<DdType::CUDD> notEquals(InternalAdd<DdType::CUDD, ValueType> const& other) const;

    /*!
     * Retrieves the function that maps all evaluations to one whose function value in the first ADD are less
     * than the one in the given ADD.
     *
     * @param other The ADD with which to perform the operation.
     * @return The resulting function represented as an ADD.
     */
    InternalBdd<DdType::CUDD> less(InternalAdd<DdType::CUDD, ValueType> const& other) const;

    /*!
     * Retrieves the function that maps all evaluations to one whose function value in the first ADD are less or
     * equal than the one in the given ADD.
     *
     * @param other The DD with which to perform the operation.
     * @return The resulting function represented as an ADD.
     */
    InternalBdd<DdType::CUDD> lessOrEqual(InternalAdd<DdType::CUDD, ValueType> const& other) const;

    /*!
     * Retrieves the function that maps all evaluations to one whose function value in the first ADD are greater
     * than the one in the given ADD.
     *
     * @param other The ADD with which to perform the operation.
     * @return The resulting function represented as an ADD.
     */
    InternalBdd<DdType::CUDD> greater(InternalAdd<DdType::CUDD, ValueType> const& other) const;

    /*!
     * Retrieves the function that maps all evaluations to one whose function value in the first ADD are greater
     * or equal than the one in the given ADD.
     *
     * @param other The ADD with which to perform the operation.
     * @return The resulting function represented as an ADD.
     */
    InternalBdd<DdType::CUDD> greaterOrEqual(InternalAdd<DdType::CUDD, ValueType> const& other) const;

    /*!
     * Retrieves the function that represents the current ADD to the power of the given ADD.
     *
     * @other The exponent function (given as an ADD).
     * @retur The resulting ADD.
     */
    InternalAdd<DdType::CUDD, ValueType> pow(InternalAdd<DdType::CUDD, ValueType> const& other) const;

    /*!
     * Retrieves the function that represents the current ADD modulo the given ADD.
     *
     * @other The modul function (given as an ADD).
     * @retur The resulting ADD.
     */
    InternalAdd<DdType::CUDD, ValueType> mod(InternalAdd<DdType::CUDD, ValueType> const& other) const;

    /*!
     * Retrieves the function that represents the logarithm of the current ADD to the bases given by the second
     * ADD.
     *
     * @other The base function (given as an ADD).
     * @retur The resulting ADD.
     */
    InternalAdd<DdType::CUDD, ValueType> logxy(InternalAdd<DdType::CUDD, ValueType> const& other) const;

    /*!
     * Retrieves the function that floors all values in the current ADD.
     *
     * @retur The resulting ADD.
     */
    InternalAdd<DdType::CUDD, ValueType> floor() const;

    /*!
     * Retrieves the function that ceils all values in the current ADD.
     *
     * @retur The resulting ADD.
     */
    InternalAdd<DdType::CUDD, ValueType> ceil() const;

    /*!
     * Retrieves the function that sharpens all values in the current ADD with the Kwek-Mehlhorn algorithm.
     *
     * @return The resulting ADD.
     */
    InternalAdd<DdType::CUDD, storm::RationalNumber> sharpenKwekMehlhorn(size_t precision) const;

    /*!
     * Retrieves the function that maps all evaluations to the minimum of the function values of the two ADDs.
     *
     * @param other The ADD with which to perform the operation.
     * @return The resulting function represented as an ADD.
     */
    InternalAdd<DdType::CUDD, ValueType> minimum(InternalAdd<DdType::CUDD, ValueType> const& other) const;

    /*!
     * Retrieves the function that maps all evaluations to the maximum of the function values of the two ADDs.
     *
     * @param other The ADD with which to perform the operation.
     * @return The resulting function represented as an ADD.
     */
    InternalAdd<DdType::CUDD, ValueType> maximum(InternalAdd<DdType::CUDD, ValueType> const& other) const;

    /*!
     * Replaces the leaves in this MTBDD with the converted values in the target value type.
     *
     * @return The resulting function represented as an ADD.
     */
    template<typename TargetValueType>
    typename std::enable_if<std::is_same<ValueType, TargetValueType>::value, InternalAdd<DdType::CUDD, TargetValueType>>::type toValueType() const;
    template<typename TargetValueType>
    typename std::enable_if<!std::is_same<ValueType, TargetValueType>::value, InternalAdd<DdType::CUDD, TargetValueType>>::type toValueType() const;

    /*!
     * Sum-abstracts from the given cube.
     *
     * @param cube The cube from which to abstract.
     */
    InternalAdd<DdType::CUDD, ValueType> sumAbstract(InternalBdd<DdType::CUDD> const& cube) const;

    /*!
     * Min-abstracts from the given cube.
     *
     * @param cube The cube from which to abstract.
     */
    InternalAdd<DdType::CUDD, ValueType> minAbstract(InternalBdd<DdType::CUDD> const& cube) const;

    /*!
     * Min-abstracts from the given cube and returns a representative.
     *
     * @param cube The cube from which to abstract.
     */
    InternalBdd<DdType::CUDD> minAbstractRepresentative(InternalBdd<DdType::CUDD> const& cube) const;

    /*!
     * Max-abstracts from the given cube.
     *
     * @param cube The cube from which to abstract.
     */
    InternalAdd<DdType::CUDD, ValueType> maxAbstract(InternalBdd<DdType::CUDD> const& cube) const;

    /*!
     * Max-abstracts from the given cube and returns a representative.
     *
     * @param cube The cube from which to abstract.
     */
    InternalBdd<DdType::CUDD> maxAbstractRepresentative(InternalBdd<DdType::CUDD> const& cube) const;

    /*!
     * Checks whether the current and the given ADD represent the same function modulo some given precision.
     *
     * @param other The ADD with which to compare.
     * @param precision An upper bound on the maximal difference between any two function values that is to be
     * tolerated.
     * @param relative If set to true, not the absolute values have to be within the precision, but the relative
     * values.
     */
    bool equalModuloPrecision(InternalAdd<DdType::CUDD, ValueType> const& other, ValueType const& precision, bool relative = true) const;

    /*!
     * Swaps the given pairs of DD variables in the ADD. The pairs of meta variables have to be represented by
     * vectors of BDDs must have equal length.
     *
     * @param from The vector that specifies the 'from' part of the variable renaming.
     * @param to The vector that specifies the 'to' part of the variable renaming.
     * @return The resulting ADD.
     */
    InternalAdd<DdType::CUDD, ValueType> swapVariables(std::vector<InternalBdd<DdType::CUDD>> const& from,
                                                       std::vector<InternalBdd<DdType::CUDD>> const& to) const;

    /*!
     * Permutes the given pairs of DD variables in the ADD. The pairs of meta variables have to be represented by
     * ADDs must have equal length.
     *
     * @param from The vector that specifies the 'from' part of the variable renaming.
     * @param to The vector that specifies the 'to' part of the variable renaming.
     * @return The resulting ADD.
     */
    InternalAdd<DdType::CUDD, ValueType> permuteVariables(std::vector<InternalBdd<DdType::CUDD>> const& from,
                                                          std::vector<InternalBdd<DdType::CUDD>> const& to) const;

    /*!
     * Multiplies the current ADD (representing a matrix) with the given matrix by summing over the given meta
     * variables.
     *
     * @param otherMatrix The matrix with which to multiply.
     * @param summationDdVariables The DD variables (represented as ADDs) over which to sum.
     * @return An ADD representing the result of the matrix-matrix multiplication.
     */
    InternalAdd<DdType::CUDD, ValueType> multiplyMatrix(InternalAdd<DdType::CUDD, ValueType> const& otherMatrix,
                                                        std::vector<InternalBdd<DdType::CUDD>> const& summationDdVariables) const;

    /*!
     * Multiplies the current ADD (representing a matrix) with the given matrix by summing over the given meta
     * variables.
     *
     * @param otherMatrix The matrix with which to multiply.
     * @param summationDdVariables The DD variables (represented as ADDs) over which to sum.
     * @return An ADD representing the result of the matrix-matrix multiplication.
     */
    InternalAdd<DdType::CUDD, ValueType> multiplyMatrix(InternalBdd<DdType::CUDD> const& otherMatrix,
                                                        std::vector<InternalBdd<DdType::CUDD>> const& summationDdVariables) const;

    /*!
     * Computes a BDD that represents the function in which all assignments with a function value strictly
     * larger than the given value are mapped to one and all others to zero.
     *
     * @param value The value used for the comparison.
     * @return The resulting BDD.
     */
    InternalBdd<DdType::CUDD> greater(ValueType const& value) const;

    /*!
     * Computes a BDD that represents the function in which all assignments with a function value larger or equal
     * to the given value are mapped to one and all others to zero.
     *
     * @param value The value used for the comparison.
     * @return The resulting BDD.
     */
    InternalBdd<DdType::CUDD> greaterOrEqual(ValueType const& value) const;

    /*!
     * Computes a BDD that represents the function in which all assignments with a function value strictly
     * lower than the given value are mapped to one and all others to zero.
     *
     * @param value The value used for the comparison.
     * @return The resulting BDD.
     */
    InternalBdd<DdType::CUDD> less(ValueType const& value) const;

    /*!
     * Computes a BDD that represents the function in which all assignments with a function value less or equal
     * to the given value are mapped to one and all others to zero.
     *
     * @param value The value used for the comparison.
     * @return The resulting BDD.
     */
    InternalBdd<DdType::CUDD> lessOrEqual(ValueType const& value) const;

    /*!
     * Computes a BDD that represents the function in which all assignments with a function value unequal to
     * zero are mapped to one and all others to zero.
     *
     * @return The resulting DD.
     */
    InternalBdd<DdType::CUDD> notZero() const;

    /*!
     * Computes the constraint of the current ADD with the given constraint. That is, the function value of the
     * resulting ADD will be the same as the current ones for all assignments mapping to one in the constraint
     * and may be different otherwise.
     *
     * @param constraint The constraint to use for the operation.
     * @return The resulting ADD.
     */
    InternalAdd<DdType::CUDD, ValueType> constrain(InternalAdd<DdType::CUDD, ValueType> const& constraint) const;

    /*!
     * Computes the restriction of the current ADD with the given constraint. That is, the function value of the
     * resulting DD will be the same as the current ones for all assignments mapping to one in the constraint
     * and may be different otherwise.
     *
     * @param constraint The constraint to use for the operation.
     * @return The resulting ADD.
     */
    InternalAdd<DdType::CUDD, ValueType> restrict(InternalAdd<DdType::CUDD, ValueType> const& constraint) const;

    /*!
     * Retrieves the support of the current ADD.
     *
     * @return The support represented as a BDD.
     */
    InternalBdd<DdType::CUDD> getSupport() const;

    /*!
     * Retrieves the number of encodings that are mapped to a non-zero value.
     *
     * @param numberOfDdVariables The number of DD variables contained in this BDD.
     * @return The number of encodings that are mapped to a non-zero value.
     */
    uint_fast64_t getNonZeroCount(uint_fast64_t numberOfDdVariables) const;

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
     * Retrieves the value of this ADD that is required to be a leaf.
     *
     * @return The value of the leaf.
     */
    ValueType getValue() const;

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
     * Retrieves the index of the topmost variable in the ADD.
     *
     * @return The index of the topmost variable in ADD.
     */
    uint_fast64_t getIndex() const;

    /*!
     * Retrieves the level of the topmost variable in the ADD.
     *
     * @return The level of the topmost variable in ADD.
     */
    uint_fast64_t getLevel() const;

    /*!
     * Exports the DD to the given file in the dot format.
     *
     * @param filename The name of the file to which the DD is to be exported.
     * @param ddVariableNamesAsString The names of the DD variables to display in the dot file.
     */
    void exportToDot(std::string const& filename, std::vector<std::string> const& ddVariableNamesAsStrings, bool showVariablesIfPossible = true) const;

    /*!
     * Exports the DD to the given file in a textual format as specified in Sylvan.
     *
     * @param filename The name of the file to which the DD is to be exported.
     */
    void exportToText(std::string const& filename) const;
    /*!
     * Retrieves an iterator that points to the first meta variable assignment with a non-zero function value.
     *
     * @param fullDdManager The DD manager responsible for this ADD.
     * @param variableCube The cube of variables contained in this ADD.
     * @param numberOfDdVariables The number of variables contained in this ADD.
     * @param metaVariables The meta variables contained in the ADD.
     * @param enumerateDontCareMetaVariables If set to true, all meta variable assignments are enumerated, even
     * if a meta variable does not at all influence the the function value.
     * @return An iterator that points to the first meta variable assignment with a non-zero function value.
     */
    AddIterator<DdType::CUDD, ValueType> begin(DdManager<DdType::CUDD> const& fullDdManager, InternalBdd<DdType::CUDD> const& variableCube,
                                               uint_fast64_t numberOfDdVariables, std::set<storm::expressions::Variable> const& metaVariables,
                                               bool enumerateDontCareMetaVariables = true) const;

    /*!
     * Retrieves an iterator that points past the end of the container.
     *
     * @param fullDdManager The DD manager responsible for this ADD.
     * @return An iterator that points past the end of the container.
     */
    AddIterator<DdType::CUDD, ValueType> end(DdManager<DdType::CUDD> const& fullDdManager) const;

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
    void composeWithExplicitVector(Odd const& odd, std::vector<uint_fast64_t> const& ddVariableIndices, std::vector<ValueType>& targetVector,
                                   std::function<ValueType(ValueType const&, ValueType const&)> const& function) const;

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
    void forEach(Odd const& odd, std::vector<uint_fast64_t> const& ddVariableIndices,
                 std::function<void(uint64_t const&, ValueType const&)> const& function) const;

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
    void composeWithExplicitVector(Odd const& odd, std::vector<uint_fast64_t> const& ddVariableIndices, std::vector<uint_fast64_t> const& offsets,
                                   std::vector<ValueType>& targetVector, std::function<ValueType(ValueType const&, ValueType const&)> const& function) const;

    /*!
     * Splits the ADD into several ADDs that differ in the encoding of the given group variables (given via indices).
     *
     * @param ddGroupVariableIndices The indices of the variables that are used to distinguish the groups.
     * @return A vector of ADDs that are the separate groups (wrt. to the encoding of the given variables).
     */
    std::vector<InternalAdd<DdType::CUDD, ValueType>> splitIntoGroups(std::vector<uint_fast64_t> const& ddGroupVariableIndices) const;

    /*!
     * Splits the ADD into several ADDs that differ in the encoding of the given group variables (given via indices).
     * The labeling is then made by interpreting the group encodings as binary encodings.
     *
     * @param ddGroupVariableIndices The indices of the variables that are used to distinguish the groups.
     * @param ddLabelVariableIndices The indices of variables that are considered as labels.
     * @return A vector of ADDs that are the separate groups (wrt. to the encoding of the given variables).
     */
    std::vector<uint64_t> decodeGroupLabels(std::vector<uint_fast64_t> const& ddGroupVariableIndices,
                                            storm::storage::BitVector const& ddLabelVariableIndices) const;

    /*!
     * Simultaneously splits the ADD and the given vector ADD into several ADDs that differ in the encoding of
     * the given group variables (given via indices).
     *
     * @param vector The vector to split (in addition to the current ADD).
     * @param ddGroupVariableIndices The indices of the variables that are used to distinguish the groups.
     * @return A vector of pairs of ADDs that are the separate groups of the current ADD and the vector,
     * respectively (wrt. to the encoding of the given variables).
     */
    std::vector<std::pair<InternalAdd<DdType::CUDD, ValueType>, InternalAdd<DdType::CUDD, ValueType>>> splitIntoGroups(
        InternalAdd<DdType::CUDD, ValueType> vector, std::vector<uint_fast64_t> const& ddGroupVariableIndices) const;

    /*!
     * Simultaneously splits the ADD and the given vector ADDs into several ADDs that differ in the encoding of
     * the given group variables (given via indices).
     *
     * @param vectors The vectors to split (in addition to the current ADD).
     * @param ddGroupVariableIndices The indices of the variables that are used to distinguish the groups.
     * @return A vector of vectors of ADDs such that result.size() is the number of groups and result[i] refers to all ADDs within the same group i (wrt. to the
     * encoding of the given variables). result[i].back() always refers to this ADD.
     */
    std::vector<std::vector<InternalAdd<DdType::CUDD, ValueType>>> splitIntoGroups(std::vector<InternalAdd<DdType::CUDD, ValueType>> const& vectors,
                                                                                   std::vector<uint_fast64_t> const& ddGroupVariableIndices) const;

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
    void toMatrixComponents(std::vector<uint_fast64_t> const& rowGroupIndices, std::vector<uint_fast64_t>& rowIndications,
                            std::vector<storm::storage::MatrixEntry<uint_fast64_t, ValueType>>& columnsAndValues, Odd const& rowOdd, Odd const& columnOdd,
                            std::vector<uint_fast64_t> const& ddRowVariableIndices, std::vector<uint_fast64_t> const& ddColumnVariableIndices,
                            bool writeValues) const;

    /*!
     * Creates an ADD from the given explicit vector.
     *
     * @param ddManager The manager to use to built the ADD.
     * @param values The explicit vector to encode.
     * @param odd The ODD to use for the translation.
     * @param ddVariableIndices The indices of the variables to use in the ADD.
     */
    static InternalAdd<DdType::CUDD, ValueType> fromVector(InternalDdManager<DdType::CUDD> const* ddManager, std::vector<ValueType> const& values,
                                                           storm::dd::Odd const& odd, std::vector<uint_fast64_t> const& ddVariableIndices);

    /*!
     * Creates an ODD based on the current ADD.
     *
     * @return The corresponding ODD.
     */
    Odd createOdd(std::vector<uint_fast64_t> const& ddVariableIndices) const;

    InternalDdManager<DdType::CUDD> const& getInternalDdManager() const;

    /*!
     * Retrieves the CUDD ADD object associated with this ADD.
     *
     * @return The CUDD ADD object associated with this ADD.
     */
    cudd::ADD getCuddAdd() const;

    /*!
     * Retrieves the raw DD node of CUDD associated with this ADD.
     *
     * @return The DD node of CUDD associated with this ADD.
     */
    DdNode* getCuddDdNode() const;

    /*!
     * Retrieves a string representation of an ID for thid ADD.
     */
    std::string getStringId() const;

   private:
    /*!
     * Performs a recursive step for forEach.
     *
     * @param dd The DD to traverse.
     * @param currentLevel The currently considered level in the DD.
     * @param maxLevel The number of levels that need to be considered.
     * @param currentOffset The current offset.
     * @param odd The ODD used for the translation.
     * @param ddVariableIndices The (sorted) indices of all DD variables that need to be considered.
     * @param function The callback invoked for every element. The first argument is the offset and the second
     * is the value.
     */
    void forEachRec(DdNode const* dd, uint_fast64_t currentLevel, uint_fast64_t maxLevel, uint_fast64_t currentOffset, Odd const& odd,
                    std::vector<uint_fast64_t> const& ddVariableIndices, std::function<void(uint64_t const&, ValueType const&)> const& function) const;

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
    void splitIntoGroupsRec(DdNode* dd, std::vector<InternalAdd<DdType::CUDD, ValueType>>& groups, std::vector<uint_fast64_t> const& ddGroupVariableIndices,
                            uint_fast64_t currentLevel, uint_fast64_t maxLevel) const;

    /*!
     * Splits the given matrix DD into the labelings of the gropus using the given group variables.
     *
     * @param dd The DD to split.
     * @param labels A vector that is to be filled with the labels of the individual groups.
     * @param ddGroupVariableIndices The (sorted) indices of all DD group variables that need to be considered.
     * @param ddLabelVariableIndices A bit vector indicating which variables are considered label variables.
     * @param currentLevel The currently considered level in the DD.
     * @param maxLevel The number of levels that need to be considered.
     * @param remainingMetaVariables The meta variables that remain in the DDs after the groups have been split.
     * @param label The currently followed label.
     */
    void decodeGroupLabelsRec(DdNode* dd, std::vector<uint64_t>& labels, std::vector<uint_fast64_t> const& ddGroupVariableIndices,
                              storm::storage::BitVector const& ddLabelVariableIndices, uint_fast64_t currentLevel, uint_fast64_t maxLevel,
                              uint64_t label) const;

    /*!
     * Splits the given DDs into the groups using the given group variables.
     *
     * @param dd1 The first DD to split.
     * @param dd2 The second DD to split.
     * @param groups A vector that is to be filled with the DDs for the individual groups.
     * @param ddGroupVariableIndices The (sorted) indices of all DD group variables that need to be considered.
     * @param currentLevel The currently considered level in the DD.
     * @param maxLevel The number of levels that need to be considered.
     * @param remainingMetaVariables The meta variables that remain in the DDs after the groups have been split.
     */
    void splitIntoGroupsRec(DdNode* dd1, DdNode* dd2,
                            std::vector<std::pair<InternalAdd<DdType::CUDD, ValueType>, InternalAdd<DdType::CUDD, ValueType>>>& groups,
                            std::vector<uint_fast64_t> const& ddGroupVariableIndices, uint_fast64_t currentLevel, uint_fast64_t maxLevel) const;

    /*!
     * Splits the given DDs into the groups using the given group variables.
     *
     * @param dds The DDs to split.
     * @param negatedDds indicates which of the DDs need to be interpreted as negated.
     * @param groups A vector that is to be filled with the DDs for the individual groups.
     * @param ddGroupVariableIndices The (sorted) indices of all DD group variables that need to be considered.
     * @param currentLevel The currently considered level in the DD.
     * @param maxLevel The number of levels that need to be considered.
     */
    void splitIntoGroupsRec(std::vector<DdNode*> const& dds, std::vector<std::vector<InternalAdd<DdType::CUDD, ValueType>>>& groups,
                            std::vector<uint_fast64_t> const& ddGroupVariableIndices, uint_fast64_t currentLevel, uint_fast64_t maxLevel) const;

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
    void toMatrixComponentsRec(DdNode const* dd, std::vector<uint_fast64_t> const& rowGroupOffsets, std::vector<uint_fast64_t>& rowIndications,
                               std::vector<storm::storage::MatrixEntry<uint_fast64_t, ValueType>>& columnsAndValues, Odd const& rowOdd, Odd const& columnOdd,
                               uint_fast64_t currentRowLevel, uint_fast64_t currentColumnLevel, uint_fast64_t maxLevel, uint_fast64_t currentRowOffset,
                               uint_fast64_t currentColumnOffset, std::vector<uint_fast64_t> const& ddRowVariableIndices,
                               std::vector<uint_fast64_t> const& ddColumnVariableIndices, bool writeValues) const;

    /*!
     * Builds an ADD representing the given vector.
     *
     * @param manager The manager responsible for the ADD.
     * @param currentOffset The current offset in the vector.
     * @param currentLevel The current level in the DD.
     * @param maxLevel The maximal level in the DD.
     * @param values The vector that is to be represented by the ADD.
     * @param odd The ODD used for the translation.
     * @param ddVariableIndices The (sorted) list of DD variable indices to use.
     * @return The resulting (CUDD) ADD node.
     */
    static DdNode* fromVectorRec(::DdManager* manager, uint_fast64_t& currentOffset, uint_fast64_t currentLevel, uint_fast64_t maxLevel,
                                 std::vector<ValueType> const& values, Odd const& odd, std::vector<uint_fast64_t> const& ddVariableIndices);

    /*!
     * Recursively builds the ODD from an ADD (that has no complement edges).
     *
     * @param dd The DD for which to build the ODD.
     * @param manager The manager responsible for the DD.
     * @param currentLevel The currently considered level in the DD.
     * @param maxLevel The number of levels that need to be considered.
     * @param ddVariableIndices The (sorted) indices of all DD variables that need to be considered.
     * @param uniqueTableForLevels A vector of unique tables, one for each level to be considered, that keeps
     * ODD nodes for the same DD and level unique.
     * @return A pointer to the constructed ODD for the given arguments.
     */
    static std::shared_ptr<Odd> createOddRec(DdNode* dd, cudd::Cudd const& manager, uint_fast64_t currentLevel, uint_fast64_t maxLevel,
                                             std::vector<uint_fast64_t> const& ddVariableIndices,
                                             std::vector<std::unordered_map<DdNode*, std::shared_ptr<Odd>>>& uniqueTableForLevels);

    InternalDdManager<DdType::CUDD> const* ddManager;

    cudd::ADD cuddAdd;
};
}  // namespace dd
}  // namespace storm

#endif /* STORM_STORAGE_DD_CUDD_INTERNALCUDDADD_H_ */
