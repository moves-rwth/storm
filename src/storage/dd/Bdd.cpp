#include "src/storage/dd/Bdd.h"
#include "src/storage/dd/Add.h"
#include "src/storage/dd/Odd.h"

#include "src/logic/ComparisonType.h"

#include "src/storage/dd/DdMetaVariable.h"
#include "src/storage/dd/DdManager.h"
#include "src/storage/dd/Odd.h"

#include "src/storage/BitVector.h"

#include "src/utility/macros.h"
#include "src/exceptions/InvalidArgumentException.h"

namespace storm {
    namespace dd {
        
        template<DdType LibraryType>
        Bdd<LibraryType>::Bdd(std::shared_ptr<DdManager<LibraryType> const> ddManager, InternalBdd<LibraryType> const& internalBdd, std::set<storm::expressions::Variable> const& containedMetaVariables) : Dd<LibraryType>(ddManager, containedMetaVariables), internalBdd(internalBdd) {
            // Intentionally left empty.
        }
        
        template<DdType LibraryType>
        Bdd<LibraryType> Bdd<LibraryType>::fromVector(std::shared_ptr<DdManager<LibraryType> const> ddManager, std::vector<double> const& explicitValues, storm::dd::Odd const& odd, std::set<storm::expressions::Variable> const& metaVariables, storm::logic::ComparisonType comparisonType, double value) {
            switch (comparisonType) {
                case storm::logic::ComparisonType::Less:
                    return fromVector<double>(ddManager, explicitValues, odd, metaVariables, std::bind(std::greater<double>(), value, std::placeholders::_1));
                case storm::logic::ComparisonType::LessEqual:
                    return fromVector<double>(ddManager, explicitValues, odd, metaVariables, std::bind(std::greater_equal<double>(), value, std::placeholders::_1));
                case storm::logic::ComparisonType::Greater:
                    return fromVector<double>(ddManager, explicitValues, odd, metaVariables, std::bind(std::less<double>(), value, std::placeholders::_1));
                case storm::logic::ComparisonType::GreaterEqual:
                    return fromVector<double>(ddManager, explicitValues, odd, metaVariables, std::bind(std::less_equal<double>(), value, std::placeholders::_1));
            }
        }
        
        template<DdType LibraryType>
        template<typename ValueType>
        Bdd<LibraryType> Bdd<LibraryType>::fromVector(std::shared_ptr<DdManager<LibraryType> const> ddManager, std::vector<ValueType> const& values, Odd const& odd, std::set<storm::expressions::Variable> const& metaVariables, std::function<bool (ValueType const&)> const& filter) {
            return Bdd<LibraryType>(ddManager, InternalBdd<LibraryType>::fromVector(&ddManager->internalDdManager, values, odd, ddManager->getSortedVariableIndices(metaVariables), filter), metaVariables);
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
            std::set<storm::expressions::Variable> metaVariables = Dd<LibraryType>::joinMetaVariables(thenBdd, elseBdd);
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
            Bdd<LibraryType> cube = getCube(*this->getDdManager(), metaVariables);
            return Bdd<LibraryType>(this->getDdManager(), internalBdd.existsAbstract(cube), Dd<LibraryType>::subtractMetaVariables(*this, cube));
        }

        template<DdType LibraryType>
        Bdd<LibraryType> Bdd<LibraryType>::universalAbstract(std::set<storm::expressions::Variable> const& metaVariables) const {
            Bdd<LibraryType> cube = getCube(*this->getDdManager(), metaVariables);
            return Bdd<LibraryType>(this->getDdManager(), internalBdd.universalAbstract(cube), Dd<LibraryType>::subtractMetaVariables(*this, cube));
        }
        
        template<DdType LibraryType>
        Bdd<LibraryType> Bdd<LibraryType>::andExists(Bdd<LibraryType> const& other, std::set<storm::expressions::Variable> const& existentialVariables) const {
            Bdd<LibraryType> cube = getCube(*this->getDdManager(), existentialVariables);

            std::set<storm::expressions::Variable> unionOfMetaVariables;
            std::set_union(this->getContainedMetaVariables().begin(), this->getContainedMetaVariables().end(), other.getContainedMetaVariables().begin(), other.getContainedMetaVariables().end(), std::inserter(unionOfMetaVariables, unionOfMetaVariables.begin()));
            std::set<storm::expressions::Variable> containedMetaVariables;
            std::set_difference(unionOfMetaVariables.begin(), unionOfMetaVariables.end(), existentialVariables.begin(), existentialVariables.end(), std::inserter(containedMetaVariables, containedMetaVariables.begin()));
            
            return Bdd<LibraryType>(this->getDdManager(), internalBdd.andExists(other, cube), containedMetaVariables);
        }
        
        template<DdType LibraryType>
        Bdd<LibraryType> Bdd<LibraryType>::constrain(Bdd<LibraryType> const& constraint) const {
            return Bdd<LibraryType>(this->getDdManager(), internalBdd.constrain(constraint), Dd<LibraryType>::joinMetaVariables(*this, constraint));
        }
        
        template<DdType LibraryType>
        Bdd<LibraryType> Bdd<LibraryType>::restrict(Bdd<LibraryType> const& constraint) const {
            return Bdd<LibraryType>(this->getDdManager(), internalBdd.restrict(constraint), Dd<LibraryType>::joinMetaVariables(*this, constraint));
        }
        
        template<DdType LibraryType>
        Bdd<LibraryType> Bdd<LibraryType>::relationalProduct(Bdd<LibraryType> const& relation, std::set<storm::expressions::Variable> const& rowMetaVariables) const {
            std::set<storm::expressions::Variable> tmpMetaVariables = Dd<LibraryType>::joinMetaVariables(*this, relation);
            std::set<storm::expressions::Variable> newMetaVariables;
            std::set_difference(tmpMetaVariables.begin(), tmpMetaVariables.end(), rowMetaVariables.begin(), rowMetaVariables.end(), std::inserter(newMetaVariables, newMetaVariables.begin()));
            
            std::vector<InternalBdd<LibraryType>> rowVariables;
            for (auto const& metaVariable : rowMetaVariables) {
                DdMetaVariable<LibraryType> const& variable = this->getDdManager()->getMetaVariable(metaVariable);
                for (auto const& ddVariable : variable.getDdVariables()) {
                    rowVariables.push_back(ddVariable);
                }
            }
            
            return Bdd<LibraryType>(this->getDdManager(), internalBdd.relationalProduct(relation, rowVariables), newMetaVariables);
        }
        
        template<DdType LibraryType>
        Bdd<LibraryType> Bdd<LibraryType>::swapVariables(std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& metaVariablePairs) const {
            std::set<storm::expressions::Variable> newContainedMetaVariables;
            std::vector<InternalBdd<LibraryType>> from;
            std::vector<InternalBdd<LibraryType>> to;
            for (auto const& metaVariablePair : metaVariablePairs) {
                DdMetaVariable<LibraryType> const& variable1 = this->getDdManager()->getMetaVariable(metaVariablePair.first);
                DdMetaVariable<LibraryType> const& variable2 = this->getDdManager()->getMetaVariable(metaVariablePair.second);
                
                // Keep track of the contained meta variables in the DD.
                if (this->containsMetaVariable(metaVariablePair.first)) {
                    newContainedMetaVariables.insert(metaVariablePair.second);
                }
                if (this->containsMetaVariable(metaVariablePair.second)) {
                    newContainedMetaVariables.insert(metaVariablePair.first);
                }
                
                for (auto const& ddVariable : variable1.getDdVariables()) {
                    from.push_back(ddVariable);
                }
                for (auto const& ddVariable : variable2.getDdVariables()) {
                    to.push_back(ddVariable);
                }
            }
            return Bdd<LibraryType>(this->getDdManager(), internalBdd.swapVariables(from, to), newContainedMetaVariables);
        }
        
        template<DdType LibraryType>
        template<typename ValueType>
        Add<LibraryType, ValueType> Bdd<LibraryType>::toAdd() const {
            return Add<LibraryType, ValueType>(this->getDdManager(), internalBdd.template toAdd<ValueType>(), this->getContainedMetaVariables());
        }
        
        template<DdType LibraryType>
        storm::storage::BitVector Bdd<LibraryType>::toVector(storm::dd::Odd const& rowOdd) const {
            return internalBdd.toVector(rowOdd, this->getSortedVariableIndices());
        }
        
        template<DdType LibraryType>
        Bdd<LibraryType> Bdd<LibraryType>::getSupport() const {
            return Bdd<LibraryType>(this->getDdManager(), internalBdd.getSupport(), this->getContainedMetaVariables());
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
        
        template<DdType LibraryType>
        Bdd<LibraryType> Bdd<LibraryType>::getCube(DdManager<LibraryType> const& manager, std::set<storm::expressions::Variable> const& metaVariables) {
            Bdd<LibraryType> cube = manager.getBddOne();
            for (auto const& metaVariable : metaVariables) {
                cube &= manager.getMetaVariable(metaVariable).getCube();
            }
            return cube;
        }
        
        template<DdType LibraryType>
        Odd Bdd<LibraryType>::createOdd() const {
            return internalBdd.createOdd(this->getSortedVariableIndices());
        }
        
        template<DdType LibraryType>
        template<typename ValueType>
        std::vector<ValueType> Bdd<LibraryType>::filterExplicitVector(Odd const& odd, std::vector<ValueType> const& values) const {
            std::vector<ValueType> result(this->getNonZeroCount());
            internalBdd.filterExplicitVector(odd, this->getSortedVariableIndices(), values, result);
            return result;
        }
        
        template<DdType LibraryType>
        Bdd<LibraryType>::operator InternalBdd<LibraryType>() const {
            return internalBdd;
        }
        
        template class Bdd<DdType::CUDD>;
        
        template Bdd<DdType::CUDD> Bdd<DdType::CUDD>::fromVector(std::shared_ptr<DdManager<DdType::CUDD> const> ddManager, std::vector<double> const& values, Odd const& odd, std::set<storm::expressions::Variable> const& metaVariables, std::function<bool (double const&)> const& filter);
        template Bdd<DdType::CUDD> Bdd<DdType::CUDD>::fromVector(std::shared_ptr<DdManager<DdType::CUDD> const> ddManager, std::vector<uint_fast64_t> const& values, Odd const& odd, std::set<storm::expressions::Variable> const& metaVariables, std::function<bool (uint_fast64_t const&)> const& filter);
        
        template Add<DdType::CUDD, double> Bdd<DdType::CUDD>::toAdd() const;
        template Add<DdType::CUDD, uint_fast64_t> Bdd<DdType::CUDD>::toAdd() const;
        
        template std::vector<double> Bdd<DdType::CUDD>::filterExplicitVector(Odd const& odd, std::vector<double> const& values) const;
        template std::vector<uint_fast64_t> Bdd<DdType::CUDD>::filterExplicitVector(Odd const& odd, std::vector<uint_fast64_t> const& values) const;
        
        
        template class Bdd<DdType::Sylvan>;

        template Bdd<DdType::Sylvan> Bdd<DdType::Sylvan>::fromVector(std::shared_ptr<DdManager<DdType::Sylvan> const> ddManager, std::vector<double> const& values, Odd const& odd, std::set<storm::expressions::Variable> const& metaVariables, std::function<bool (double const&)> const& filter);
        template Bdd<DdType::Sylvan> Bdd<DdType::Sylvan>::fromVector(std::shared_ptr<DdManager<DdType::Sylvan> const> ddManager, std::vector<uint_fast64_t> const& values, Odd const& odd, std::set<storm::expressions::Variable> const& metaVariables, std::function<bool (uint_fast64_t const&)> const& filter);
        
        template Add<DdType::Sylvan, double> Bdd<DdType::Sylvan>::toAdd() const;
        template Add<DdType::Sylvan, uint_fast64_t> Bdd<DdType::Sylvan>::toAdd() const;
        
        template std::vector<double> Bdd<DdType::Sylvan>::filterExplicitVector(Odd const& odd, std::vector<double> const& values) const;
        template std::vector<uint_fast64_t> Bdd<DdType::Sylvan>::filterExplicitVector(Odd const& odd, std::vector<uint_fast64_t> const& values) const;
    }
}