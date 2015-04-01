#include "src/storage/dd/CuddOdd.h"

#include <algorithm>

#include "src/storage/dd/CuddDdManager.h"
#include "src/storage/dd/CuddDdMetaVariable.h"

namespace storm {
    namespace dd {
        Odd<DdType::CUDD>::Odd(Add<DdType::CUDD> const& add) {
            std::shared_ptr<DdManager<DdType::CUDD> const> manager = add.getDdManager();
            
            // First, we need to determine the involved DD variables indices.
            std::vector<uint_fast64_t> ddVariableIndices = add.getSortedVariableIndices();
            
            // Prepare a unique table for each level that keeps the constructed ODD nodes unique.
            std::vector<std::map<DdNode*, std::shared_ptr<Odd<DdType::CUDD>>>> uniqueTableForLevels(ddVariableIndices.size() + 1);
            
            // Now construct the ODD structure from the ADD.
            std::shared_ptr<Odd<DdType::CUDD>> rootOdd = buildOddFromAddRec(add.getCuddDdNode(), manager->getCuddManager(), 0, ddVariableIndices.size(), ddVariableIndices, uniqueTableForLevels);
            
            // Finally, move the children of the root ODD into this ODD.
            this->elseNode = std::move(rootOdd->elseNode);
            this->thenNode = std::move(rootOdd->thenNode);
            this->elseOffset = rootOdd->elseOffset;
            this->thenOffset = rootOdd->thenOffset;
        }
        
        Odd<DdType::CUDD>::Odd(Bdd<DdType::CUDD> const& bdd) {
            std::shared_ptr<DdManager<DdType::CUDD> const> manager = bdd.getDdManager();
            
            // First, we need to determine the involved DD variables indices.
            std::vector<uint_fast64_t> ddVariableIndices = bdd.getSortedVariableIndices();
            
            // Prepare a unique table for each level that keeps the constructed ODD nodes unique.
            std::vector<std::map<DdNode*, std::shared_ptr<Odd<DdType::CUDD>>>> uniqueTableForLevels(ddVariableIndices.size() + 1);
            
            // Now construct the ODD structure from the ADD.
            bdd.exportToDot("bdd.dot");
            bdd.toAdd().exportToDot("add.dot");
            std::cout << "count: " << bdd.getNonZeroCount() << std::endl;
            std::cout << "root cmpl? " << Cudd_IsComplement(bdd.getCuddDdNode()) << std::endl;
            std::shared_ptr<Odd<DdType::CUDD>> rootOdd = buildOddFromBddRec(bdd.getCuddDdNode(), manager->getCuddManager(), 0, Cudd_IsComplement(bdd.getCuddDdNode()), ddVariableIndices.size(), ddVariableIndices, uniqueTableForLevels);

            // Finally, move the children of the root ODD into this ODD.
            this->elseNode = std::move(rootOdd->elseNode);
            this->thenNode = std::move(rootOdd->thenNode);
            
            // If the node is a complement node,
            if (Cudd_IsComplement(bdd.getCuddDdNode())) {
                this->elseOffset = (1ull << (ddVariableIndices.size() - 1)) - rootOdd->elseOffset;
                this->thenOffset = (1ull << (ddVariableIndices.size() - 1)) - rootOdd->thenOffset;
            } else {
                this->elseOffset = rootOdd->elseOffset;
                this->thenOffset = rootOdd->thenOffset;
            }
            
            std::cout << "then offset is: " << this->thenOffset << std::endl;
            std::cout << "else offset is: " << this->elseOffset << std::endl;
        }
        
        Odd<DdType::CUDD>::Odd(std::shared_ptr<Odd<DdType::CUDD>> elseNode, uint_fast64_t elseOffset, std::shared_ptr<Odd<DdType::CUDD>> thenNode, uint_fast64_t thenOffset) : elseNode(elseNode), thenNode(thenNode), elseOffset(elseOffset), thenOffset(thenOffset) {
            std::cout << "creating ODD with offsets (" << elseOffset << ", " << thenOffset << ")" << std::endl;
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
        
        std::shared_ptr<Odd<DdType::CUDD>> Odd<DdType::CUDD>::buildOddFromAddRec(DdNode* dd, Cudd const& manager, uint_fast64_t currentLevel, uint_fast64_t maxLevel, std::vector<uint_fast64_t> const& ddVariableIndices, std::vector<std::map<DdNode*, std::shared_ptr<Odd<DdType::CUDD>>>>& uniqueTableForLevels) {
            // Check whether the ODD for this node has already been computed (for this level) and if so, return this instead.
            auto const& iterator = uniqueTableForLevels[currentLevel].find(dd);
            if (iterator != uniqueTableForLevels[currentLevel].end()) {
                return iterator->second;
            } else {
                // Otherwise, we need to recursively compute the ODD.
                
                // If we are already past the maximal level that is to be considered, we can simply create an Odd without
                // successors
                if (currentLevel == maxLevel) {
                    uint_fast64_t elseOffset = 0;
                    uint_fast64_t thenOffset = 0;
                    
                    // If the DD is not the zero leaf, then the then-offset is 1.
                    if ((Cudd_IsComplement(dd) && dd != Cudd_ReadOne(manager.getManager())) || (!Cudd_IsComplement(dd) && dd != Cudd_ReadZero(manager.getManager()))) {
                        thenOffset = 1;
                    }
                    
                    return std::shared_ptr<Odd<DdType::CUDD>>(new Odd<DdType::CUDD>(nullptr, elseOffset, nullptr, thenOffset));
                } else if (ddVariableIndices[currentLevel] < static_cast<uint_fast64_t>(dd->index)) {
                    
                    // If we skipped the level in the DD, we compute the ODD just for the else-successor and use the same
                    // node for the then-successor as well.
                    std::shared_ptr<Odd<DdType::CUDD>> elseNode = buildOddFromAddRec(dd, manager, currentLevel + 1, maxLevel, ddVariableIndices, uniqueTableForLevels);
                    std::shared_ptr<Odd<DdType::CUDD>> thenNode = elseNode;
                    return std::shared_ptr<Odd<DdType::CUDD>>(new Odd<DdType::CUDD>(elseNode, elseNode->getElseOffset() + elseNode->getThenOffset(), thenNode, thenNode->getElseOffset() + thenNode->getThenOffset()));
                } else {
                    // Otherwise, we compute the ODDs for both the then- and else successors.
                    bool elseComplemented = Cudd_IsComplement(Cudd_E(dd));
                    bool thenComplemented = Cudd_IsComplement(Cudd_T(dd));
                    std::cout << "something complemented? " << elseComplemented << " // " << thenComplemented << std::endl;
                    std::shared_ptr<Odd<DdType::CUDD>> elseNode = buildOddFromAddRec(Cudd_E(dd), manager, currentLevel + 1, maxLevel, ddVariableIndices, uniqueTableForLevels);
                    std::shared_ptr<Odd<DdType::CUDD>> thenNode = buildOddFromAddRec(Cudd_T(dd), manager, currentLevel + 1, maxLevel, ddVariableIndices, uniqueTableForLevels);
                    uint_fast64_t totalElseOffset = elseNode->getElseOffset() + elseNode->getThenOffset();
                    uint_fast64_t totalThenOffset = thenNode->getElseOffset() + thenNode->getThenOffset();
                    return std::shared_ptr<Odd<DdType::CUDD>>(new Odd<DdType::CUDD>(elseNode, elseComplemented ? (1ull << (maxLevel - currentLevel - 1)) - totalElseOffset : totalElseOffset, thenNode, thenComplemented ? (1ull << (maxLevel - currentLevel - 1)) - totalThenOffset : totalThenOffset));
                }
            }
        }
        
        std::shared_ptr<Odd<DdType::CUDD>> Odd<DdType::CUDD>::buildOddFromBddRec(DdNode* dd, Cudd const& manager, uint_fast64_t currentLevel, bool complement, uint_fast64_t maxLevel, std::vector<uint_fast64_t> const& ddVariableIndices, std::vector<std::map<DdNode*, std::shared_ptr<Odd<DdType::CUDD>>>>& uniqueTableForLevels) {
            // Check whether the ODD for this node has already been computed (for this level) and if so, return this instead.
            auto const& iterator = uniqueTableForLevels[currentLevel].find(dd);
            if (iterator != uniqueTableForLevels[currentLevel].end()) {
                return iterator->second;
            } else {
                std::cout << "treating level " << currentLevel << std::endl;
                // Otherwise, we need to recursively compute the ODD.
                
                // If we are already past the maximal level that is to be considered, we can simply create an Odd without
                // successors
                if (currentLevel == maxLevel) {
                    std::cout << "curLev " << currentLevel << " and max " << maxLevel << std::endl;
                    uint_fast64_t elseOffset = 0;
                    uint_fast64_t thenOffset = 0;
                    
                    // If the DD is not the zero leaf, then the then-offset is 1.
                    std::cout << "complement flag set? " << complement << std::endl;
                    DdNode* node = Cudd_Regular(dd);
                    if (node != Cudd_ReadZero(manager.getManager())) {
                        std::cout << "offset is one" << std::endl;
                        thenOffset = 1;
                    }
                    
                    // If we need to complement the 'terminal' node, we need to negate its offset.
                    if (complement) {
                        std::cout << "negating offset" << std::endl;
                        thenOffset = 1 - thenOffset;
                    }
                    
                    std::cout << "(1) new ODD at level " << currentLevel << std::endl;
                    return std::shared_ptr<Odd<DdType::CUDD>>(new Odd<DdType::CUDD>(nullptr, elseOffset, nullptr, thenOffset));
                } else if (ddVariableIndices[currentLevel] < static_cast<uint_fast64_t>(dd->index)) {
                    // If we skipped the level in the DD, we compute the ODD just for the else-successor and use the same
                    // node for the then-successor as well.
                    std::cout << "following then/else node..." << std::endl;
                    std::shared_ptr<Odd<DdType::CUDD>> elseNode = buildOddFromBddRec(dd, manager, currentLevel + 1, complement, maxLevel, ddVariableIndices, uniqueTableForLevels);
                    std::shared_ptr<Odd<DdType::CUDD>> thenNode = elseNode;
                    uint_fast64_t totalOffset = elseNode->getElseOffset() + elseNode->getThenOffset();
                    if (complement) {
                        totalOffset = (1ull << (maxLevel - currentLevel - 1)) - totalOffset;
                    }
                    
                    std::cout << "(2) new ODD at level " << currentLevel << std::endl;
                    return std::shared_ptr<Odd<DdType::CUDD>>(new Odd<DdType::CUDD>(elseNode, totalOffset, thenNode, totalOffset));
                } else {
                    // Otherwise, we compute the ODDs for both the then- and else successors.
                    bool elseComplemented = Cudd_IsComplement(Cudd_E(dd)) ^ complement;
                    bool thenComplemented = Cudd_IsComplement(Cudd_T(dd)) ^ complement;
                    std::cout << "something complemented? " << elseComplemented << " // " << thenComplemented << std::endl;
                    std::cout << "following else node..." << std::endl;
                    std::shared_ptr<Odd<DdType::CUDD>> elseNode = buildOddFromBddRec(Cudd_E(dd), manager, currentLevel + 1, elseComplemented, maxLevel, ddVariableIndices, uniqueTableForLevels);
                    std::cout << "following then node..." << std::endl;
                    std::shared_ptr<Odd<DdType::CUDD>> thenNode = buildOddFromBddRec(Cudd_T(dd), manager, currentLevel + 1, thenComplemented, maxLevel, ddVariableIndices, uniqueTableForLevels);
                    uint_fast64_t totalElseOffset = elseNode->getElseOffset() + elseNode->getThenOffset();
                    uint_fast64_t totalThenOffset = thenNode->getElseOffset() + thenNode->getThenOffset();
                    std::cout << "(3) new ODD at level " << currentLevel << std::endl;
                    return std::shared_ptr<Odd<DdType::CUDD>>(new Odd<DdType::CUDD>(elseNode, elseComplemented ? (1ull << (maxLevel - currentLevel - 1)) - totalElseOffset : totalElseOffset, thenNode, thenComplemented ? (1ull << (maxLevel - currentLevel - 1)) - totalThenOffset : totalThenOffset));
                }
            }
        }
    }
}