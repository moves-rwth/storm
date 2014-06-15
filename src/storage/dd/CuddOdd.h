#ifndef STORM_STORAGE_DD_CUDDODD_H_
#define STORM_STORAGE_DD_CUDDODD_H_

#include <memory>

#include "src/storage/dd/Odd.h"
#include "src/storage/dd/CuddDd.h"
#include "src/utility/OsDetection.h"

// Include the C++-interface of CUDD.
#include "cuddObj.hh"

namespace storm {
    namespace dd {
        template<>
        class Odd<DdType::CUDD> {
        public:
            /*!
             * Constructs an offset-labeled DD from the given DD.
             *
             * @param dd The DD for which to build the offset-labeled DD.
             */
            Odd(Dd<DdType::CUDD> const& dd);
            
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
            
        private:
            /*!
             * Constructs an offset-labeled DD with the given topmost DD node, else- and then-successor.
             *
             * @param dd The DD associated with this ODD node.
             * @param elseNode The else-successor of thie ODD node.
             * @param elseOffset The offset of the else-successor.
             * @param thenNode The then-successor of thie ODD node.
             * @param thenOffset The offset of the then-successor.
             */
            Odd(ADD dd, std::shared_ptr<Odd<DdType::CUDD>>&& elseNode, uint_fast64_t elseOffset, std::shared_ptr<Odd<DdType::CUDD>>&& thenNode, uint_fast64_t thenOffset);
            
            /*!
             * Recursively builds the ODD.
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
            static std::shared_ptr<Odd<DdType::CUDD>> buildOddRec(DdNode* dd, Cudd const& manager, uint_fast64_t currentLevel, uint_fast64_t maxLevel, std::vector<uint_fast64_t> const& ddVariableIndices, std::vector<std::map<DdNode*, std::shared_ptr<Odd<DdType::CUDD>>>>& uniqueTableForLevels);
            
            // The DD associated with this ODD node.
            ADD dd;
            
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