#ifndef STORM_STORAGE_DD_CUDDDD_H_
#define STORM_STORAGE_DD_CUDDDD_H_

#include <map>
#include <set>
#include <memory>
#include <iostream>

#include "src/storage/dd/Dd.h"
#include "src/storage/dd/CuddDdForwardIterator.h"
#include "src/utility/OsDetection.h"

// Include the C++-interface of CUDD.
#include "cuddObj.hh"

namespace storm {
    namespace dd {
        // Forward-declare the DdManager class.
        template<DdType Type> class DdManager;
        
        template<>
        class Dd<DdType::CUDD> {
        public:
            // Declare the DdManager and DdIterator class as friend so it can access the internals of a DD.
            friend class DdManager<DdType::CUDD>;
            friend class DdForwardIterator<DdType::CUDD>;
            
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
            void existsAbstract(std::set<std::string> const& metaVariableNames);
            
            /*!
             * Universally abstracts from the given meta variables.
             *
             * @param metaVariableNames The names of all meta variables from which to abstract.
             */
            void universalAbstract(std::set<std::string> const& metaVariableNames);
            
            /*!
             * Sum-abstracts from the given meta variables.
             *
             * @param metaVariableNames The names of all meta variables from which to abstract.
             */
            void sumAbstract(std::set<std::string> const& metaVariableNames);
            
            /*!
             * Min-abstracts from the given meta variables.
             *
             * @param metaVariableNames The names of all meta variables from which to abstract.
             */
            void minAbstract(std::set<std::string> const& metaVariableNames);
            
            /*!
             * Max-abstracts from the given meta variables.
             *
             * @param metaVariableNames The names of all meta variables from which to abstract.
             */
            void maxAbstract(std::set<std::string> const& metaVariableNames);
            
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
             * Creates a DD that encapsulates the given CUDD ADD.
             *
             * @param ddManager The manager responsible for this DD.
             * @param cuddAdd The CUDD ADD to store.
             * @param
             */
            Dd(std::shared_ptr<DdManager<DdType::CUDD>> ddManager, ADD cuddAdd, std::set<std::string> const& containedMetaVariableNames = std::set<std::string>());
            
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