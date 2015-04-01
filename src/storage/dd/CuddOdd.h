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
            
        private:
            // Declare a hash functor that is used for the unique tables in the construction process.
            class HashFunctor {
            public:
                std::size_t operator()(std::pair<DdNode*, bool> const& key) const;
            };
            
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