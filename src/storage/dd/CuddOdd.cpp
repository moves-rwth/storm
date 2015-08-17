#include "src/storage/dd/CuddOdd.h"

#include <algorithm>
#include <boost/functional/hash.hpp>

#include "src/exceptions/InvalidArgumentException.h"
#include "src/utility/macros.h"

#include "src/storage/dd/CuddDdManager.h"
#include "src/storage/dd/CuddDdMetaVariable.h"

namespace storm {
    namespace dd {
        Odd<DdType::CUDD>::Odd(Add<DdType::CUDD> const& add) {
            std::shared_ptr<DdManager<DdType::CUDD> const> manager = add.getDdManager();
            
            // First, we need to determine the involved DD variables indices.
            std::vector<uint_fast64_t> ddVariableIndices = add.getSortedVariableIndices();
            
            // Prepare a unique table for each level that keeps the constructed ODD nodes unique.
            std::vector<std::unordered_map<DdNode*, std::shared_ptr<Odd<DdType::CUDD>>>> uniqueTableForLevels(ddVariableIndices.size() + 1);
            
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
            std::vector<std::unordered_map<std::pair<DdNode*, bool>, std::shared_ptr<Odd<DdType::CUDD>>, HashFunctor>> uniqueTableForLevels(ddVariableIndices.size() + 1);
            
            // Now construct the ODD structure from the BDD.
            std::shared_ptr<Odd<DdType::CUDD>> rootOdd = buildOddFromBddRec(Cudd_Regular(bdd.getCuddDdNode()), manager->getCuddManager(), 0, Cudd_IsComplement(bdd.getCuddDdNode()), ddVariableIndices.size(), ddVariableIndices, uniqueTableForLevels);
            
            // Finally, move the children of the root ODD into this ODD.
            this->elseNode = std::move(rootOdd->elseNode);
            this->thenNode = std::move(rootOdd->thenNode);
            this->elseOffset = rootOdd->elseOffset;
            this->thenOffset = rootOdd->thenOffset;
        }
        
        Odd<DdType::CUDD>::Odd(std::shared_ptr<Odd<DdType::CUDD>> elseNode, uint_fast64_t elseOffset, std::shared_ptr<Odd<DdType::CUDD>> thenNode, uint_fast64_t thenOffset) : elseNode(elseNode), thenNode(thenNode), elseOffset(elseOffset), thenOffset(thenOffset) {
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
        
        bool Odd<DdType::CUDD>::isTerminalNode() const {
            return this->elseNode == nullptr && this->thenNode == nullptr;
        }
        
        std::vector<double> Odd<DdType::CUDD>::filterExplicitVector(storm::dd::Bdd<DdType::CUDD> const& selectedValues, std::vector<double> const& values) const {
            std::vector<double> result(selectedValues.getNonZeroCount());
            
            // First, we need to determine the involved DD variables indices.
            std::vector<uint_fast64_t> ddVariableIndices = selectedValues.getSortedVariableIndices();
            
            uint_fast64_t currentIndex = 0;
            addSelectedValuesToVectorRec(selectedValues.getCuddDdNode(), selectedValues.getDdManager()->getCuddManager(), 0, Cudd_IsComplement(selectedValues.getCuddDdNode()), ddVariableIndices.size(), ddVariableIndices, 0, *this, result, currentIndex, values);
            return result;
        }
        
        void Odd<DdType::CUDD>::expandExplicitVector(storm::dd::Odd<DdType::CUDD> const& newOdd, std::vector<double> const& oldValues, std::vector<double>& newValues) const {
            expandValuesToVectorRec(0, *this, oldValues, 0, newOdd, newValues);
        }
        
        void Odd<DdType::CUDD>::expandValuesToVectorRec(uint_fast64_t oldOffset, storm::dd::Odd<DdType::CUDD> const& oldOdd, std::vector<double> const& oldValues, uint_fast64_t newOffset, storm::dd::Odd<DdType::CUDD> const& newOdd, std::vector<double>& newValues) {
            if (oldOdd.isTerminalNode()) {
                STORM_LOG_THROW(newOdd.isTerminalNode(), storm::exceptions::InvalidArgumentException, "The ODDs for the translation must have the same height.");
                if (oldOdd.getThenOffset() != 0) {
                    newValues[newOffset] += oldValues[oldOffset];
                }
            } else {
                expandValuesToVectorRec(oldOffset, oldOdd.getElseSuccessor(), oldValues, newOffset, newOdd.getElseSuccessor(), newValues);
                expandValuesToVectorRec(oldOffset + oldOdd.getElseOffset(), oldOdd.getThenSuccessor(), oldValues, newOffset + newOdd.getElseOffset(), newOdd.getThenSuccessor(), newValues);
            }
        }
        
        void Odd<DdType::CUDD>::addSelectedValuesToVectorRec(DdNode* dd, Cudd const& manager, uint_fast64_t currentLevel, bool complement, uint_fast64_t maxLevel, std::vector<uint_fast64_t> const& ddVariableIndices, uint_fast64_t currentOffset, storm::dd::Odd<DdType::CUDD> const& odd, std::vector<double>& result, uint_fast64_t& currentIndex, std::vector<double> const& values) {
            // If there are no more values to select, we can directly return.
            if (dd == Cudd_ReadLogicZero(manager.getManager()) && !complement) {
                return;
            } else if (dd == Cudd_ReadOne(manager.getManager()) && complement) {
                return;
            }
            
            if (currentLevel == maxLevel) {
                // If the DD is not the zero leaf, then the then-offset is 1.
                bool selected = false;
                if (dd != Cudd_ReadLogicZero(manager.getManager())) {
                    selected = !complement;
                }
                
                if (selected) {
                    result[currentIndex++] = values[currentOffset];
                }
            } else if (ddVariableIndices[currentLevel] < dd->index) {
                // If we skipped a level, we need to enumerate the explicit entries for the case in which the bit is set
                // and for the one in which it is not set.
                addSelectedValuesToVectorRec(dd, manager, currentLevel + 1, complement, maxLevel, ddVariableIndices, currentOffset, odd.getElseSuccessor(), result, currentIndex, values);
                addSelectedValuesToVectorRec(dd, manager, currentLevel + 1, complement, maxLevel, ddVariableIndices, currentOffset + odd.getElseOffset(), odd.getThenSuccessor(), result, currentIndex, values);
            } else {
                // Otherwise, we compute the ODDs for both the then- and else successors.
                DdNode* thenDdNode = Cudd_T(dd);
                DdNode* elseDdNode = Cudd_E(dd);
                
                // Determine whether we have to evaluate the successors as if they were complemented.
                bool elseComplemented = Cudd_IsComplement(elseDdNode) ^ complement;
                bool thenComplemented = Cudd_IsComplement(thenDdNode) ^ complement;
                
                addSelectedValuesToVectorRec(Cudd_Regular(elseDdNode), manager, currentLevel + 1, elseComplemented, maxLevel, ddVariableIndices, currentOffset, odd.getElseSuccessor(), result, currentIndex, values);
                addSelectedValuesToVectorRec(Cudd_Regular(thenDdNode), manager, currentLevel + 1, thenComplemented, maxLevel, ddVariableIndices, currentOffset + odd.getElseOffset(), odd.getThenSuccessor(), result, currentIndex, values);
            }
        }
        
        std::shared_ptr<Odd<DdType::CUDD>> Odd<DdType::CUDD>::buildOddFromAddRec(DdNode* dd, Cudd const& manager, uint_fast64_t currentLevel, uint_fast64_t maxLevel, std::vector<uint_fast64_t> const& ddVariableIndices, std::vector<std::unordered_map<DdNode*, std::shared_ptr<Odd<DdType::CUDD>>>>& uniqueTableForLevels) {
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
                    if (dd != Cudd_ReadZero(manager.getManager())) {
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
                    std::shared_ptr<Odd<DdType::CUDD>> elseNode = buildOddFromAddRec(Cudd_E(dd), manager, currentLevel + 1, maxLevel, ddVariableIndices, uniqueTableForLevels);
                    std::shared_ptr<Odd<DdType::CUDD>> thenNode = buildOddFromAddRec(Cudd_T(dd), manager, currentLevel + 1, maxLevel, ddVariableIndices, uniqueTableForLevels);
                    uint_fast64_t totalElseOffset = elseNode->getElseOffset() + elseNode->getThenOffset();
                    uint_fast64_t totalThenOffset = thenNode->getElseOffset() + thenNode->getThenOffset();
                    return std::shared_ptr<Odd<DdType::CUDD>>(new Odd<DdType::CUDD>(elseNode, elseComplemented ? (1ull << (maxLevel - currentLevel - 1)) - totalElseOffset : totalElseOffset, thenNode, thenComplemented ? (1ull << (maxLevel - currentLevel - 1)) - totalThenOffset : totalThenOffset));
                }
            }
        }
        
        std::size_t Odd<DdType::CUDD>::HashFunctor::operator()(std::pair<DdNode*, bool> const& key) const {
            std::size_t result = 0;
            boost::hash_combine(result, key.first);
            boost::hash_combine(result, key.second);
            return result;
        }
        
        std::shared_ptr<Odd<DdType::CUDD>> Odd<DdType::CUDD>::buildOddFromBddRec(DdNode* dd, Cudd const& manager, uint_fast64_t currentLevel, bool complement, uint_fast64_t maxLevel, std::vector<uint_fast64_t> const& ddVariableIndices, std::vector<std::unordered_map<std::pair<DdNode*, bool>, std::shared_ptr<Odd<DdType::CUDD>>, HashFunctor>>& uniqueTableForLevels) {
            // Check whether the ODD for this node has already been computed (for this level) and if so, return this instead.
            auto const& iterator = uniqueTableForLevels[currentLevel].find(std::make_pair(dd, complement));
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
                    if (dd != Cudd_ReadZero(manager.getManager())) {
                        thenOffset = 1;
                    }
                    
                    // If we need to complement the 'terminal' node, we need to negate its offset.
                    if (complement) {
                        thenOffset = 1 - thenOffset;
                    }
                    
                    return std::shared_ptr<Odd<DdType::CUDD>>(new Odd<DdType::CUDD>(nullptr, elseOffset, nullptr, thenOffset));
                } else if (ddVariableIndices[currentLevel] < static_cast<uint_fast64_t>(dd->index)) {
                    // If we skipped the level in the DD, we compute the ODD just for the else-successor and use the same
                    // node for the then-successor as well.
                    std::shared_ptr<Odd<DdType::CUDD>> elseNode = buildOddFromBddRec(dd, manager, currentLevel + 1, complement, maxLevel, ddVariableIndices, uniqueTableForLevels);
                    std::shared_ptr<Odd<DdType::CUDD>> thenNode = elseNode;
                    uint_fast64_t totalOffset = elseNode->getElseOffset() + elseNode->getThenOffset();
                    return std::shared_ptr<Odd<DdType::CUDD>>(new Odd<DdType::CUDD>(elseNode, totalOffset, thenNode, totalOffset));
                } else {
                    // Otherwise, we compute the ODDs for both the then- and else successors.
                    DdNode* thenDdNode = Cudd_T(dd);
                    DdNode* elseDdNode = Cudd_E(dd);
                    
                    // Determine whether we have to evaluate the successors as if they were complemented.
                    bool elseComplemented = Cudd_IsComplement(elseDdNode) ^ complement;
                    bool thenComplemented = Cudd_IsComplement(thenDdNode) ^ complement;
                    
                    std::shared_ptr<Odd<DdType::CUDD>> elseNode = buildOddFromBddRec(Cudd_Regular(elseDdNode), manager, currentLevel + 1, elseComplemented, maxLevel, ddVariableIndices, uniqueTableForLevels);
                    std::shared_ptr<Odd<DdType::CUDD>> thenNode = buildOddFromBddRec(Cudd_Regular(thenDdNode), manager, currentLevel + 1, thenComplemented, maxLevel, ddVariableIndices, uniqueTableForLevels);
                    
                    return std::shared_ptr<Odd<DdType::CUDD>>(new Odd<DdType::CUDD>(elseNode, elseNode->getElseOffset() + elseNode->getThenOffset(), thenNode, thenNode->getElseOffset() + thenNode->getThenOffset()));
                }
            }
        }
    }
}