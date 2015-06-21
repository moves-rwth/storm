#ifndef STORM_STORAGE_DD_CUDDBDD_H_
#define STORM_STORAGE_DD_CUDDBDD_H_

#include "src/logic/ComparisonType.h"
#include "src/storage/dd/Bdd.h"
#include "src/storage/dd/CuddDd.h"
#include "src/storage/dd/CuddAdd.h"
#include "src/storage/expressions/Variable.h"
#include "src/utility/OsDetection.h"

// Include the C++-interface of CUDD.
#include "cuddObj.hh"

namespace storm {
    namespace dd {
        // Forward-declare some classes.
        template<DdType Type> class DdManager;
        template<DdType Type> class Odd;
        template<DdType Type> class Add;
        
        template<>
        class Bdd<DdType::CUDD> : public Dd<DdType::CUDD> {
        public:
            /*!
             * Constructs a BDD representation of all encodings that are in the requested relation with the given value.
             *
             * @param ddManager The DD manager responsible for the resulting BDD.
             * @param explicitValues The explicit values to compare to the given value.
             * @param odd The ODD used for the translation from the explicit representation to a symbolic one.
             * @param metaVariables The meta variables to use for the symbolic encoding.
             * @param comparisonType The relation that needs to hold for the values (wrt. to the given value).
             * @param value The value to compare with.
             */
            Bdd(std::shared_ptr<DdManager<DdType::CUDD> const> ddManager, std::vector<double> const& explicitValues, storm::dd::Odd<DdType::CUDD> const& odd, std::set<storm::expressions::Variable> const& metaVariables, storm::logic::ComparisonType comparisonType, double value);
            
            // Declare the DdManager and DdIterator class as friend so it can access the internals of a DD.
            friend class DdManager<DdType::CUDD>;
            friend class DdForwardIterator<DdType::CUDD>;
            friend class Add<DdType::CUDD>;
            friend class Odd<DdType::CUDD>;
            
            // Instantiate all copy/move constructors/assignments with the default implementation.
            Bdd() = default;
            Bdd(Bdd<DdType::CUDD> const& other) = default;
            Bdd& operator=(Bdd<DdType::CUDD> const& other) = default;
#ifndef WINDOWS
            Bdd(Bdd<DdType::CUDD>&& other) = default;
            Bdd& operator=(Bdd<DdType::CUDD>&& other) = default;
#endif
            
            /*!
             * Retrieves whether the two BDDs represent the same function.
             *
             * @param other The BDD that is to be compared with the current one.
             * @return True if the BDDs represent the same function.
             */
            bool operator==(Bdd<DdType::CUDD> const& other) const;
            
            /*!
             * Retrieves whether the two BDDs represent different functions.
             *
             * @param other The BDD that is to be compared with the current one.
             * @return True if the BDDs represent the different functions.
             */
            bool operator!=(Bdd<DdType::CUDD> const& other) const;
            
            /*!
             * Performs an if-then-else with the given operands, i.e. maps all valuations that are mapped to a non-zero
             * function value to the function values specified by the first DD and all others to the function values
             * specified by the second DD.
             *
             * @param thenBdd The BDD defining the 'then' part.
             * @param elseBdd The BDD defining the 'else' part.
             * @return The resulting BDD.
             */
            Bdd<DdType::CUDD> ite(Bdd<DdType::CUDD> const& thenBdd, Bdd<DdType::CUDD> const& elseBdd) const;
            
            /*!
             * Performs a logical or of the current and the given BDD.
             *
             * @param other The second BDD used for the operation.
             * @return The logical or of the operands.
             */
            Bdd<DdType::CUDD> operator||(Bdd<DdType::CUDD> const& other) const;
            
            /*!
             * Performs a logical or of the current and the given BDD and assigns it to the current BDD.
             *
             * @param other The second BDD used for the operation.
             * @return A reference to the current BDD after the operation
             */
            Bdd<DdType::CUDD>& operator|=(Bdd<DdType::CUDD> const& other);
            
            /*!
             * Performs a logical and of the current and the given BDD.
             *
             * @param other The second BDD used for the operation.
             * @return The logical and of the operands.
             */
            Bdd<DdType::CUDD> operator&&(Bdd<DdType::CUDD> const& other) const;
            
            /*!
             * Performs a logical and of the current and the given BDD and assigns it to the current BDD.
             *
             * @param other The second BDD used for the operation.
             * @return A reference to the current BDD after the operation
             */
            Bdd<DdType::CUDD>& operator&=(Bdd<DdType::CUDD> const& other);
            
            /*!
             * Performs a logical iff of the current and the given BDD.
             *
             * @param other The second BDD used for the operation.
             * @return The logical iff of the operands.
             */
            Bdd<DdType::CUDD> iff(Bdd<DdType::CUDD> const& other) const;
            
            /*!
             * Performs a logical exclusive-or of the current and the given BDD.
             *
             * @param other The second BDD used for the operation.
             * @return The logical exclusive-or of the operands.
             */
            Bdd<DdType::CUDD> exclusiveOr(Bdd<DdType::CUDD> const& other) const;
            
            /*!
             * Performs a logical implication of the current and the given BDD.
             *
             * @param other The second BDD used for the operation.
             * @return The logical implication of the operands.
             */
            Bdd<DdType::CUDD> implies(Bdd<DdType::CUDD> const& other) const;
            
            /*!
             * Logically inverts the current BDD.
             *
             * @return The resulting BDD.
             */
            Bdd<DdType::CUDD> operator!() const;
            
            /*!
             * Logically complements the current BDD.
             *
             * @return A reference to the current BDD after the operation.
             */
            Bdd<DdType::CUDD>& complement();
            
            /*!
             * Existentially abstracts from the given meta variables.
             *
             * @param metaVariables The meta variables from which to abstract.
             */
            Bdd<DdType::CUDD> existsAbstract(std::set<storm::expressions::Variable> const& metaVariables) const;
            
            /*!
             * Universally abstracts from the given meta variables.
             *
             * @param metaVariables The meta variables from which to abstract.
             */
            Bdd<DdType::CUDD> universalAbstract(std::set<storm::expressions::Variable> const& metaVariables) const;
            
            /*!
             * Swaps the given pairs of meta variables in the BDD. The pairs of meta variables must be guaranteed to have
             * the same number of underlying BDD variables.
             *
             * @param metaVariablePairs A vector of meta variable pairs that are to be swapped for one another.
             * @return The resulting BDD.
             */
            Bdd<DdType::CUDD> swapVariables(std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& metaVariablePairs) const;

            /*!
             * Computes the logical and of the current and the given BDD and existentially abstracts from the given set
             * of variables.
             *
             * @param other The second BDD for the logical and.
             * @param existentialVariables The variables from which to existentially abstract.
             * @return A BDD representing the result.
             */
            Bdd<DdType::CUDD> andExists(Bdd<DdType::CUDD> const& other, std::set<storm::expressions::Variable> const& existentialVariables) const;
            
            /*!
             * Computes the constraint of the current BDD with the given constraint. That is, the function value of the
             * resulting BDD will be the same as the current ones for all assignments mapping to one in the constraint
             * and may be different otherwise.
             *
             * @param constraint The constraint to use for the operation.
             * @return The resulting BDD.
             */
            Bdd<DdType::CUDD> constrain(Bdd<DdType::CUDD> const& constraint) const;
            
            /*!
             * Computes the restriction of the current BDD with the given constraint. That is, the function value of the
             * resulting DD will be the same as the current ones for all assignments mapping to one in the constraint
             * and may be different otherwise.
             *
             * @param constraint The constraint to use for the operation.
             * @return The resulting BDD.
             */
            Bdd<DdType::CUDD> restrict(Bdd<DdType::CUDD> const& constraint) const;
            
            /*!
             * Retrieves the support of the current BDD.
             *
             * @return The support represented as a BDD.
             */
            virtual Bdd<DdType::CUDD> getSupport() const override;
            
            /*!
             * Retrieves the number of encodings that are mapped to a non-zero value.
             *
             * @return The number of encodings that are mapped to a non-zero value.
             */
            virtual uint_fast64_t getNonZeroCount() const override;
            
            /*!
             * Retrieves the number of leaves of the DD.
             *
             * @return The number of leaves of the DD.
             */
            virtual uint_fast64_t getLeafCount() const override;
            
            /*!
             * Retrieves the number of nodes necessary to represent the DD.
             *
             * @return The number of nodes in this DD.
             */
            virtual uint_fast64_t getNodeCount() const override;
            
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
             * Retrieves the index of the topmost variable in the BDD.
             *
             * @return The index of the topmost variable in BDD.
             */
            virtual uint_fast64_t getIndex() const override;
            
            /*!
             * Exports the BDD to the given file in the dot format.
             *
             * @param filename The name of the file to which the BDD is to be exported.
             */
            virtual void exportToDot(std::string const& filename = "") const override;
            
            friend std::ostream & operator<<(std::ostream& out, const Bdd<DdType::CUDD>& bdd);
            
            /*!
             * Converts a BDD to an equivalent ADD.
             *
             * @return The corresponding ADD.
             */
            Add<DdType::CUDD> toAdd() const;
            
            /*!
             * Converts the BDD to a bit vector. The given offset-labeled DD is used to determine the correct row of
             * each entry.
             *
             * @param rowOdd The ODD used for determining the correct row.
             * @return The bit vector that is represented by this BDD.
             */
            storm::storage::BitVector toVector(storm::dd::Odd<DdType::CUDD> const& rowOdd) const;
            
        private:
            /*!
             * Retrieves the CUDD BDD object associated with this DD.
             *
             * @return The CUDD BDD object associated with this DD.
             */
            BDD getCuddBdd() const;
            
            /*!
             * Retrieves the raw DD node of CUDD associated with this BDD.
             *
             * @return The DD node of CUDD associated with this BDD.
             */
            DdNode* getCuddDdNode() const;
            
            /*!
             * Creates a DD that encapsulates the given CUDD ADD.
             *
             * @param ddManager The manager responsible for this DD.
             * @param cuddBdd The CUDD BDD to store.
             * @param containedMetaVariables The meta variables that appear in the DD.
             */
            Bdd(std::shared_ptr<DdManager<DdType::CUDD> const> ddManager, BDD cuddBdd, std::set<storm::expressions::Variable> const& containedMetaVariables = std::set<storm::expressions::Variable>());
            
            /*!
             * Builds a BDD representing the values that make the given filter function evaluate to true.
             *
             * @param ddManager The manager responsible for the BDD.
             * @param values The values that are to be checked against the filter function.
             * @param odd The ODD used for the translation.
             * @param metaVariables The meta variables used for the translation.
             * @param filter The filter that evaluates whether an encoding is to be mapped to 0 or 1.
             * @return The resulting (CUDD) BDD.
             */
            template<typename ValueType>
            static BDD fromVector(std::shared_ptr<DdManager<DdType::CUDD> const> ddManager, std::vector<ValueType> const& values, Odd<DdType::CUDD> const& odd, std::set<storm::expressions::Variable> const& metaVariables, std::function<bool (ValueType const&)> const& filter);
            
            /*!
             * Builds a BDD representing the values that make the given filter function evaluate to true.
             *
             * @param manager The manager responsible for the BDD.
             * @param currentOffset The current offset in the vector.
             * @param currentLevel The current level in the DD.
             * @param maxLevel The maximal level in the DD.
             * @param values The values that are to be checked against the filter function.
             * @param odd The ODD used for the translation.
             * @param ddVariableIndices The (sorted) list of DD variable indices to use.
             * @return The resulting (CUDD) BDD node.
             */
            template<typename ValueType>
            static DdNode* fromVectorRec(::DdManager* manager, uint_fast64_t& currentOffset, uint_fast64_t currentLevel, uint_fast64_t maxLevel, std::vector<ValueType> const& values, Odd<DdType::CUDD> const& odd, std::vector<uint_fast64_t> const& ddVariableIndices, std::function<bool (ValueType const&)> const& filter);
            
            /*!
             * Helper function to convert the DD into a bit vector.
             *
             * @param dd The DD to convert.
             * @param manager The Cudd manager responsible for the DDs.
             * @param result The vector that will hold the values upon successful completion.
             * @param rowOdd The ODD used for the row translation.
             * @param complement A flag indicating whether the result is to be interpreted as a complement.
             * @param currentRowLevel The currently considered row level in the DD.
             * @param maxLevel The number of levels that need to be considered.
             * @param currentRowOffset The current row offset.
             * @param ddRowVariableIndices The (sorted) indices of all DD row variables that need to be considered.
             */
            void toVectorRec(DdNode const* dd, Cudd const& manager, storm::storage::BitVector& result, Odd<DdType::CUDD> const& rowOdd, bool complement, uint_fast64_t currentRowLevel, uint_fast64_t maxLevel, uint_fast64_t currentRowOffset, std::vector<uint_fast64_t> const& ddRowVariableIndices) const;
            
            // The BDD created by CUDD.
            BDD cuddBdd;
        };
    }
}

#endif /* STORM_STORAGE_DD_CUDDBDD_H_ */