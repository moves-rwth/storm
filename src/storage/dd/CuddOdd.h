#ifndef STORM_STORAGE_DD_CUDDODD_H_
#define STORM_STORAGE_DD_CUDDODD_H_

#include <memory>
#include <unordered_map>

#include "src/storage/dd/Odd.h"
#include "src/storage/dd/CuddAdd.h"
#include "src/utility/OsDetection.h"

// Include the C++-interface of CUDD.
#include "cuddObj.hh"

namespace storm {
    namespace dd {
        template<>
        class Odd<DdType::CUDD> {
        public:
            /*!
             * Constructs an offset-labeled DD from the given ADD.
             *
             * @param add The ADD for which to build the offset-labeled ADD.
             */
            Odd(Add<DdType::CUDD> const& add);
            
            /*!
             * Constructs an offset-labeled DD from the given BDD.
             *
             * @param bdd The BDD for which to build the offset-labeled ADD.
             */
            Odd(Bdd<DdType::CUDD> const& bdd);
            
            // Instantiate all copy/move constructors/assignments with the default implementation.
            Odd() = default;
            Odd(Odd<DdType::CUDD> const& other) = default;
			Odd& operator=(Odd<DdType::CUDD> const& other) = default;
#ifndef WINDOWS
            Odd(Odd<DdType::CUDD>&& other) = default;
            Odd& operator=(Odd<DdType::CUDD>&& other) = default;
#endif
            
            /*!
             * Retrieves the then-successor of this ODD node.
             *
             * @return The then-successor of this ODD node.
             */
            Odd<DdType::CUDD> const& getThenSuccessor() const;

            /*!
             * Retrieves the else-successor of this ODD node.
             *
             * @return The else-successor of this ODD node.
             */
            Odd<DdType::CUDD> const& getElseSuccessor() const;

            /*!
             * Retrieves the else-offset of this ODD node.
             *
             * @return The else-offset of this ODD node.
             */
            uint_fast64_t getElseOffset() const;
            
            /*!
             * Sets the else-offset of this ODD node.
             *
             * @param newOffset The new else-offset of this ODD node.
             */
            void setElseOffset(uint_fast64_t newOffset);
            
            /*!
             * Retrieves the then-offset of this ODD node.
             *
             * @return The then-offset of this ODD node.
             */
            uint_fast64_t getThenOffset() const;
            
            /*!
             * Sets the then-offset of this ODD node.
             *
             * @param newOffset The new then-offset of this ODD node.
             */
            void setThenOffset(uint_fast64_t newOffset);
            
            /*!
             * Retrieves the total offset, i.e., the sum of the then- and else-offset.
             *
             * @return The total offset of this ODD.
             */
            uint_fast64_t getTotalOffset() const;
            
            /*!
             * Retrieves the size of the ODD. Note: the size is computed by a traversal, so this may be costlier than
             * expected.
             *
             * @return The size (in nodes) of this ODD.
             */
            uint_fast64_t getNodeCount() const;
            
            /*!
             * Retrieves the height of the ODD.
             *
             * @return The height of the ODD.
             */
            uint_fast64_t getHeight() const;
            
            /*!
             * Checks whether the given ODD node is a terminal node, i.e. has no successors.
             *
             * @return True iff the node is terminal.
             */
            bool isTerminalNode() const;
            
            /*!
             * Filters the given explicit vector using the symbolic representation of which values to select.
             *
             * @param selectedValues A symbolic representation of which values to select.
             * @param values The value vector from which to select the values.
             * @return The resulting vector.
             */
            std::vector<double> filterExplicitVector(storm::dd::Bdd<DdType::CUDD> const& selectedValues, std::vector<double> const& values) const;
            
            /*!
             * Adds the old values to the new values. It does so by writing the old values at their correct positions
             * wrt. to the new ODD.
             *
             * @param newOdd The new ODD to use.
             * @param oldValues The old vector of values (which is being read).
             * @param newValues The new vector of values (which is being written).
             */
            void expandExplicitVector(storm::dd::Odd<DdType::CUDD> const& newOdd, std::vector<double> const& oldValues, std::vector<double>& newValues) const;
            
            /*!
             * Exports the ODD in the dot format to the given file.
             *
             * @param filename The name of the file to which to write the dot output.
             */
            void exportToDot(std::string const& filename) const;
            
        private:
            // Declare a hash functor that is used for the unique tables in the construction process.
            class HashFunctor {
            public:
                std::size_t operator()(std::pair<DdNode*, bool> const& key) const;
            };
            
            /*!
             * Adds all nodes below the current one to the given mapping.
             *
             * @param levelToOddNodesMap A mapping of the level to the ODD node.
             * @param The level of the current node.
             */
            void addToLevelToOddNodesMap(std::map<uint_fast64_t, std::vector<std::reference_wrapper<storm::dd::Odd<DdType::CUDD> const>>>& levelToOddNodesMap, uint_fast64_t level = 0) const;
            
            /*!
             * Constructs an offset-labeled DD with the given topmost DD node, else- and then-successor.
             *
             * @param dd The DD node associated with this ODD node.
             * @param elseNode The else-successor of thie ODD node.
             * @param elseOffset The offset of the else-successor.
             * @param thenNode The then-successor of thie ODD node.
             * @param thenOffset The offset of the then-successor.
             */
            Odd(std::shared_ptr<Odd<DdType::CUDD>> elseNode, uint_fast64_t elseOffset, std::shared_ptr<Odd<DdType::CUDD>> thenNode, uint_fast64_t thenOffset);
            
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
            static std::shared_ptr<Odd<DdType::CUDD>> buildOddFromAddRec(DdNode* dd, Cudd const& manager, uint_fast64_t currentLevel, uint_fast64_t maxLevel, std::vector<uint_fast64_t> const& ddVariableIndices, std::vector<std::unordered_map<DdNode*, std::shared_ptr<Odd<DdType::CUDD>>>>& uniqueTableForLevels);

            /*!
             * Recursively builds the ODD from a BDD (that potentially has complement edges).
             *
             * @param dd The DD for which to build the ODD.
             * @param manager The manager responsible for the DD.
             * @param currentLevel The currently considered level in the DD.
             * @param complement A flag indicating whether or not the given node is to be considered as complemented.
             * @param maxLevel The number of levels that need to be considered.
             * @param ddVariableIndices The (sorted) indices of all DD variables that need to be considered.
             * @param uniqueTableForLevels A vector of unique tables, one for each level to be considered, that keeps
             * ODD nodes for the same DD and level unique.
             * @return A pointer to the constructed ODD for the given arguments.
             */
            static std::shared_ptr<Odd<DdType::CUDD>> buildOddFromBddRec(DdNode* dd, Cudd const& manager, uint_fast64_t currentLevel, bool complement, uint_fast64_t maxLevel, std::vector<uint_fast64_t> const& ddVariableIndices, std::vector<std::unordered_map<std::pair<DdNode*, bool>, std::shared_ptr<Odd<DdType::CUDD>>, HashFunctor>>& uniqueTableForLevels);
            
            /*!
             * Adds the selected values the target vector.
             *
             * @param dd The current node of the DD representing the selected values.
             * @param manager The manager responsible for the DD.
             * @param currentLevel The currently considered level in the DD.
             * @param maxLevel The number of levels that need to be considered.
             * @param ddVariableIndices The sorted list of variable indices to use.
             * @param currentOffset The offset along the path taken in the DD representing the selected values.
             * @param odd The current ODD node.
             * @param result The target vector to which to write the values.
             * @param currentIndex The index at which the next element is to be written.
             * @param values The value vector from which to select the values.
             */
            static void addSelectedValuesToVectorRec(DdNode* dd, Cudd const& manager, uint_fast64_t currentLevel, bool complement, uint_fast64_t maxLevel, std::vector<uint_fast64_t> const& ddVariableIndices, uint_fast64_t currentOffset, storm::dd::Odd<DdType::CUDD> const& odd, std::vector<double>& result, uint_fast64_t& currentIndex, std::vector<double> const& values);
            
            /*!
             * Adds the values of the old explicit values to the new explicit values where the positions in the old vector
             * are given by the current old ODD and the positions in the new vector are given by the new ODD.
             *
             * @param oldOffset The offset in the old explicit values.
             * @param oldOdd The ODD to use for the old explicit values.
             * @param oldValues The vector of old values.
             * @param newOffset The offset in the new explicit values.
             * @param newOdd The ODD to use for the new explicit values.
             * @param newValues The vector of new values.
             */
            static void expandValuesToVectorRec(uint_fast64_t oldOffset, storm::dd::Odd<DdType::CUDD> const& oldOdd, std::vector<double> const& oldValues, uint_fast64_t newOffset, storm::dd::Odd<DdType::CUDD> const& newOdd, std::vector<double>& newValues);
            
            // The then- and else-nodes.
            std::shared_ptr<Odd<DdType::CUDD>> elseNode;
            std::shared_ptr<Odd<DdType::CUDD>> thenNode;
            
            // The offsets that need to be added if the then- or else-successor is taken, respectively.
            uint_fast64_t elseOffset;
            uint_fast64_t thenOffset;
        };
    }
}

#endif /* STORM_STORAGE_DD_CUDDODD_H_ */