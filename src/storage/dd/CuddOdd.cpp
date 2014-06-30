#include "src/storage/dd/CuddOdd.h"

#include <algorithm>

#include "src/storage/dd/CuddDdManager.h"
#include "src/storage/dd/CuddDdMetaVariable.h"

namespace storm {
    namespace dd {
        Odd<DdType::CUDD>::Odd(Dd<DdType::CUDD> const& dd) {
            std::shared_ptr<DdManager<DdType::CUDD>> manager = dd.getDdManager();
            
            // First, we need to determine the involved DD variables indices.
            std::vector<uint_fast64_t> ddVariableIndices = dd.getSortedVariableIndices();
            
            // Prepare a unique table for each level that keeps the constructed ODD nodes unique.
            std::vector<std::map<DdNode*, std::shared_ptr<Odd<DdType::CUDD>>>> uniqueTableForLevels(ddVariableIndices.size() + 1);
            
            // Now construct the ODD structure.
            std::shared_ptr<Odd<DdType::CUDD>> rootOdd = buildOddRec(dd.getCuddAdd().getNode(), manager->getCuddManager(), 0, ddVariableIndices.size(), ddVariableIndices, uniqueTableForLevels);
            
            // Finally, move the children of the root ODD into this ODD.
            this->dd = rootOdd->dd;
            this->elseNode = std::move(rootOdd->elseNode);
            this->thenNode = std::move(rootOdd->thenNode);
            this->elseOffset = rootOdd->elseOffset;
            this->thenOffset = rootOdd->thenOffset;
        }
        
        Odd<DdType::CUDD>::Odd(ADD dd, std::shared_ptr<Odd<DdType::CUDD>>&& elseNode, uint_fast64_t elseOffset, std::shared_ptr<Odd<DdType::CUDD>>&& thenNode, uint_fast64_t thenOffset) : dd(dd), elseNode(elseNode), thenNode(thenNode), elseOffset(elseOffset), thenOffset(thenOffset) {
            // Intentionally left empty.
        }
        
        Odd<DdType::CUDD> const& Odd<DdType::CUDD>::getThenSuccessor() const {
            return *this->thenNode;
        }
        
        Odd<DdType::CUDD> const& Odd<DdType::CUDD>::getElseSuccessor() const {
            return *this->elseNode;
        }
        
        uint_fast64_t Odd<DdType::CUDD>::getElseOffset() const {
            return this->elseOffset;
        }
        
        void Odd<DdType::CUDD>::setElseOffset(uint_fast64_t newOffset) {
            this->elseOffset = newOffset;
        }
        
        uint_fast64_t Odd<DdType::CUDD>::getThenOffset() const {
            return this->thenOffset;
        }
        
        void Odd<DdType::CUDD>::setThenOffset(uint_fast64_t newOffset) {
            this->thenOffset = newOffset;
        }
        
        uint_fast64_t Odd<DdType::CUDD>::getTotalOffset() const {
            return this->elseOffset + this->thenOffset;
        }
        
        uint_fast64_t Odd<DdType::CUDD>::getNodeCount() const {
            // If the ODD contains a constant (and thus has no children), the size is 1.
            if (this->elseNode == nullptr && this->thenNode == nullptr) {
                return 1;
            }
            
            // If the two successors are actually the same, we need to count the subnodes only once.
            if (this->elseNode == this->thenNode) {
                return this->elseNode->getNodeCount();
            } else {
                return this->elseNode->getNodeCount() + this->thenNode->getNodeCount();
            }
        }
        
        std::shared_ptr<Odd<DdType::CUDD>> Odd<DdType::CUDD>::buildOddRec(DdNode* dd, Cudd const& manager, uint_fast64_t currentLevel, uint_fast64_t maxLevel, std::vector<uint_fast64_t> const& ddVariableIndices, std::vector<std::map<DdNode*, std::shared_ptr<Odd<DdType::CUDD>>>>& uniqueTableForLevels) {
            // Check whether the ODD for this node has already been computed (for this level) and if so, return this instead.
            auto const& iterator = uniqueTableForLevels[currentLevel].find(dd);
            if (iterator != uniqueTableForLevels[currentLevel].end()) {
                return iterator->second;
            } else {
                // Otherwise, we need to recursively compute the ODD.
                
                // If we are already past the maximal level that is to be considered, we can simply create a Odd without
                // successors
                if (currentLevel == maxLevel) {
                    uint_fast64_t elseOffset = 0;
                    uint_fast64_t thenOffset = 0;
                    
                    // If the DD is not the zero leaf, then the then-offset is 1.
                    if (dd != Cudd_ReadZero(manager.getManager())) {
                        thenOffset = 1;
                    }
                    
                    return std::shared_ptr<Odd<DdType::CUDD>>(new Odd<DdType::CUDD>(ADD(manager, dd), nullptr, elseOffset, nullptr, thenOffset));
                } else if (ddVariableIndices[currentLevel] < static_cast<uint_fast64_t>(dd->index)) {
                    // If we skipped the level in the DD, we compute the ODD just for the else-successor and use the same
                    // node for the then-successor as well.
                    std::shared_ptr<Odd<DdType::CUDD>> elseNode = buildOddRec(dd, manager, currentLevel + 1, maxLevel, ddVariableIndices, uniqueTableForLevels);
                    std::shared_ptr<Odd<DdType::CUDD>> thenNode = elseNode;
                    return std::shared_ptr<Odd<DdType::CUDD>>(new Odd<DdType::CUDD>(ADD(manager, dd), std::move(elseNode), elseNode->getElseOffset() + elseNode->getThenOffset(), std::move(thenNode), thenNode->getElseOffset() + thenNode->getThenOffset()));
                } else {
                    // Otherwise, we compute the ODDs for both the then- and else successors.
                    std::shared_ptr<Odd<DdType::CUDD>> elseNode = buildOddRec(Cudd_E(dd), manager, currentLevel + 1, maxLevel, ddVariableIndices, uniqueTableForLevels);
                    std::shared_ptr<Odd<DdType::CUDD>> thenNode = buildOddRec(Cudd_T(dd), manager, currentLevel + 1, maxLevel, ddVariableIndices, uniqueTableForLevels);
                    return std::shared_ptr<Odd<DdType::CUDD>>(new Odd<DdType::CUDD>(ADD(manager, dd), std::move(elseNode), elseNode->getElseOffset() + elseNode->getThenOffset(), std::move(thenNode), thenNode->getElseOffset() + thenNode->getThenOffset()));
                }
            }
        }
    }
}