#include <cstring>
#include <algorithm>

#include "src/storage/dd/CuddBdd.h"
#include "src/storage/dd/CuddAdd.h"
#include "src/storage/dd/CuddDdManager.h"

namespace storm {
    namespace dd {
        Bdd<DdType::CUDD>::Bdd(std::shared_ptr<DdManager<DdType::CUDD> const> ddManager, BDD cuddDd, std::set<storm::expressions::Variable> const& containedMetaVariables) : Dd<DdType::CUDD>(ddManager, containedMetaVariables) {
            // Intentionally left empty.
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
        }
        
        Bdd<DdType::CUDD> Bdd<DdType::CUDD>::operator&&(Bdd<DdType::CUDD> const& other) const {
            Dd<DdType::CUDD> result(*this);
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
            Dd<DdType::CUDD> cubeDd(this->getDdManager()->getBddOne());
            
            std::set<storm::expressions::Variable> newMetaVariables = this->getContainedMetaVariables();
            for (auto const& metaVariable : metaVariables) {
                // First check whether the BDD contains the meta variable and erase it, if this is the case.
                STORM_LOG_THROW(this->containsMetaVariable(metaVariable), storm::exceptions::InvalidArgumentException, "Cannot abstract from meta variable '" << metaVariable.getName() << "' that is not present in the DD.");
                newMetaVariables.erase(metaVariable);
                
                DdMetaVariable<DdType::CUDD> const& ddMetaVariable = this->getDdManager()->getMetaVariable(metaVariable);
                cubeDd &= ddMetaVariable.getCube();
            }
            
            return Bdd<DdType::CUDD>(this->getDdManager(), this->getCuddBdd().ExistAbstract(cubeDd.getCuddBdd()), newMetaVariables);
        }
        
        Dd<DdType::CUDD> Bdd<DdType::CUDD>::universalAbstract(std::set<storm::expressions::Variable> const& metaVariables) const {
            Dd<DdType::CUDD> cubeDd(this->getDdManager()->getBddOne());
            
            std::set<storm::expressions::Variable> newMetaVariables = this->getContainedMetaVariables();
            for (auto const& metaVariable : metaVariables) {
                // First check whether the BDD contains the meta variable and erase it, if this is the case.
                STORM_LOG_THROW(this->containsMetaVariable(metaVariable), storm::exceptions::InvalidArgumentException, "Cannot abstract from meta variable '" << metaVariable.getName() << "' that is not present in the DD.");
                newMetaVariables.erase(metaVariable);
                
                DdMetaVariable<DdType::CUDD> const& ddMetaVariable = this->getDdManager()->getMetaVariable(metaVariable);
                cubeDd &= ddMetaVariable.getCube();
            }
            
            return Bdd<DdType::CUDD>(this->getDdManager(), this->getCuddBdd().UnivAbstract(cubeDd.getCuddBdd()), newMetaVariables);
        }
        
        Dd<DdType::CUDD> Bdd<DdType::CUDD>::swapVariables(std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& metaVariablePairs) {
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
            STORM_LOG_WARN_COND(this->isBdd() && other.isBdd(), "Performing logical operation on MTBDD(s).");
            
            Dd<DdType::CUDD> cubeDd(this->getDdManager()->getOne());
            for (auto const& metaVariable : existentialVariables) {
                DdMetaVariable<DdType::CUDD> const& ddMetaVariable = this->getDdManager()->getMetaVariable(metaVariable);
                cubeDd &= ddMetaVariable.getCube();
            }
            
            std::set<storm::expressions::Variable> unionOfMetaVariables;
            std::set_union(this->getContainedMetaVariables().begin(), this->getContainedMetaVariables().end(), other.getContainedMetaVariables().begin(), other.getContainedMetaVariables().end(), std::inserter(unionOfMetaVariables, unionOfMetaVariables.begin()));
            std::set<storm::expressions::Variable> containedMetaVariables;
            std::set_difference(unionOfMetaVariables.begin(), unionOfMetaVariables.end(), existentialVariables.begin(), existentialVariables.end(), std::inserter(containedMetaVariables, containedMetaVariables.begin()));
            
            return Dd<DdType::CUDD>(this->getDdManager(), this->getCuddBdd().AndAbstract(other.getCuddBdd(), cubeDd.getCuddBdd()), containedMetaVariables);
        }
        
        Dd<DdType::CUDD> Bdd<DdType::CUDD>::constrain(Dd<DdType::CUDD> const& constraint) const {
            std::set<storm::expressions::Variable> metaVariables(this->getContainedMetaVariables());
            metaVariables.insert(constraint.getContainedMetaVariables().begin(), constraint.getContainedMetaVariables().end());
            
            if (this->isBdd() && constraint.isBdd()) {
                return Dd<DdType::CUDD>(this->getDdManager(), this->getCuddBdd().Constrain(constraint.getCuddBdd()), metaVariables);
            } else {
                return Dd<DdType::CUDD>(this->getDdManager(), this->getCuddMtbdd().Constrain(constraint.getCuddMtbdd()), metaVariables);
            }
        }
        
        Dd<DdType::CUDD> Bdd<DdType::CUDD>::restrict(Dd<DdType::CUDD> const& constraint) const {
            std::set<storm::expressions::Variable> metaVariables(this->getContainedMetaVariables());
            metaVariables.insert(constraint.getContainedMetaVariables().begin(), constraint.getContainedMetaVariables().end());
            
            if (this->isBdd() && constraint.isBdd()) {
                return Dd<DdType::CUDD>(this->getDdManager(), this->getCuddBdd().Restrict(constraint.getCuddBdd()), metaVariables);
            } else {
                return Dd<DdType::CUDD>(this->getDdManager(), this->getCuddMtbdd().Restrict(constraint.getCuddMtbdd()), metaVariables);
            }
        }
        
        Dd<DdType::CUDD> Bdd<DdType::CUDD>::getSupport() const {
            if (this->isBdd()) {
                return Dd<DdType::CUDD>(this->getDdManager(), this->getCuddBdd().Support(), this->getContainedMetaVariables());
            } else {
                return Dd<DdType::CUDD>(this->getDdManager(), this->getCuddMtbdd().Support(), this->getContainedMetaVariables());
            }
        }
        
        uint_fast64_t Bdd<DdType::CUDD>::getNonZeroCount() const {
            std::size_t numberOfDdVariables = 0;
            for (auto const& metaVariable : this->getContainedMetaVariables()) {
                numberOfDdVariables += this->getDdManager()->getMetaVariable(metaVariable).getNumberOfDdVariables();
            }
            if (this->isBdd()) {
                return static_cast<uint_fast64_t>(this->getCuddBdd().CountMinterm(static_cast<int>(numberOfDdVariables)));
            } else {
                return static_cast<uint_fast64_t>(this->getCuddMtbdd().CountMinterm(static_cast<int>(numberOfDdVariables)));
            }
        }
        
        uint_fast64_t Bdd<DdType::CUDD>::getLeafCount() const {
            if (this->isBdd()) {
                return static_cast<uint_fast64_t>(this->getCuddBdd().CountLeaves());
            } else {
                return static_cast<uint_fast64_t>(this->getCuddMtbdd().CountLeaves());
            }
        }
        
        uint_fast64_t Bdd<DdType::CUDD>::getNodeCount() const {
            if (this->isBdd()) {
                return static_cast<uint_fast64_t>(this->getCuddBdd().nodeCount());
            } else {
                return static_cast<uint_fast64_t>(this->getCuddMtbdd().nodeCount());
            }
        }
        
        bool Bdd<DdType::CUDD>::isOne() const {
            if (this->isBdd()) {
                return this->getCuddBdd().IsOne();
            } else {
                return this->getCuddMtbdd().IsOne();
            }
        }
        
        bool Bdd<DdType::CUDD>::isZero() const {
            if (this->isBdd()) {
                return this->getCuddBdd().IsZero();
            } else {
                return this->getCuddMtbdd().IsZero();
            }
        }
        
        uint_fast64_t Bdd<DdType::CUDD>::getIndex() const {
            if (this->isBdd()) {
                return static_cast<uint_fast64_t>(this->getCuddBdd().NodeReadIndex());
            } else {
                return static_cast<uint_fast64_t>(this->getCuddMtbdd().NodeReadIndex());
            }
        }
        
        std::vector<uint_fast64_t> Bdd<DdType::CUDD>::getSortedVariableIndices() const {
            std::vector<uint_fast64_t> ddVariableIndices;
            for (auto const& metaVariableName : this->getContainedMetaVariables()) {
                auto const& metaVariable = this->getDdManager()->getMetaVariable(metaVariableName);
                for (auto const& ddVariable : metaVariable.getDdVariables()) {
                    ddVariableIndices.push_back(ddVariable.getIndex());
                }
            }
            
            // Next, we need to sort them, since they may be arbitrarily ordered otherwise.
            std::sort(ddVariableIndices.begin(), ddVariableIndices.end());
            return ddVariableIndices;
        }
        
        void Bdd<DdType::CUDD>::exportToDot(std::string const& filename) const {
            if (filename.empty()) {
                if (this->isBdd()) {
                    std::vector<BDD> cuddBddVector = { this->getCuddBdd() };
                    this->getDdManager()->getCuddManager().DumpDot(cuddBddVector);
                } else {
                    std::vector<ADD> cuddAddVector = { this->getCuddMtbdd() };
                    this->getDdManager()->getCuddManager().DumpDot(cuddAddVector);
                }
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
                if (this->isBdd()) {
                    std::vector<BDD> cuddBddVector = { this->getCuddBdd() };
                    this->getDdManager()->getCuddManager().DumpDot(cuddBddVector, &ddVariableNames[0], &ddNames[0], filePointer);
                } else {
                    std::vector<ADD> cuddAddVector = { this->getCuddMtbdd() };
                    this->getDdManager()->getCuddManager().DumpDot(cuddAddVector, &ddVariableNames[0], &ddNames[0], filePointer);
                }
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
        
        std::ostream& operator<<(std::ostream& out, const Dd<DdType::CUDD>& dd) {
            dd.exportToDot();
            return out;
        }
    }
}