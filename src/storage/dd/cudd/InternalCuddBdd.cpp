#include "src/storage/dd/cudd/InternalCuddBdd.h"

namespace storm {
    namespace dd {
        bool InternalBdd<DdType::CUDD>::operator==(InternalBdd<DdType::CUDD> const& other) const {
            return this->getCuddBdd() == other.getCuddBdd();
        }
        
        bool InternalBdd<DdType::CUDD>::operator!=(InternalBdd<DdType::CUDD> const& other) const {
            return !(*this == other);
        }
        
        InternalBdd<DdType::CUDD> InternalBdd<DdType::CUDD>::ite(InternalBdd<DdType::CUDD> const& thenDd, InternalBdd<DdType::CUDD> const& elseDd) const {
            return InternalBdd<DdType::CUDD>(this->getDdManager(), this->getCuddBdd().Ite(thenDd.getCuddBdd(), elseDd.getCuddBdd()), metaVariableNames);
        }
        
        InternalBdd<DdType::CUDD> InternalBdd<DdType::CUDD>::operator||(InternalBdd<DdType::CUDD> const& other) const {
            InternalBdd<DdType::CUDD> result(*this);
            result |= other;
            return result;
        }
        
        InternalBdd<DdType::CUDD>& InternalBdd<DdType::CUDD>::operator|=(InternalBdd<DdType::CUDD> const& other) {
            this->cuddBdd = this->getCuddBdd() | other.getCuddBdd();
            return *this;
        }
        
        InternalBdd<DdType::CUDD> InternalBdd<DdType::CUDD>::operator&&(InternalBdd<DdType::CUDD> const& other) const {
            InternalBdd<DdType::CUDD> result(*this);
            result &= other;
            return result;
        }
        
        InternalBdd<DdType::CUDD>& InternalBdd<DdType::CUDD>::operator&=(InternalBdd<DdType::CUDD> const& other) {
            this->cuddBdd = this->getCuddBdd() & other.getCuddBdd();
            return *this;
        }
        
        InternalBdd<DdType::CUDD> InternalBdd<DdType::CUDD>::iff(InternalBdd<DdType::CUDD> const& other) const {
            return InternalBdd<DdType::CUDD>(this->getDdManager(), this->getCuddBdd().Xnor(other.getCuddBdd()), metaVariables);
        }
        
        InternalBdd<DdType::CUDD> InternalBdd<DdType::CUDD>::exclusiveOr(InternalBdd<DdType::CUDD> const& other) const {
            return InternalBdd<DdType::CUDD>(this->getDdManager(), this->getCuddBdd().Xor(other.getCuddBdd()), metaVariables);
        }
        
        InternalBdd<DdType::CUDD> InternalBdd<DdType::CUDD>::implies(InternalBdd<DdType::CUDD> const& other) const {
            return InternalBdd<DdType::CUDD>(this->getDdManager(), this->getCuddBdd().Ite(other.getCuddBdd(), this->getDdManager()->getBddOne().getCuddBdd()), metaVariables);
        }
        
        InternalBdd<DdType::CUDD> InternalBdd<DdType::CUDD>::operator!() const {
            InternalBdd<DdType::CUDD> result(*this);
            result.complement();
            return result;
        }
        
        InternalBdd<DdType::CUDD>& InternalBdd<DdType::CUDD>::complement() {
            this->cuddBdd = ~this->getCuddBdd();
            return *this;
        }
        
        InternalBdd<DdType::CUDD> InternalBdd<DdType::CUDD>::existsAbstract(InternalBdd<DdType> const& cube) const {
            return InternalBdd<DdType::CUDD>(this->getDdManager(), this->getCuddBdd().ExistAbstract(cube.getCuddBdd()), newMetaVariables);
        }
        
        InternalBdd<DdType::CUDD> InternalBdd<DdType::CUDD>::universalAbstract(InternalBdd<DdType> const& cube) const {
            return InternalBdd<DdType::CUDD>(this->getDdManager(), this->getCuddBdd().UnivAbstract(cube.getCuddBdd()), newMetaVariables);
        }
        
        InternalBdd<DdType::CUDD> InternalBdd<DdType::CUDD>::andExists(InternalBdd<DdType::CUDD> const& other, InternalBdd<DdType> const& cube) const {
            return InternalBdd<DdType::CUDD>(this->getDdManager(), this->getCuddBdd().AndAbstract(other.getCuddBdd(), cube.getCuddBdd()), containedMetaVariables);
        }
        
        InternalBdd<DdType::CUDD> InternalBdd<DdType::CUDD>::constrain(InternalBdd<DdType::CUDD> const& constraint) const {
            return InternalBdd<DdType::CUDD>(this->getDdManager(), this->getCuddBdd().Constrain(constraint.getCuddBdd()), metaVariables);
        }
        
        InternalBdd<DdType::CUDD> InternalBdd<DdType::CUDD>::restrict(InternalBdd<DdType::CUDD> const& constraint) const {
            return InternalBdd<DdType::CUDD>(this->getDdManager(), this->getCuddBdd().Restrict(constraint.getCuddBdd()), metaVariables);
        }
        
        InternalBdd<DdType::CUDD> InternalBdd<DdType::CUDD>::swapVariables(std::vector<std::pair<std::reference_wrapper<DdMetaVariable<LibraryType> const>, std::reference_wrapper<DdMetaVariable<LibraryType> const>>> const& fromTo) const {
            std::vector<BDD> fromBdd;
            std::vector<BDD> toBdd;
            for (auto const& metaVariablePair : fromTo) {
                DdMetaVariable<DdType::CUDD> const& variable1 = metaVariablePair.first.get();
                DdMetaVariable<DdType::CUDD> const& variable2 = metaVariablePair.second.get();
                
                // Add the variables to swap to the corresponding vectors.
                for (auto const& ddVariable : variable1.getDdVariables()) {
                    fromBdd.push_back(ddVariable.getCuddBdd());
                }
                for (auto const& ddVariable : variable2.getDdVariables()) {
                    toBdd.push_back(ddVariable.getCuddBdd());
                }
            }
            
            // Finally, call CUDD to swap the variables.
            return InternalBdd<DdType::CUDD>(this->getDdManager(), this->getCuddBdd().SwapVariables(fromBdd, toBdd), newContainedMetaVariables);
        }
        
        InternalBdd<DdType::CUDD> InternalBdd<DdType::CUDD>::getSupport() const {
            return InternalBdd<DdType::CUDD>(this->getDdManager(), this->getCuddBdd().Support(), this->getContainedMetaVariables());
        }
        
        uint_fast64_t InternalBdd<DdType::CUDD>::getNonZeroCount(uint_fast64_t numberOfDdVariables) const {
            return static_cast<uint_fast64_t>(this->getCuddBdd().CountMinterm(static_cast<int>(numberOfDdVariables)));
        }
        
        uint_fast64_t InternalBdd<DdType::CUDD>::getLeafCount() const {
            return static_cast<uint_fast64_t>(this->getCuddBdd().CountLeaves());
        }
        
        uint_fast64_t InternalBdd<DdType::CUDD>::getNodeCount() const {
            return static_cast<uint_fast64_t>(this->getCuddBdd().nodeCount());
        }
        
        bool InternalBdd<DdType::CUDD>::isOne() const {
            return this->getCuddBdd().IsOne();
        }
        
        bool InternalBdd<DdType::CUDD>::isZero() const {
            return this->getCuddBdd().IsZero();
        }
        
        uint_fast64_t InternalBdd<DdType::CUDD>::getIndex() const {
            return static_cast<uint_fast64_t>(this->getCuddBdd().NodeReadIndex());
        }
        
        void InternalBdd<DdType::CUDD>::exportToDot(std::string const& filename, std::vector<std::string> const& ddVariableNamesAsStrings) const {
            // Build the name input of the DD.
            std::vector<char*> ddNames;
            std::string ddName("f");
            ddNames.push_back(new char[ddName.size() + 1]);
            std::copy(ddName.c_str(), ddName.c_str() + 2, ddNames.back());
            
            // Now build the variables names.
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
        
        BDD InternalBdd<DdType::CUDD>::getCuddBdd() const {
            return this->cuddBdd;
        }
        
        DdNode* InternalBdd<DdType::CUDD>::getCuddDdNode() const {
            return this->getCuddBdd().getNode();
        }
        

        
        
        
        
        
        
        Add<DdType::CUDD> InternalBdd<DdType::CUDD>::toAdd() const {
            return Add<DdType::CUDD>(this->getDdManager(), this->getCuddBdd().Add(), this->getContainedMetaVariables());
        }
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        template<typename ValueType>
        BDD InternalBdd<DdType::CUDD>::fromVector(std::shared_ptr<DdManager<DdType::CUDD> const> ddManager, std::vector<ValueType> const& values, Odd<DdType::CUDD> const& odd, std::set<storm::expressions::Variable> const& metaVariables, std::function<bool (ValueType const&)> const& filter) {
            std::vector<uint_fast64_t> ddVariableIndices = getSortedVariableIndices(*ddManager, metaVariables);
            uint_fast64_t offset = 0;
            return BDD(ddManager->getCuddManager(), fromVectorRec(ddManager->getCuddManager().getManager(), offset, 0, ddVariableIndices.size(), values, odd, ddVariableIndices, filter));
        }
        
        template<typename ValueType>
        DdNode* InternalBdd<DdType::CUDD>::fromVectorRec(::DdManager* manager, uint_fast64_t& currentOffset, uint_fast64_t currentLevel, uint_fast64_t maxLevel, std::vector<ValueType> const& values, Odd<DdType::CUDD> const& odd, std::vector<uint_fast64_t> const& ddVariableIndices, std::function<bool (ValueType const&)> const& filter) {
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
        
        storm::storage::BitVector InternalBdd<DdType::CUDD>::toVector(storm::dd::Odd<DdType::CUDD> const& rowOdd) const {
            std::vector<uint_fast64_t> ddVariableIndices = this->getSortedVariableIndices();
            storm::storage::BitVector result(rowOdd.getTotalOffset());
            this->toVectorRec(this->getCuddDdNode(), this->getDdManager()->getCuddManager(), result, rowOdd, Cudd_IsComplement(this->getCuddDdNode()), 0, ddVariableIndices.size(), 0, ddVariableIndices);
            return result;
        }
        
        void InternalBdd<DdType::CUDD>::toVectorRec(DdNode const* dd, Cudd const& manager, storm::storage::BitVector& result, Odd<DdType::CUDD> const& rowOdd, bool complement, uint_fast64_t currentRowLevel, uint_fast64_t maxLevel, uint_fast64_t currentRowOffset, std::vector<uint_fast64_t> const& ddRowVariableIndices) const {
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
        
        InternalBdd<DdType::CUDD>::InternalBdd(std::shared_ptr<DdManager<DdType::CUDD> const> ddManager, BDD cuddBdd, std::set<storm::expressions::Variable> const& containedMetaVariables) : Dd<DdType::CUDD>(ddManager, containedMetaVariables), cuddBdd(cuddBdd) {
            // Intentionally left empty.
        }
        
        InternalBdd<DdType::CUDD>::InternalBdd(std::shared_ptr<DdManager<DdType::CUDD> const> ddManager, std::vector<double> const& explicitValues, storm::dd::Odd<DdType::CUDD> const& odd, std::set<storm::expressions::Variable> const& metaVariables, storm::logic::ComparisonType comparisonType, double value) : Dd<DdType::CUDD>(ddManager, metaVariables) {
            switch (comparisonType) {
                case storm::logic::ComparisonType::Less:
                    this->cuddBdd = fromVector<double>(ddManager, explicitValues, odd, metaVariables, std::bind(std::greater<double>(), value, std::placeholders::_1));
                    break;
                case storm::logic::ComparisonType::LessEqual:
                    this->cuddBdd = fromVector<double>(ddManager, explicitValues, odd, metaVariables, std::bind(std::greater_equal<double>(), value, std::placeholders::_1));
                    break;
                case storm::logic::ComparisonType::Greater:
                    this->cuddBdd = fromVector<double>(ddManager, explicitValues, odd, metaVariables, std::bind(std::less<double>(), value, std::placeholders::_1));
                    break;
                case storm::logic::ComparisonType::GreaterEqual:
                    this->cuddBdd = fromVector<double>(ddManager, explicitValues, odd, metaVariables, std::bind(std::less_equal<double>(), value, std::placeholders::_1));
                    break;
            }
            
        }
        
    }
}