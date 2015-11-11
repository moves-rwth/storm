#include "src/storage/dd/Bdd.h"
#include "src/storage/dd/Add.h"
#include "src/storage/dd/Odd.h"

#include "src/storage/dd/DdMetaVariable.h"
#include "src/storage/dd/DdManager.h"

#include "src/storage/BitVector.h"

#include "src/utility/macros.h"
#include "src/exceptions/InvalidArgumentException.h"

namespace storm {
    namespace dd {
        
        template<DdType LibraryType>
        Bdd<LibraryType>::Bdd(std::shared_ptr<DdManager<DdType::CUDD> const> ddManager, InternalBdd<LibraryType> const& internalBdd, std::set<storm::expressions::Variable> const& containedMetaVariables) : Dd<LibraryType>(ddManager, containedMetaVariables), internalBdd(internalBdd) {
            // Intentionally left empty.
        }
        
        template<DdType LibraryType>
        Bdd<LibraryType>::Bdd(std::shared_ptr<DdManager<LibraryType> const> ddManager, std::vector<double> const& explicitValues, storm::dd::Odd<LibraryType> const& odd, std::set<storm::expressions::Variable> const& metaVariables, storm::logic::ComparisonType comparisonType, double value) {
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
        
        template<DdType LibraryType>
        bool Bdd<LibraryType>::operator==(Bdd<LibraryType> const& other) const {
            return internalBdd == other.internalBdd;
        }
    
        template<DdType LibraryType>
        bool Bdd<LibraryType>::operator!=(Bdd<LibraryType> const& other) const {
            return internalBdd != other.internalBdd;
        }
        
        template<DdType LibraryType>
        Bdd<LibraryType> Bdd<LibraryType>::ite(Bdd<LibraryType> const& thenBdd, Bdd<LibraryType> const& elseBdd) const {
            std::set<storm::expressions::Variable> metaVariables = Dd<LibraryType>::joinMetaVariables(*this, thenBdd);
            metaVariables.insert(this->getContainedMetaVariables().begin(), this->getContainedMetaVariables().end());
            return Bdd<LibraryType>(this->getDdManager(), internalBdd.ite(thenBdd.internalBdd, elseBdd.internalBdd), metaVariables);
        }
        
        template<DdType LibraryType>
        Bdd<LibraryType> Bdd<LibraryType>::operator||(Bdd<LibraryType> const& other) const {
            return Bdd<LibraryType>(this->getDdManager(), internalBdd || other.internalBdd, Dd<LibraryType>::joinMetaVariables(*this, other));
        }
        
        template<DdType LibraryType>
        Bdd<LibraryType>& Bdd<LibraryType>::operator|=(Bdd<LibraryType> const& other) {
            this->addMetaVariables(other.getContainedMetaVariables());
            internalBdd |= other.internalBdd;
            return *this;
        }

        template<DdType LibraryType>
        Bdd<LibraryType> Bdd<LibraryType>::operator&&(Bdd<LibraryType> const& other) const {
            return Bdd<LibraryType>(this->getDdManager(), internalBdd && other.internalBdd, Dd<LibraryType>::joinMetaVariables(*this, other));
        }

        template<DdType LibraryType>
        Bdd<LibraryType>& Bdd<LibraryType>::operator&=(Bdd<LibraryType> const& other) {
            this->addMetaVariables(other.getContainedMetaVariables());
            internalBdd &= other.internalBdd;
            return *this;
        }
        
        template<DdType LibraryType>
        Bdd<LibraryType> Bdd<LibraryType>::iff(Bdd<LibraryType> const& other) const {
            return Bdd<LibraryType>(this->getDdManager(), internalBdd.iff(other.internalBdd), Dd<LibraryType>::joinMetaVariables(*this, other));
        }
        
        template<DdType LibraryType>
        Bdd<LibraryType> Bdd<LibraryType>::exclusiveOr(Bdd<LibraryType> const& other) const {
            return Bdd<LibraryType>(this->getDdManager(), internalBdd.exclusiveOr(other.internalBdd), Dd<LibraryType>::joinMetaVariables(*this, other));
        }

        template<DdType LibraryType>
        Bdd<LibraryType> Bdd<LibraryType>::implies(Bdd<LibraryType> const& other) const {
            return Bdd<LibraryType>(this->getDdManager(), internalBdd.implies(other.internalBdd), Dd<LibraryType>::joinMetaVariables(*this, other));
        }
   
        template<DdType LibraryType>
        Bdd<LibraryType> Bdd<LibraryType>::operator!() const {
            return Bdd<LibraryType>(this->getDdManager(), !internalBdd, this->getContainedMetaVariables());
        }

        template<DdType LibraryType>
        Bdd<LibraryType>& Bdd<LibraryType>::complement() {
            internalBdd.complement();
            return *this;
        }

        template<DdType LibraryType>
        Bdd<LibraryType> Bdd<LibraryType>::existsAbstract(std::set<storm::expressions::Variable> const& metaVariables) const {
            Bdd<LibraryType> cubeBdd = this->getDdManager()->getBddOne();
            
            std::set<storm::expressions::Variable> newMetaVariables = this->getContainedMetaVariables();
            for (auto const& metaVariable : metaVariables) {
                // First check whether the BDD contains the meta variable and erase it, if this is the case.
                STORM_LOG_THROW(this->containsMetaVariable(metaVariable), storm::exceptions::InvalidArgumentException, "Cannot abstract from meta variable '" << metaVariable.getName() << "' that is not present in the DD.");
                newMetaVariables.erase(metaVariable);
                
                DdMetaVariable<DdType::CUDD> const& ddMetaVariable = this->getDdManager()->getMetaVariable(metaVariable);
                cubeBdd &= ddMetaVariable.getCube();
            }
            
            return Bdd<LibraryType>(internalBdd.existsAbstract(cubeBdd));
        }

        template<DdType LibraryType>
        Bdd<LibraryType> Bdd<LibraryType>::universalAbstract(std::set<storm::expressions::Variable> const& metaVariables) const {
            InternalBdd<LibraryType> cubeBdd = this->getDdManager()->getBddOne();
            
            std::set<storm::expressions::Variable> newMetaVariables = this->getContainedMetaVariables();
            for (auto const& metaVariable : metaVariables) {
                // First check whether the BDD contains the meta variable and erase it, if this is the case.
                STORM_LOG_THROW(this->containsMetaVariable(metaVariable), storm::exceptions::InvalidArgumentException, "Cannot abstract from meta variable '" << metaVariable.getName() << "' that is not present in the DD.");
                newMetaVariables.erase(metaVariable);
                
                DdMetaVariable<DdType::CUDD> const& ddMetaVariable = this->getDdManager()->getMetaVariable(metaVariable);
                cubeBdd &= ddMetaVariable.getCube();
            }
            
            return Bdd<LibraryType>(internalBdd.universalAbstract(cubeBdd));
        }
        
        template<DdType LibraryType>
        Bdd<LibraryType> Bdd<LibraryType>::andExists(Bdd<LibraryType> const& other, std::set<storm::expressions::Variable> const& existentialVariables) const {
            InternalBdd<DdType::CUDD> cubeBdd(this->getDdManager()->getBddOne());
            for (auto const& metaVariable : existentialVariables) {
                DdMetaVariable<DdType::CUDD> const& ddMetaVariable = this->getDdManager()->getMetaVariable(metaVariable);
                cubeBdd &= ddMetaVariable.getCube();
            }

            std::set<storm::expressions::Variable> unionOfMetaVariables;
            std::set_union(this->getContainedMetaVariables().begin(), this->getContainedMetaVariables().end(), other.getContainedMetaVariables().begin(), other.getContainedMetaVariables().end(), std::inserter(unionOfMetaVariables, unionOfMetaVariables.begin()));
            std::set<storm::expressions::Variable> containedMetaVariables;
            std::set_difference(unionOfMetaVariables.begin(), unionOfMetaVariables.end(), existentialVariables.begin(), existentialVariables.end(), std::inserter(containedMetaVariables, containedMetaVariables.begin()));
            
            return Bdd<LibraryType>(internalBdd.andExists(other.internalBdd, cubeBdd));
        }
        
        template<DdType LibraryType>
        Bdd<LibraryType> Bdd<LibraryType>::constrain(Bdd<LibraryType> const& constraint) const {
            this->addMetaVariables(constraint.getContainedMetaVariables());
            return internalBdd.constraint(constraint.internalBdd);
        }
        
        template<DdType LibraryType>
        Bdd<LibraryType> Bdd<LibraryType>::restrict(Bdd<LibraryType> const& constraint) const {
            this->addMetaVariables(constraint.getContainedMetaVariables());
            return internalBdd.constraint(constraint.internalBdd);
        }
        
        template<DdType LibraryType>
        Bdd<LibraryType> Bdd<LibraryType>::swapVariables(std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& metaVariablePairs) const {
            std::set<storm::expressions::Variable> newContainedMetaVariables;
            std::vector<std::pair<std::reference_wrapper<DdMetaVariable<LibraryType> const>, std::reference_wrapper<DdMetaVariable<LibraryType> const>>> fromTo;
            for (auto const& metaVariablePair : metaVariablePairs) {
                std::reference_wrapper<DdMetaVariable<LibraryType> const> variable1 = this->getDdManager()->getMetaVariable(metaVariablePair.first);
                std::reference_wrapper<DdMetaVariable<LibraryType> const> variable2 = this->getDdManager()->getMetaVariable(metaVariablePair.second);
                fromTo.push_back(std::make_pair(variable1, variable2));
            }
            return Bdd<LibraryType>(internalBdd.swapVariables(fromTo));
        }
        
        template<DdType LibraryType>
        template<typename ValueType>
        Add<LibraryType, ValueType> Bdd<LibraryType>::toAdd() const {
            return Add<LibraryType, ValueType>(internalBdd.template toAdd<ValueType>());
        }
        
        template<DdType LibraryType>
        storm::storage::BitVector Bdd<LibraryType>::toVector(storm::dd::Odd<LibraryType> const& rowOdd) const {
            return internalBdd.toVector(rowOdd);
        }
        
        template<DdType LibraryType>
        Bdd<LibraryType> Bdd<LibraryType>::getSupport() const {
            return Bdd<LibraryType>(internalBdd.getSupport());
        }
        
        template<DdType LibraryType>
        uint_fast64_t Bdd<LibraryType>::getNonZeroCount() const {
            std::size_t numberOfDdVariables = 0;
            for (auto const& metaVariable : this->getContainedMetaVariables()) {
                numberOfDdVariables += this->getDdManager()->getMetaVariable(metaVariable).getNumberOfDdVariables();
            }
            return internalBdd.getNonZeroCount(numberOfDdVariables);
        }
        
        template<DdType LibraryType>
        uint_fast64_t Bdd<LibraryType>::getLeafCount() const {
            return internalBdd.getLeafCount();
        }
        
        template<DdType LibraryType>
        uint_fast64_t Bdd<LibraryType>::getNodeCount() const {
            return internalBdd.getNodeCount();
        }
        
        template<DdType LibraryType>
        uint_fast64_t Bdd<LibraryType>::getIndex() const {
            return internalBdd.getIndex();
        }
        
        template<DdType LibraryType>
        bool Bdd<LibraryType>::isOne() const {
            return internalBdd.isOne();
        }
        
        template<DdType LibraryType>
        bool Bdd<LibraryType>::isZero() const {
            return internalBdd.isZero();
        }
        
        template<DdType LibraryType>
        void Bdd<LibraryType>::exportToDot(std::string const& filename) const {
            internalBdd.exportToDot(filename, this->getDdManager()->getDdVariableNames());
        }
        
        template class Bdd<storm::dd::DdType::CUDD>;
    }
}