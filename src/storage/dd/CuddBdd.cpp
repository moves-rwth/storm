#include <cstring>
#include <algorithm>
#include <functional>

#include "src/storage/dd/CuddAdd.h"
#include "src/storage/dd/CuddBdd.h"
#include "src/storage/dd/CuddOdd.h"
#include "src/storage/dd/CuddDdManager.h"

#include "src/storage/BitVector.h"

#include "src/logic/ComparisonType.h"

#include "src/utility/macros.h"
#include "src/exceptions/InvalidArgumentException.h"

namespace storm {
    namespace dd {
        Bdd<DdType::CUDD>::Bdd(std::weak_ptr<DdManager<DdType::CUDD> const> ddManager, BDD cuddBdd, std::set<storm::expressions::Variable> const& containedMetaVariables) : Dd<DdType::CUDD>(ddManager, containedMetaVariables), cuddBdd(cuddBdd) {
            // Intentionally left empty.
        }
        
        Bdd<DdType::CUDD>::Bdd(std::weak_ptr<DdManager<DdType::CUDD> const> ddManager, std::vector<double> const& explicitValues, storm::dd::Odd<DdType::CUDD> const& odd, std::set<storm::expressions::Variable> const& metaVariables, storm::logic::ComparisonType comparisonType, double value) : Dd<DdType::CUDD>(ddManager, metaVariables) {
            switch (comparisonType) {
                case storm::logic::ComparisonType::Less:
                    this->cuddBdd = fromVector<double>(ddManager.lock(), explicitValues, odd, metaVariables, std::bind(std::greater<double>(), value, std::placeholders::_1));
                    break;
                case storm::logic::ComparisonType::LessEqual:
                    this->cuddBdd = fromVector<double>(ddManager.lock(), explicitValues, odd, metaVariables, std::bind(std::greater_equal<double>(), value, std::placeholders::_1));
                    break;
                case storm::logic::ComparisonType::Greater:
                    this->cuddBdd = fromVector<double>(ddManager.lock(), explicitValues, odd, metaVariables, std::bind(std::less<double>(), value, std::placeholders::_1));
                    break;
                case storm::logic::ComparisonType::GreaterEqual:
                    this->cuddBdd = fromVector<double>(ddManager.lock(), explicitValues, odd, metaVariables, std::bind(std::less_equal<double>(), value, std::placeholders::_1));
                    break;
            }
        }
        
        Add<DdType::CUDD> Bdd<DdType::CUDD>::toAdd() const {
            return Add<DdType::CUDD>(this->getDdManager(), this->getCuddBdd().Add(), this->getContainedMetaVariables());
        }
        
        BDD Bdd<DdType::CUDD>::getCuddBdd() const {
            return this->cuddBdd;
        }
        
        DdNode* Bdd<DdType::CUDD>::getCuddDdNode() const {
            return this->getCuddBdd().getNode();
        }
        
        bool Bdd<DdType::CUDD>::operator==(Bdd<DdType::CUDD> const& other) const {
            return this->getCuddBdd() == other.getCuddBdd();
        }
        
        bool Bdd<DdType::CUDD>::operator!=(Bdd<DdType::CUDD> const& other) const {
            return !(*this == other);
        }
        
        Bdd<DdType::CUDD> Bdd<DdType::CUDD>::ite(Bdd<DdType::CUDD> const& thenDd, Bdd<DdType::CUDD> const& elseDd) const {
            std::set<storm::expressions::Variable> metaVariableNames;
            std::set_union(thenDd.getContainedMetaVariables().begin(), thenDd.getContainedMetaVariables().end(), elseDd.getContainedMetaVariables().begin(), elseDd.getContainedMetaVariables().end(), std::inserter(metaVariableNames, metaVariableNames.begin()));
            metaVariableNames.insert(this->getContainedMetaVariables().begin(), this->getContainedMetaVariables().end());
            
            return Bdd<DdType::CUDD>(this->getDdManager(), this->getCuddBdd().Ite(thenDd.getCuddBdd(), elseDd.getCuddBdd()), metaVariableNames);
        }
        
        Bdd<DdType::CUDD> Bdd<DdType::CUDD>::operator!() const {
            Bdd<DdType::CUDD> result(*this);
            result.complement();
            return result;
        }
        
        Bdd<DdType::CUDD>& Bdd<DdType::CUDD>::complement() {
            this->cuddBdd = ~this->getCuddBdd();
            return *this;
        }
        
        Bdd<DdType::CUDD> Bdd<DdType::CUDD>::operator&&(Bdd<DdType::CUDD> const& other) const {
            Bdd<DdType::CUDD> result(*this);
            result &= other;
            return result;
        }
        
        Bdd<DdType::CUDD>& Bdd<DdType::CUDD>::operator&=(Bdd<DdType::CUDD> const& other) {
            this->addMetaVariables(other.getContainedMetaVariables());
            this->cuddBdd = this->getCuddBdd() & other.getCuddBdd();
            return *this;
        }
        
        Bdd<DdType::CUDD> Bdd<DdType::CUDD>::operator||(Bdd<DdType::CUDD> const& other) const {
            Bdd<DdType::CUDD> result(*this);
            result |= other;
            return result;
        }
        
        Bdd<DdType::CUDD>& Bdd<DdType::CUDD>::operator|=(Bdd<DdType::CUDD> const& other) {
            this->addMetaVariables(other.getContainedMetaVariables());
            this->cuddBdd = this->getCuddBdd() | other.getCuddBdd();
            return *this;
        }
        
        Bdd<DdType::CUDD> Bdd<DdType::CUDD>::iff(Bdd<DdType::CUDD> const& other) const {
            std::set<storm::expressions::Variable> metaVariables(this->getContainedMetaVariables());
            metaVariables.insert(other.getContainedMetaVariables().begin(), other.getContainedMetaVariables().end());
            return Bdd<DdType::CUDD>(this->getDdManager(), this->getCuddBdd().Xnor(other.getCuddBdd()), metaVariables);
        }
        
        Bdd<DdType::CUDD> Bdd<DdType::CUDD>::exclusiveOr(Bdd<DdType::CUDD> const& other) const {
            std::set<storm::expressions::Variable> metaVariables(this->getContainedMetaVariables());
            metaVariables.insert(other.getContainedMetaVariables().begin(), other.getContainedMetaVariables().end());
            return Bdd<DdType::CUDD>(this->getDdManager(), this->getCuddBdd().Xor(other.getCuddBdd()), metaVariables);
        }
        
        Bdd<DdType::CUDD> Bdd<DdType::CUDD>::implies(Bdd<DdType::CUDD> const& other) const {
            std::set<storm::expressions::Variable> metaVariables(this->getContainedMetaVariables());
            metaVariables.insert(other.getContainedMetaVariables().begin(), other.getContainedMetaVariables().end());
            return Bdd<DdType::CUDD>(this->getDdManager(), this->getCuddBdd().Ite(other.getCuddBdd(), this->getDdManager()->getBddOne().getCuddBdd()), metaVariables);
        }
        
        Bdd<DdType::CUDD> Bdd<DdType::CUDD>::existsAbstract(std::set<storm::expressions::Variable> const& metaVariables) const {
            Bdd<DdType::CUDD> cubeBdd = this->getDdManager()->getBddOne();
            
            std::set<storm::expressions::Variable> newMetaVariables = this->getContainedMetaVariables();
            for (auto const& metaVariable : metaVariables) {
                // First check whether the BDD contains the meta variable and erase it, if this is the case.
                STORM_LOG_THROW(this->containsMetaVariable(metaVariable), storm::exceptions::InvalidArgumentException, "Cannot abstract from meta variable '" << metaVariable.getName() << "' that is not present in the DD.");
                newMetaVariables.erase(metaVariable);
                
                DdMetaVariable<DdType::CUDD> const& ddMetaVariable = this->getDdManager()->getMetaVariable(metaVariable);
                cubeBdd &= ddMetaVariable.getCube();
            }
            
            return Bdd<DdType::CUDD>(this->getDdManager(), this->getCuddBdd().ExistAbstract(cubeBdd.getCuddBdd()), newMetaVariables);
        }
        
        Bdd<DdType::CUDD> Bdd<DdType::CUDD>::existsAbstractRepresentative(std::set<storm::expressions::Variable> const& metaVariables) const {
            Bdd<DdType::CUDD> cubeBdd = this->getDdManager()->getBddOne();
            
            std::set<storm::expressions::Variable> newMetaVariables = this->getContainedMetaVariables();
            for (auto const& metaVariable : metaVariables) {
                // First check whether the BDD contains the meta variable and erase it, if this is the case.
                STORM_LOG_THROW(this->containsMetaVariable(metaVariable), storm::exceptions::InvalidArgumentException, "Cannot abstract from meta variable '" << metaVariable.getName() << "' that is not present in the DD.");
                newMetaVariables.erase(metaVariable);
                
                DdMetaVariable<DdType::CUDD> const& ddMetaVariable = this->getDdManager()->getMetaVariable(metaVariable);
                cubeBdd &= ddMetaVariable.getCube();
            }
            
            return Bdd<DdType::CUDD>(this->getDdManager(), this->getCuddBdd().ExistAbstractRepresentative(cubeBdd.getCuddBdd()), newMetaVariables);
        }
        
        Bdd<DdType::CUDD> Bdd<DdType::CUDD>::universalAbstract(std::set<storm::expressions::Variable> const& metaVariables) const {
            Bdd<DdType::CUDD> cubeBdd = this->getDdManager()->getBddOne();
            
            std::set<storm::expressions::Variable> newMetaVariables = this->getContainedMetaVariables();
            for (auto const& metaVariable : metaVariables) {
                // First check whether the BDD contains the meta variable and erase it, if this is the case.
                STORM_LOG_THROW(this->containsMetaVariable(metaVariable), storm::exceptions::InvalidArgumentException, "Cannot abstract from meta variable '" << metaVariable.getName() << "' that is not present in the DD.");
                newMetaVariables.erase(metaVariable);
                
                DdMetaVariable<DdType::CUDD> const& ddMetaVariable = this->getDdManager()->getMetaVariable(metaVariable);
                cubeBdd &= ddMetaVariable.getCube();
            }
            
            return Bdd<DdType::CUDD>(this->getDdManager(), this->getCuddBdd().UnivAbstract(cubeBdd.getCuddBdd()), newMetaVariables);
        }
        
        Bdd<DdType::CUDD> Bdd<DdType::CUDD>::swapVariables(std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& metaVariablePairs) const {
            std::set<storm::expressions::Variable> newContainedMetaVariables;
            std::vector<BDD> from;
            std::vector<BDD> to;
            for (auto const& metaVariablePair : metaVariablePairs) {
                DdMetaVariable<DdType::CUDD> const& variable1 = this->getDdManager()->getMetaVariable(metaVariablePair.first);
                DdMetaVariable<DdType::CUDD> const& variable2 = this->getDdManager()->getMetaVariable(metaVariablePair.second);
                
                // Check if it's legal so swap the meta variables.
                STORM_LOG_THROW(variable1.getNumberOfDdVariables() == variable2.getNumberOfDdVariables(), storm::exceptions::InvalidArgumentException, "Unable to swap meta variables with different size.");
                
                // Keep track of the contained meta variables in the DD.
                if (this->containsMetaVariable(metaVariablePair.first)) {
                    newContainedMetaVariables.insert(metaVariablePair.second);
                }
                if (this->containsMetaVariable(metaVariablePair.second)) {
                    newContainedMetaVariables.insert(metaVariablePair.first);
                }
                
                // Add the variables to swap to the corresponding vectors.
                for (auto const& ddVariable : variable1.getDdVariables()) {
                    from.push_back(ddVariable.getCuddBdd());
                }
                for (auto const& ddVariable : variable2.getDdVariables()) {
                    to.push_back(ddVariable.getCuddBdd());
                }
            }
            
            // Finally, call CUDD to swap the variables.
            return Bdd<DdType::CUDD>(this->getDdManager(), this->getCuddBdd().SwapVariables(from, to), newContainedMetaVariables);
        }
        
        Bdd<DdType::CUDD> Bdd<DdType::CUDD>::andExists(Bdd<DdType::CUDD> const& other, std::set<storm::expressions::Variable> const& existentialVariables) const {
            Bdd<DdType::CUDD> cubeBdd(this->getDdManager()->getBddOne());
            for (auto const& metaVariable : existentialVariables) {
                DdMetaVariable<DdType::CUDD> const& ddMetaVariable = this->getDdManager()->getMetaVariable(metaVariable);
                cubeBdd &= ddMetaVariable.getCube();
            }
            
            std::set<storm::expressions::Variable> unionOfMetaVariables;
            std::set_union(this->getContainedMetaVariables().begin(), this->getContainedMetaVariables().end(), other.getContainedMetaVariables().begin(), other.getContainedMetaVariables().end(), std::inserter(unionOfMetaVariables, unionOfMetaVariables.begin()));
            std::set<storm::expressions::Variable> containedMetaVariables;
            std::set_difference(unionOfMetaVariables.begin(), unionOfMetaVariables.end(), existentialVariables.begin(), existentialVariables.end(), std::inserter(containedMetaVariables, containedMetaVariables.begin()));
            
            return Bdd<DdType::CUDD>(this->getDdManager(), this->getCuddBdd().AndAbstract(other.getCuddBdd(), cubeBdd.getCuddBdd()), containedMetaVariables);
        }
        
        Bdd<DdType::CUDD> Bdd<DdType::CUDD>::constrain(Bdd<DdType::CUDD> const& constraint) const {
            std::set<storm::expressions::Variable> metaVariables;
            std::set_union(this->getContainedMetaVariables().begin(), this->getContainedMetaVariables().end(), constraint.getContainedMetaVariables().begin(), constraint.getContainedMetaVariables().end(), std::inserter(metaVariables, metaVariables.begin()));
            return Bdd<DdType::CUDD>(this->getDdManager(), this->getCuddBdd().Constrain(constraint.getCuddBdd()), metaVariables);
        }
        
        Bdd<DdType::CUDD> Bdd<DdType::CUDD>::restrict(Bdd<DdType::CUDD> const& constraint) const {
            std::set<storm::expressions::Variable> metaVariables;
            std::set_union(this->getContainedMetaVariables().begin(), this->getContainedMetaVariables().end(), constraint.getContainedMetaVariables().begin(), constraint.getContainedMetaVariables().end(), std::inserter(metaVariables, metaVariables.begin()));
            return Bdd<DdType::CUDD>(this->getDdManager(), this->getCuddBdd().Restrict(constraint.getCuddBdd()), metaVariables);
        }
        
        Bdd<DdType::CUDD> Bdd<DdType::CUDD>::getSupport() const {
            return Bdd<DdType::CUDD>(this->getDdManager(), this->getCuddBdd().Support(), this->getContainedMetaVariables());
        }
        
        uint_fast64_t Bdd<DdType::CUDD>::getNonZeroCount() const {
            std::size_t numberOfDdVariables = 0;
            for (auto const& metaVariable : this->getContainedMetaVariables()) {
                numberOfDdVariables += this->getDdManager()->getMetaVariable(metaVariable).getNumberOfDdVariables();
            }
            return static_cast<uint_fast64_t>(this->getCuddBdd().CountMinterm(static_cast<int>(numberOfDdVariables)));
        }
                
        uint_fast64_t Bdd<DdType::CUDD>::getLeafCount() const {
            return static_cast<uint_fast64_t>(this->getCuddBdd().CountLeaves());
        }
        
        uint_fast64_t Bdd<DdType::CUDD>::getNodeCount() const {
            return static_cast<uint_fast64_t>(this->getCuddBdd().nodeCount());
        }
        
        bool Bdd<DdType::CUDD>::isOne() const {
            return this->getCuddBdd().IsOne();
        }
        
        bool Bdd<DdType::CUDD>::isZero() const {
            return this->getCuddBdd().IsZero();
        }
        
        uint_fast64_t Bdd<DdType::CUDD>::getIndex() const {
            return static_cast<uint_fast64_t>(this->getCuddBdd().NodeReadIndex());
        }
        
        uint_fast64_t Bdd<DdType::CUDD>::getLevel() const {
            return static_cast<uint_fast64_t>(this->getDdManager()->getCuddManager().ReadPerm(this->getIndex()));
        }
        
        void Bdd<DdType::CUDD>::exportToDot(std::string const& filename) const {
            if (filename.empty()) {
                std::vector<BDD> cuddBddVector = { this->getCuddBdd() };
                this->getDdManager()->getCuddManager().DumpDot(cuddBddVector);
            } else {
                // Build the name input of the DD.
                std::vector<char*> ddNames;
                std::string ddName("f");
                ddNames.push_back(new char[ddName.size() + 1]);
                std::copy(ddName.c_str(), ddName.c_str() + 2, ddNames.back());
                
                // Now build the variables names.
                std::vector<std::string> ddVariableNamesAsStrings = this->getDdManager()->getDdVariableNames();
                std::vector<char*> ddVariableNames;
                for (auto const& element : ddVariableNamesAsStrings) {
                    ddVariableNames.push_back(new char[element.size() + 1]);
                    std::copy(element.c_str(), element.c_str() + element.size() + 1, ddVariableNames.back());
                }
                
                // Open the file, dump the DD and close it again.
                FILE* filePointer = fopen(filename.c_str() , "w");
                std::vector<BDD> cuddBddVector = { this->getCuddBdd() };
                this->getDdManager()->getCuddManager().DumpDot(cuddBddVector, &ddVariableNames[0], &ddNames[0], filePointer);
                fclose(filePointer);
                
                // Finally, delete the names.
                for (char* element : ddNames) {
                    delete element;
                }
                for (char* element : ddVariableNames) {
                    delete element;
                }
            }
        }
        
        template<typename ValueType>
        BDD Bdd<DdType::CUDD>::fromVector(std::shared_ptr<DdManager<DdType::CUDD> const> ddManager, std::vector<ValueType> const& values, Odd<DdType::CUDD> const& odd, std::set<storm::expressions::Variable> const& metaVariables, std::function<bool (ValueType const&)> const& filter) {
            std::vector<uint_fast64_t> ddVariableIndices = getSortedVariableIndices(*ddManager, metaVariables);
            uint_fast64_t offset = 0;
            return BDD(ddManager->getCuddManager(), fromVectorRec(ddManager->getCuddManager().getManager(), offset, 0, ddVariableIndices.size(), values, odd, ddVariableIndices, filter));
        }
        
        template<typename ValueType>
        DdNode* Bdd<DdType::CUDD>::fromVectorRec(::DdManager* manager, uint_fast64_t& currentOffset, uint_fast64_t currentLevel, uint_fast64_t maxLevel, std::vector<ValueType> const& values, Odd<DdType::CUDD> const& odd, std::vector<uint_fast64_t> const& ddVariableIndices, std::function<bool (ValueType const&)> const& filter) {
            if (currentLevel == maxLevel) {
                // If we are in a terminal node of the ODD, we need to check whether the then-offset of the ODD is one
                // (meaning the encoding is a valid one) or zero (meaning the encoding is not valid). Consequently, we
                // need to copy the next value of the vector iff the then-offset is greater than zero.
                if (odd.getThenOffset() > 0) {
                    if (filter(values[currentOffset++])) {
                        return Cudd_ReadOne(manager);
                    } else {
                        return Cudd_ReadLogicZero(manager);
                    }
                } else {
                    return Cudd_ReadZero(manager);
                }
            } else {
                // If the total offset is zero, we can just return the constant zero DD.
                if (odd.getThenOffset() + odd.getElseOffset() == 0) {
                    return Cudd_ReadZero(manager);
                }
                
                // Determine the new else-successor.
                DdNode* elseSuccessor = nullptr;
                if (odd.getElseOffset() > 0) {
                    elseSuccessor = fromVectorRec(manager, currentOffset, currentLevel + 1, maxLevel, values, odd.getElseSuccessor(), ddVariableIndices, filter);
                } else {
                    elseSuccessor = Cudd_ReadLogicZero(manager);
                }
                Cudd_Ref(elseSuccessor);
                
                // Determine the new then-successor.
                DdNode* thenSuccessor = nullptr;
                if (odd.getThenOffset() > 0) {
                    thenSuccessor = fromVectorRec(manager, currentOffset, currentLevel + 1, maxLevel, values, odd.getThenSuccessor(), ddVariableIndices, filter);
                } else {
                    thenSuccessor = Cudd_ReadLogicZero(manager);
                }
                Cudd_Ref(thenSuccessor);
                
                // Create a node representing ITE(currentVar, thenSuccessor, elseSuccessor);
                DdNode* result = Cudd_bddIthVar(manager, static_cast<int>(ddVariableIndices[currentLevel]));
                Cudd_Ref(result);
                DdNode* newResult = Cudd_bddIte(manager, result, thenSuccessor, elseSuccessor);
                Cudd_Ref(newResult);
                
                // Dispose of the intermediate results
                Cudd_RecursiveDeref(manager, result);
                Cudd_RecursiveDeref(manager, thenSuccessor);
                Cudd_RecursiveDeref(manager, elseSuccessor);
                
                // Before returning, we remove the protection imposed by the previous call to Cudd_Ref.
                Cudd_Deref(newResult);
                
                return newResult;
            }
        }
        
        storm::storage::BitVector Bdd<DdType::CUDD>::toVector(storm::dd::Odd<DdType::CUDD> const& rowOdd) const {
            std::vector<uint_fast64_t> ddVariableIndices = this->getSortedVariableIndices();
            storm::storage::BitVector result(rowOdd.getTotalOffset());
            this->toVectorRec(this->getCuddDdNode(), this->getDdManager()->getCuddManager(), result, rowOdd, Cudd_IsComplement(this->getCuddDdNode()), 0, ddVariableIndices.size(), 0, ddVariableIndices);
            return result;
        }
        
        std::pair<std::vector<storm::expressions::Expression>, std::unordered_map<std::pair<uint_fast64_t, uint_fast64_t>, storm::expressions::Variable>> Bdd<DdType::CUDD>::toExpression(storm::expressions::ExpressionManager& manager, std::unordered_map<uint_fast64_t, storm::expressions::Expression> const& indexToExpressionMap) const {
            std::pair<std::vector<storm::expressions::Expression>, std::unordered_map<std::pair<uint_fast64_t, uint_fast64_t>, storm::expressions::Variable>> result;
            
            // Create (and maintain) a mapping from the DD nodes to a counter that says the how-many-th node (within the
            // nodes of equal index) the node was.
            std::unordered_map<DdNode*, uint_fast64_t> nodeToCounterMap;
            this->toExpressionRec(this->getCuddDdNode(), this->getDdManager()->getCuddManager(), manager, result.first, result.second, nodeToCounterMap);
            
            return result;
        }
        
        void Bdd<DdType::CUDD>::toExpressionRec(DdNode const* f, Cudd const& ddManager, storm::expressions::ExpressionManager& manager, std::vector<storm::expressions::Expression>& expressions, std::unordered_map<std::pair<uint_fast64_t, uint_fast64_t>, storm::expressions::Variable>& countIndexToVariablePair, std::unordered_map<DdNode*, uint_fast64_t>& nodeToCounterMap) const {
            DdNode const* F = Cudd_Regular(f);
            
            // Terminal cases.
            if (F == Cudd_ReadOne(ddManager.getManager())) {
                
            }
            
            // Non-terminal cases.
            // (1) Check if we have seen the node before.
            auto nodeIt = nodeToCounterMap.find();
        }
        
        void Bdd<DdType::CUDD>::toVectorRec(DdNode const* dd, Cudd const& manager, storm::storage::BitVector& result, Odd<DdType::CUDD> const& rowOdd, bool complement, uint_fast64_t currentRowLevel, uint_fast64_t maxLevel, uint_fast64_t currentRowOffset, std::vector<uint_fast64_t> const& ddRowVariableIndices) const {
            // If there are no more values to select, we can directly return.
            if (dd == Cudd_ReadLogicZero(manager.getManager()) && !complement) {
                return;
            } else if (dd == Cudd_ReadOne(manager.getManager()) && complement) {
                return;
            }
            
            // If we are at the maximal level, the value to be set is stored as a constant in the DD.
            if (currentRowLevel == maxLevel) {
                result.set(currentRowOffset, true);
            } else if (ddRowVariableIndices[currentRowLevel] < dd->index) {
                toVectorRec(dd, manager, result, rowOdd.getElseSuccessor(), complement, currentRowLevel + 1, maxLevel, currentRowOffset, ddRowVariableIndices);
                toVectorRec(dd, manager, result, rowOdd.getThenSuccessor(), complement, currentRowLevel + 1, maxLevel, currentRowOffset + rowOdd.getElseOffset(), ddRowVariableIndices);
            } else {
                // Otherwise, we compute the ODDs for both the then- and else successors.
                DdNode* elseDdNode = Cudd_E(dd);
                DdNode* thenDdNode = Cudd_T(dd);
                
                // Determine whether we have to evaluate the successors as if they were complemented.
                bool elseComplemented = Cudd_IsComplement(elseDdNode) ^ complement;
                bool thenComplemented = Cudd_IsComplement(thenDdNode) ^ complement;
                
                toVectorRec(Cudd_Regular(elseDdNode), manager, result, rowOdd.getElseSuccessor(), elseComplemented, currentRowLevel + 1, maxLevel, currentRowOffset, ddRowVariableIndices);
                toVectorRec(Cudd_Regular(thenDdNode), manager, result, rowOdd.getThenSuccessor(), thenComplemented, currentRowLevel + 1, maxLevel, currentRowOffset + rowOdd.getElseOffset(), ddRowVariableIndices);
            }
        }
        
        std::ostream& operator<<(std::ostream& out, const Bdd<DdType::CUDD>& bdd) {
            bdd.exportToDot();
            return out;
        }
    }
}