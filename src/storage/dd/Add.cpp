#include "src/storage/dd/Add.h"

#include <boost/algorithm/string/join.hpp>

#include "src/storage/dd/DdMetaVariable.h"
#include "src/storage/dd/DdManager.h"

#include "src/storage/SparseMatrix.h"

#include "src/utility/constants.h"
#include "src/utility/macros.h"
#include "src/exceptions/InvalidArgumentException.h"

namespace storm {
    namespace dd {
        template<DdType LibraryType, typename ValueType>
        Add<LibraryType, ValueType>::Add(std::shared_ptr<DdManager<LibraryType> const> ddManager, InternalAdd<LibraryType, ValueType> const& internalAdd, std::set<storm::expressions::Variable> const& containedMetaVariables) : Dd<LibraryType>(ddManager, containedMetaVariables), internalAdd(internalAdd) {
            // Intentionally left empty.
        }
        
        template<DdType LibraryType, typename ValueType>
        bool Add<LibraryType, ValueType>::operator==(Add<LibraryType, ValueType> const& other) const {
            return internalAdd == other.internalAdd;
        }

        template<DdType LibraryType, typename ValueType>
        bool Add<LibraryType, ValueType>::operator!=(Add<LibraryType, ValueType> const& other) const {
            return internalAdd != other.internalAdd;
        }

        template<DdType LibraryType, typename ValueType>
        Add<LibraryType, ValueType> Add<LibraryType, ValueType>::ite(Add<LibraryType, ValueType> const& thenAdd, Add<LibraryType, ValueType> const& elseAdd) const {
            std::set<storm::expressions::Variable> metaVariables = Dd<LibraryType>::joinMetaVariables(thenAdd, elseAdd);
            metaVariables.insert(this->getContainedMetaVariables().begin(), this->getContainedMetaVariables().end());
            return Add<LibraryType, ValueType>(this->getDdManager(), internalAdd.ite(thenAdd.internalAdd, elseAdd.internalAdd), metaVariables);
        }
        
        template<DdType LibraryType, typename ValueType>
        Add<LibraryType, ValueType> Add<LibraryType, ValueType>::operator!() const {
            return Add<LibraryType, ValueType>(this->getDdManager(), !internalAdd, this->getContainedMetaVariables());
        }
        
        template<DdType LibraryType, typename ValueType>
        Add<LibraryType, ValueType> Add<LibraryType, ValueType>::operator||(Add<LibraryType, ValueType> const& other) const {
            return Add<LibraryType, ValueType>(this->getDdManager(), internalAdd || other.internalAdd, Dd<LibraryType>::joinMetaVariables(*this, other));
        }
        
        template<DdType LibraryType, typename ValueType>
        Add<LibraryType, ValueType>& Add<LibraryType, ValueType>::operator|=(Add<LibraryType, ValueType> const& other) {
            this->addMetaVariables(other.getContainedMetaVariables());
            internalAdd |= other.internalAdd;
            return *this;
        }

        template<DdType LibraryType, typename ValueType>
        Add<LibraryType, ValueType> Add<LibraryType, ValueType>::operator+(Add<LibraryType, ValueType> const& other) const {
            return Add<LibraryType, ValueType>(this->getDdManager(), internalAdd + other.internalAdd, Dd<LibraryType>::joinMetaVariables(*this, other));
        }

        template<DdType LibraryType, typename ValueType>
        Add<LibraryType, ValueType>& Add<LibraryType, ValueType>::operator+=(Add<LibraryType, ValueType> const& other) {
            this->addMetaVariables(other.getContainedMetaVariables());
            internalAdd += other.internalAdd;
            return *this;
        }

        template<DdType LibraryType, typename ValueType>
        Add<LibraryType, ValueType> Add<LibraryType, ValueType>::operator*(Add<LibraryType, ValueType> const& other) const {
            return Add<LibraryType, ValueType>(this->getDdManager(), internalAdd * other.internalAdd, Dd<LibraryType>::joinMetaVariables(*this, other));
        }
        
        template<DdType LibraryType, typename ValueType>
        Add<LibraryType, ValueType>& Add<LibraryType, ValueType>::operator*=(Add<LibraryType, ValueType> const& other) {
            this->addMetaVariables(other.getContainedMetaVariables());
            internalAdd *= other.internalAdd;
            return *this;
        }
        
        template<DdType LibraryType, typename ValueType>
        Add<LibraryType, ValueType> Add<LibraryType, ValueType>::operator-(Add<LibraryType, ValueType> const& other) const {
            return Add<LibraryType, ValueType>(this->getDdManager(), internalAdd - other.internalAdd, Dd<LibraryType>::joinMetaVariables(*this, other));
        }

        template<DdType LibraryType, typename ValueType>
        Add<LibraryType, ValueType> Add<LibraryType, ValueType>::operator-() const {
            return this->getDdManager()->getAddZero() - *this;
        }

        template<DdType LibraryType, typename ValueType>
        Add<LibraryType, ValueType>& Add<LibraryType, ValueType>::operator-=(Add<LibraryType, ValueType> const& other) {
            this->addMetaVariables(other.getContainedMetaVariables());
            internalAdd -= other.internalAdd;
            return *this;
        }
        
        template<DdType LibraryType, typename ValueType>
        Add<LibraryType, ValueType> Add<LibraryType, ValueType>::operator/(Add<LibraryType, ValueType> const& other) const {
            return Add<LibraryType, ValueType>(this->getDdManager(), internalAdd / other.internalAdd, Dd<LibraryType>::joinMetaVariables(*this, other));
        }
        
        template<DdType LibraryType, typename ValueType>
        Add<LibraryType, ValueType>& Add<LibraryType, ValueType>::operator/=(Add<LibraryType, ValueType> const& other) {
            this->addMetaVariables(other.getContainedMetaVariables());
            internalAdd /= other.internalAdd;
            return *this;
        }

        template<DdType LibraryType, typename ValueType>
        Add<LibraryType, ValueType> Add<LibraryType, ValueType>::equals(Add<LibraryType, ValueType> const& other) const {
            return Add<LibraryType, ValueType>(this->getDdManager(), internalAdd.equals(other), Dd<LibraryType>::joinMetaVariables(*this, other));
        }
        
        template<DdType LibraryType, typename ValueType>
        Add<LibraryType, ValueType> Add<LibraryType, ValueType>::notEquals(Add<LibraryType, ValueType> const& other) const {
            return Add<LibraryType, ValueType>(this->getDdManager(), internalAdd.notEquals(other), Dd<LibraryType>::joinMetaVariables(*this, other));

        }
        
        template<DdType LibraryType, typename ValueType>
        Add<LibraryType, ValueType> Add<LibraryType, ValueType>::less(Add<LibraryType, ValueType> const& other) const {
            return Add<LibraryType, ValueType>(this->getDdManager(), internalAdd.less(other), Dd<LibraryType>::joinMetaVariables(*this, other));
        }
        
        template<DdType LibraryType, typename ValueType>
        Add<LibraryType, ValueType> Add<LibraryType, ValueType>::lessOrEqual(Add<LibraryType, ValueType> const& other) const {
            return Add<LibraryType, ValueType>(this->getDdManager(), internalAdd.lessOrEqual(other), Dd<LibraryType>::joinMetaVariables(*this, other));

        }
        
        template<DdType LibraryType, typename ValueType>
        Add<LibraryType, ValueType> Add<LibraryType, ValueType>::greater(Add<LibraryType, ValueType> const& other) const {
            return Add<LibraryType, ValueType>(this->getDdManager(), internalAdd.greater(other), Dd<LibraryType>::joinMetaVariables(*this, other));
        }

        template<DdType LibraryType, typename ValueType>
        Add<LibraryType, ValueType> Add<LibraryType, ValueType>::greaterOrEqual(Add<LibraryType, ValueType> const& other) const {
            return Add<LibraryType, ValueType>(this->getDdManager(), internalAdd.greaterOrEqual(other), Dd<LibraryType>::joinMetaVariables(*this, other));

        }

        template<DdType LibraryType, typename ValueType>
        Add<LibraryType, ValueType> Add<LibraryType, ValueType>::pow(Add<LibraryType, ValueType> const& other) const {
            return Add<LibraryType, ValueType>(this->getDdManager(), internalAdd.pow(other), Dd<LibraryType>::joinMetaVariables(*this, other));
        }
        
        template<DdType LibraryType, typename ValueType>
        Add<LibraryType, ValueType> Add<LibraryType, ValueType>::mod(Add<LibraryType, ValueType> const& other) const {
            return Add<LibraryType, ValueType>(this->getDdManager(), internalAdd.mod(other), Dd<LibraryType>::joinMetaVariables(*this, other));

        }
        
        template<DdType LibraryType, typename ValueType>
        Add<LibraryType, ValueType> Add<LibraryType, ValueType>::logxy(Add<LibraryType, ValueType> const& other) const {
            return Add<LibraryType, ValueType>(this->getDdManager(), internalAdd.logxy(other), Dd<LibraryType>::joinMetaVariables(*this, other));

        }
        
        template<DdType LibraryType, typename ValueType>
        Add<LibraryType, ValueType> Add<LibraryType, ValueType>::floor() const {
            return Add<LibraryType, ValueType>(this->getDdManager(), internalAdd.floor(), this->getContainedMetaVariables());
        }
        
        template<DdType LibraryType, typename ValueType>
        Add<LibraryType, ValueType> Add<LibraryType, ValueType>::ceil() const {
            return Add<LibraryType, ValueType>(this->getDdManager(), internalAdd.ceil(), this->getContainedMetaVariables());
        }
        
        template<DdType LibraryType, typename ValueType>
        Add<LibraryType, ValueType> Add<LibraryType, ValueType>::minimum(Add<LibraryType, ValueType> const& other) const {
            return Add<LibraryType, ValueType>(this->getDdManager(), internalAdd.minimum(other), Dd<LibraryType>::joinMetaVariables(*this, other));
        }

        template<DdType LibraryType, typename ValueType>
        Add<LibraryType, ValueType> Add<LibraryType, ValueType>::maximum(Add<LibraryType, ValueType> const& other) const {
            return Add<LibraryType, ValueType>(this->getDdManager(), internalAdd.maximum(other), Dd<LibraryType>::joinMetaVariables(*this, other));
        }

        template<DdType LibraryType, typename ValueType>
        Add<LibraryType, ValueType> Add<LibraryType, ValueType>::sumAbstract(std::set<storm::expressions::Variable> const& metaVariables) const {
            Bdd<LibraryType> cube = this->getCube(metaVariables);
            return Add<LibraryType, ValueType>(this->getDdManager(), internalAdd.sumAbstract(cube), Dd<LibraryType>::subtractMetaVariables(*this, cube));
        }

        template<DdType LibraryType, typename ValueType>
        Add<LibraryType, ValueType> Add<LibraryType, ValueType>::minAbstract(std::set<storm::expressions::Variable> const& metaVariables) const {
            Bdd<LibraryType> cube = this->getCube(metaVariables);
            return Add<LibraryType, ValueType>(this->getDdManager(), internalAdd.minAbstract(cube), Dd<LibraryType>::subtractMetaVariables(*this, cube));
        }
        
        template<DdType LibraryType, typename ValueType>
        Add<LibraryType, ValueType> Add<LibraryType, ValueType>::maxAbstract(std::set<storm::expressions::Variable> const& metaVariables) const {
            Bdd<LibraryType> cube = this->getCube(metaVariables);
            return Add<LibraryType, ValueType>(this->getDdManager(), internalAdd.maxAbstract(cube), Dd<LibraryType>::subtractMetaVariables(*this, cube));
        }

        template<DdType LibraryType, typename ValueType>
        bool Add<LibraryType, ValueType>::equalModuloPrecision(Add<LibraryType, ValueType> const& other, double precision, bool relative) const {
            return internalAdd.equalModuloPrecision(other, precision, relative);
        }
        
        template<DdType LibraryType, typename ValueType>
        Add<LibraryType, ValueType> Add<LibraryType, ValueType>::swapVariables(std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& metaVariablePairs) const {
            std::set<storm::expressions::Variable> newContainedMetaVariables;
            std::vector<InternalAdd<LibraryType, ValueType>> from;
            std::vector<InternalAdd<LibraryType, ValueType>> to;
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
                    from.push_back(ddVariable.toAdd());
                }
                for (auto const& ddVariable : variable2.getDdVariables()) {
                    to.push_back(ddVariable.toAdd());
                }
            }
            return Bdd<LibraryType>(this->getDdManager(), internalAdd.swapVariables(from, to), newContainedMetaVariables);
        }
        
        template<DdType LibraryType, typename ValueType>
        Add<LibraryType, ValueType> Add<LibraryType, ValueType>::multiplyMatrix(Add<LibraryType, ValueType> const& otherMatrix, std::set<storm::expressions::Variable> const& summationMetaVariables) const {
            // Create the CUDD summation variables.
            std::vector<InternalAdd<LibraryType, ValueType>> summationDdVariables;
            for (auto const& metaVariable : summationMetaVariables) {
                for (auto const& ddVariable : this->getDdManager()->getMetaVariable(metaVariable).getDdVariables()) {
                    summationDdVariables.push_back(ddVariable.toAdd());
                }
            }
            
            std::set<storm::expressions::Variable> unionOfMetaVariables = Dd<LibraryType>::joinMetaVariables(*this, otherMatrix);
            std::set<storm::expressions::Variable> containedMetaVariables;
            std::set_difference(unionOfMetaVariables.begin(), unionOfMetaVariables.end(), summationMetaVariables.begin(), summationMetaVariables.end(), std::inserter(containedMetaVariables, containedMetaVariables.begin()));
            
            return Add<LibraryType, ValueType>(this->getDdManager(), internalAdd.multiplyMatrix(otherMatrix, summationDdVariables), containedMetaVariables);
        }

        template<DdType LibraryType, typename ValueType>
        Bdd<LibraryType> Add<LibraryType, ValueType>::greater(ValueType const& value) const {
            return Bdd<LibraryType>(this->getDdManager(), internalAdd.greater(value), this->getContainedMetaVariables());
        }
        
        template<DdType LibraryType, typename ValueType>
        Bdd<LibraryType> Add<LibraryType, ValueType>::greaterOrEqual(ValueType const& value) const {
            return Bdd<LibraryType>(this->getDdManager(), internalAdd.greaterOrEqual(value), this->getContainedMetaVariables());
        }
        
        template<DdType LibraryType, typename ValueType>
        Bdd<LibraryType> Add<LibraryType, ValueType>::less(ValueType const& value) const {
            return Bdd<LibraryType>(this->getDdManager(), internalAdd.less(value), this->getContainedMetaVariables());
        }
        
        template<DdType LibraryType, typename ValueType>
        Bdd<LibraryType> Add<LibraryType, ValueType>::lessOrEqual(ValueType const& value) const {
            return Bdd<LibraryType>(this->getDdManager(), internalAdd.lessOrEqual(value), this->getContainedMetaVariables());
        }
        
        template<DdType LibraryType, typename ValueType>
        Bdd<LibraryType> Add<LibraryType, ValueType>::notZero() const {
            return this->toBdd();
        }
        
        template<DdType LibraryType, typename ValueType>
        Add<LibraryType, ValueType> Add<LibraryType, ValueType>::constrain(Add<LibraryType, ValueType> const& constraint) const {
            return Add<LibraryType, ValueType>(this->getDdManager(), internalAdd.constrain(constraint), Dd<LibraryType>::joinMetaVariables(*this, constraint));
        }
        
        template<DdType LibraryType, typename ValueType>
        Add<LibraryType, ValueType> Add<LibraryType, ValueType>::restrict(Add<LibraryType, ValueType> const& constraint) const {
            return Add<LibraryType, ValueType>(this->getDdManager(), internalAdd.restrict(constraint), Dd<LibraryType>::joinMetaVariables(*this, constraint));
        }

        template<DdType LibraryType, typename ValueType>
        Bdd<LibraryType> Add<LibraryType, ValueType>::getSupport() const {
            return Bdd<LibraryType>(this->getDdManager(), internalAdd.getSupport(), this->getContainedMetaVariables());
        }
        
        template<DdType LibraryType, typename ValueType>
        uint_fast64_t Add<LibraryType, ValueType>::getNonZeroCount() const {
            std::size_t numberOfDdVariables = 0;
            for (auto const& metaVariable : this->getContainedMetaVariables()) {
                numberOfDdVariables += this->getDdManager()->getMetaVariable(metaVariable).getNumberOfDdVariables();
            }
            return internalAdd.getNonZeroCount(numberOfDdVariables);
        }
        
        template<DdType LibraryType, typename ValueType>
        uint_fast64_t Add<LibraryType, ValueType>::getLeafCount() const {
            return internalAdd.getLeafCount();
        }
        
        template<DdType LibraryType, typename ValueType>
        uint_fast64_t Add<LibraryType, ValueType>::getNodeCount() const {
            return internalAdd.getNodeCount();
        }
        
        template<DdType LibraryType, typename ValueType>
        ValueType Add<LibraryType, ValueType>::getMin() const {
            return internalAdd.getMin();
        }
        
        template<DdType LibraryType, typename ValueType>
        ValueType Add<LibraryType, ValueType>::getMax() const {
            return internalAdd.getMax();
        }

        template<DdType LibraryType, typename ValueType>
        void Add<LibraryType, ValueType>::setValue(storm::expressions::Variable const& metaVariable, int_fast64_t variableValue, ValueType const& targetValue) {
            std::map<storm::expressions::Variable, int_fast64_t> metaVariableToValueMap;
            metaVariableToValueMap.emplace(metaVariable, variableValue);
            this->setValue(metaVariableToValueMap, targetValue);
        }
        
        template<DdType LibraryType, typename ValueType>
        void Add<LibraryType, ValueType>::setValue(storm::expressions::Variable const& metaVariable1, int_fast64_t variableValue1, storm::expressions::Variable const& metaVariable2, int_fast64_t variableValue2, ValueType const& targetValue) {
            std::map<storm::expressions::Variable, int_fast64_t> metaVariableToValueMap;
            metaVariableToValueMap.emplace(metaVariable1, variableValue1);
            metaVariableToValueMap.emplace(metaVariable2, variableValue2);
            this->setValue(metaVariableToValueMap, targetValue);
        }

        template<DdType LibraryType, typename ValueType>
        void Add<LibraryType, ValueType>::setValue(std::map<storm::expressions::Variable, int_fast64_t> const& metaVariableToValueMap, ValueType const& targetValue) {
            Bdd<LibraryType> valueEncoding = this->getDdManager()->getBddOne();
            for (auto const& nameValuePair : metaVariableToValueMap) {
                valueEncoding &= this->getDdManager()->getEncoding(nameValuePair.first, nameValuePair.second);
                // Also record that the DD now contains the meta variable.
                this->addMetaVariable(nameValuePair.first);
            }
            
            internalAdd = valueEncoding.toAdd().ite(this->getDdManager()->getConstant(targetValue), internalAdd);
        }

        template<DdType LibraryType, typename ValueType>
        ValueType Add<LibraryType, ValueType>::getValue(std::map<storm::expressions::Variable, int_fast64_t> const& metaVariableToValueMap) const {
            std::set<storm::expressions::Variable> remainingMetaVariables(this->getContainedMetaVariables());
            Bdd<LibraryType> valueEncoding = this->getDdManager()->getBddOne();
            for (auto const& nameValuePair : metaVariableToValueMap) {
                valueEncoding &= this->getDdManager()->getEncoding(nameValuePair.first, nameValuePair.second);
                if (this->containsMetaVariable(nameValuePair.first)) {
                    remainingMetaVariables.erase(nameValuePair.first);
                }
            }
            
            STORM_LOG_THROW(remainingMetaVariables.empty(), storm::exceptions::InvalidArgumentException, "Cannot evaluate function for which not all inputs were given.");
            
            Add<LibraryType, ValueType> value = *this * valueEncoding.toAdd();
            value = value.sumAbstract(this->getContainedMetaVariables());
            return value.getMax();
        }
        
        template<DdType LibraryType, typename ValueType>
        bool Add<LibraryType, ValueType>::isOne() const {
            return internalAdd.isOne();
        }
        
        template<DdType LibraryType, typename ValueType>
        bool Add<LibraryType, ValueType>::isZero() const {
            return internalAdd.isZero();
        }
        
        template<DdType LibraryType, typename ValueType>
        bool Add<LibraryType, ValueType>::isConstant() const {
            return internalAdd.isConstant();
        }
        
        template<DdType LibraryType, typename ValueType>
        uint_fast64_t Add<LibraryType, ValueType>::getIndex() const {
            return internalAdd.getIndex();
        }
        
        template<DdType LibraryType, typename ValueType>
        std::vector<ValueType> Add<LibraryType, ValueType>::toVector() const {
            return this->toVector(Odd<LibraryType>(*this));
        }
        
        template<DdType LibraryType, typename ValueType>
        std::vector<ValueType> Add<LibraryType, ValueType>::toVector(Odd<LibraryType> const& rowOdd) const {
            std::vector<ValueType> result(rowOdd.getTotalOffset());
            std::vector<uint_fast64_t> ddVariableIndices = this->getDdManager().getSortedVariableIndices();
            addToVector(rowOdd, ddVariableIndices, result);
            return result;
        }

        template<DdType LibraryType, typename ValueType>
        std::vector<ValueType> Add<LibraryType, ValueType>::toVector(std::set<storm::expressions::Variable> const& groupMetaVariables, storm::dd::Odd<LibraryType> const& rowOdd, std::vector<uint_fast64_t> const& groupOffsets) const {
            std::set<storm::expressions::Variable> rowMetaVariables;
            
            // Prepare the proper sets of meta variables.
            for (auto const& variable : this->getContainedMetaVariables()) {
                if (groupMetaVariables.find(variable) != groupMetaVariables.end()) {
                    continue;
                }
                
                rowMetaVariables.insert(variable);
            }
            std::vector<uint_fast64_t> ddGroupVariableIndices;
            for (auto const& variable : groupMetaVariables) {
                DdMetaVariable<LibraryType> const& metaVariable = this->getDdManager()->getMetaVariable(variable);
                for (auto const& ddVariable : metaVariable.getDdVariables()) {
                    ddGroupVariableIndices.push_back(ddVariable.getIndex());
                }
            }
            std::vector<uint_fast64_t> ddRowVariableIndices;
            for (auto const& variable : rowMetaVariables) {
                DdMetaVariable<LibraryType> const& metaVariable = this->getDdManager()->getMetaVariable(variable);
                for (auto const& ddVariable : metaVariable.getDdVariables()) {
                    ddRowVariableIndices.push_back(ddVariable.getIndex());
                }
            }
            
            return internalAdd.toVector(ddGroupVariableIndices, rowOdd, ddRowVariableIndices, groupOffsets);
        }
        
        template<DdType LibraryType, typename ValueType>
        storm::storage::SparseMatrix<ValueType> Add<LibraryType, ValueType>::toMatrix() const {
            std::set<storm::expressions::Variable> rowVariables;
            std::set<storm::expressions::Variable> columnVariables;
            
            for (auto const& variable : this->getContainedMetaVariables()) {
                if (variable.getName().size() > 0 && variable.getName().back() == '\'') {
                    columnVariables.insert(variable);
                } else {
                    rowVariables.insert(variable);
                }
            }
            
            return toMatrix(rowVariables, columnVariables, Odd<LibraryType>(this->sumAbstract(rowVariables)), Odd<LibraryType>(this->sumAbstract(columnVariables)));
        }
        
        template<DdType LibraryType, typename ValueType>
        storm::storage::SparseMatrix<ValueType> Add<LibraryType, ValueType>::toMatrix(storm::dd::Odd<LibraryType> const& rowOdd, storm::dd::Odd<LibraryType> const& columnOdd) const {
            std::set<storm::expressions::Variable> rowMetaVariables;
            std::set<storm::expressions::Variable> columnMetaVariables;
            
            for (auto const& variable : this->getContainedMetaVariables()) {
                if (variable.getName().size() > 0 && variable.getName().back() == '\'') {
                    columnMetaVariables.insert(variable);
                } else {
                    rowMetaVariables.insert(variable);
                }
            }
            
            return toMatrix(rowMetaVariables, columnMetaVariables, rowOdd, columnOdd);
        }
        
        template<DdType LibraryType, typename ValueType>
        storm::storage::SparseMatrix<ValueType> Add<LibraryType, ValueType>::toMatrix(std::set<storm::expressions::Variable> const& rowMetaVariables, std::set<storm::expressions::Variable> const& columnMetaVariables, storm::dd::Odd<LibraryType> const& rowOdd, storm::dd::Odd<LibraryType> const& columnOdd) const {
            std::vector<uint_fast64_t> ddRowVariableIndices;
            std::vector<uint_fast64_t> ddColumnVariableIndices;
            
            for (auto const& variable : rowMetaVariables) {
                DdMetaVariable<LibraryType> const& metaVariable = this->getDdManager()->getMetaVariable(variable);
                for (auto const& ddVariable : metaVariable.getDdVariables()) {
                    ddRowVariableIndices.push_back(ddVariable.getIndex());
                }
            }
            std::sort(ddRowVariableIndices.begin(), ddRowVariableIndices.end());
            
            for (auto const& variable : columnMetaVariables) {
                DdMetaVariable<LibraryType> const& metaVariable = this->getDdManager()->getMetaVariable(variable);
                for (auto const& ddVariable : metaVariable.getDdVariables()) {
                    ddColumnVariableIndices.push_back(ddVariable.getIndex());
                }
            }
            std::sort(ddColumnVariableIndices.begin(), ddColumnVariableIndices.end());
            
            return internalAdd.toMatrix(rowOdd, ddRowVariableIndices, columnOdd, ddColumnVariableIndices);
        }
        
        template<DdType LibraryType, typename ValueType>
        storm::storage::SparseMatrix<ValueType> Add<LibraryType, ValueType>::toMatrix(std::set<storm::expressions::Variable> const& groupMetaVariables, storm::dd::Odd<LibraryType> const& rowOdd, storm::dd::Odd<LibraryType> const& columnOdd) const {
            std::set<storm::expressions::Variable> rowMetaVariables;
            std::set<storm::expressions::Variable> columnMetaVariables;
            
            for (auto const& variable : this->getContainedMetaVariables()) {
                // If the meta variable is a group meta variable, we do not insert it into the set of row/column meta variables.
                if (groupMetaVariables.find(variable) != groupMetaVariables.end()) {
                    continue;
                }
                
                if (variable.getName().size() > 0 && variable.getName().back() == '\'') {
                    columnMetaVariables.insert(variable);
                } else {
                    rowMetaVariables.insert(variable);
                }
            }
            
            // Create the canonical row group sizes and build the matrix.
            return toMatrix(rowMetaVariables, columnMetaVariables, groupMetaVariables, rowOdd, columnOdd);
        }
        
        template<DdType LibraryType, typename ValueType>
        storm::storage::SparseMatrix<ValueType> Add<LibraryType, ValueType>::toMatrix(std::set<storm::expressions::Variable> const& rowMetaVariables, std::set<storm::expressions::Variable> const& columnMetaVariables, std::set<storm::expressions::Variable> const& groupMetaVariables, storm::dd::Odd<LibraryType> const& rowOdd, storm::dd::Odd<LibraryType> const& columnOdd) const {
            std::vector<uint_fast64_t> ddRowVariableIndices;
            std::vector<uint_fast64_t> ddColumnVariableIndices;
            std::vector<uint_fast64_t> ddGroupVariableIndices;
            std::set<storm::expressions::Variable> rowAndColumnMetaVariables;
            
            for (auto const& variable : rowMetaVariables) {
                DdMetaVariable<LibraryType> const& metaVariable = this->getDdManager()->getMetaVariable(variable);
                for (auto const& ddVariable : metaVariable.getDdVariables()) {
                    ddRowVariableIndices.push_back(ddVariable.getIndex());
                }
                rowAndColumnMetaVariables.insert(variable);
            }
            std::sort(ddRowVariableIndices.begin(), ddRowVariableIndices.end());
            for (auto const& variable : columnMetaVariables) {
                DdMetaVariable<LibraryType> const& metaVariable = this->getDdManager()->getMetaVariable(variable);
                for (auto const& ddVariable : metaVariable.getDdVariables()) {
                    ddColumnVariableIndices.push_back(ddVariable.getIndex());
                }
                rowAndColumnMetaVariables.insert(variable);
            }
            std::sort(ddColumnVariableIndices.begin(), ddColumnVariableIndices.end());
            for (auto const& variable : groupMetaVariables) {
                DdMetaVariable<LibraryType> const& metaVariable = this->getDdManager()->getMetaVariable(variable);
                for (auto const& ddVariable : metaVariable.getDdVariables()) {
                    ddGroupVariableIndices.push_back(ddVariable.getIndex());
                }
            }
            std::sort(ddGroupVariableIndices.begin(), ddGroupVariableIndices.end());
            
            return internalAdd.toMatrix(ddGroupVariableIndices, rowOdd, ddRowVariableIndices, columnOdd, ddColumnVariableIndices);
        }
        
        template<DdType LibraryType, typename ValueType>
        std::pair<storm::storage::SparseMatrix<ValueType>, std::vector<ValueType>> Add<LibraryType, ValueType>::toMatrixVector(storm::dd::Add<LibraryType, ValueType> const& vector, std::vector<uint_fast64_t>&& rowGroupSizes, std::set<storm::expressions::Variable> const& groupMetaVariables, storm::dd::Odd<LibraryType> const& rowOdd, storm::dd::Odd<LibraryType> const& columnOdd) const {
            std::set<storm::expressions::Variable> rowMetaVariables;
            std::set<storm::expressions::Variable> columnMetaVariables;
            
            for (auto const& variable : this->getContainedMetaVariables()) {
                // If the meta variable is a group meta variable, we do not insert it into the set of row/column meta variables.
                if (groupMetaVariables.find(variable) != groupMetaVariables.end()) {
                    continue;
                }
                
                if (variable.getName().size() > 0 && variable.getName().back() == '\'') {
                    columnMetaVariables.insert(variable);
                } else {
                    rowMetaVariables.insert(variable);
                }
            }
            
            // Create the canonical row group sizes and build the matrix.
            return toMatrixVector(vector, std::move(rowGroupSizes), rowMetaVariables, columnMetaVariables, groupMetaVariables, rowOdd, columnOdd);
        }

        template<DdType LibraryType, typename ValueType>
        std::pair<storm::storage::SparseMatrix<ValueType>, std::vector<ValueType>> Add<LibraryType, ValueType>::toMatrixVector(storm::dd::Add<LibraryType, ValueType> const& vector, std::vector<uint_fast64_t>&& rowGroupIndices, std::set<storm::expressions::Variable> const& rowMetaVariables, std::set<storm::expressions::Variable> const& columnMetaVariables, std::set<storm::expressions::Variable> const& groupMetaVariables, storm::dd::Odd<LibraryType> const& rowOdd, storm::dd::Odd<LibraryType> const& columnOdd) const {
            std::vector<uint_fast64_t> ddRowVariableIndices;
            std::vector<uint_fast64_t> ddColumnVariableIndices;
            std::vector<uint_fast64_t> ddGroupVariableIndices;
            std::set<storm::expressions::Variable> rowAndColumnMetaVariables;
            
            for (auto const& variable : rowMetaVariables) {
                DdMetaVariable<LibraryType> const& metaVariable = this->getDdManager()->getMetaVariable(variable);
                for (auto const& ddVariable : metaVariable.getDdVariables()) {
                    ddRowVariableIndices.push_back(ddVariable.getIndex());
                }
                rowAndColumnMetaVariables.insert(variable);
            }
            std::sort(ddRowVariableIndices.begin(), ddRowVariableIndices.end());
            for (auto const& variable : columnMetaVariables) {
                DdMetaVariable<LibraryType> const& metaVariable = this->getDdManager()->getMetaVariable(variable);
                for (auto const& ddVariable : metaVariable.getDdVariables()) {
                    ddColumnVariableIndices.push_back(ddVariable.getIndex());
                }
                rowAndColumnMetaVariables.insert(variable);
            }
            std::sort(ddColumnVariableIndices.begin(), ddColumnVariableIndices.end());
            for (auto const& variable : groupMetaVariables) {
                DdMetaVariable<LibraryType> const& metaVariable = this->getDdManager()->getMetaVariable(variable);
                for (auto const& ddVariable : metaVariable.getDdVariables()) {
                    ddGroupVariableIndices.push_back(ddVariable.getIndex());
                }
            }
            std::sort(ddGroupVariableIndices.begin(), ddGroupVariableIndices.end());
            
            return internalAdd.toMatrixVector(vector.internalAdd, ddGroupVariableIndices, std::move(rowGroupIndices), rowOdd, ddRowVariableIndices, columnOdd, ddColumnVariableIndices);
        }

        template<DdType LibraryType, typename ValueType>
        void Add<LibraryType, ValueType>::exportToDot(std::string const& filename) const {
            internalAdd.exportToDot(filename, this->getDdManager()->getDdVariableNames());
        }
        
        template<DdType LibraryType, typename ValueType>
        AddIterator<LibraryType, ValueType> Add<LibraryType, ValueType>::begin(bool enumerateDontCareMetaVariables) const {
            internalAdd.begin(this->getContainedMetaVariables(), enumerateDontCareMetaVariables);
        }
        
        template<DdType LibraryType, typename ValueType>
        AddIterator<LibraryType, ValueType> Add<LibraryType, ValueType>::end(bool enumerateDontCareMetaVariables) const {
            return internalAdd.end(enumerateDontCareMetaVariables);
        }
        
        template<DdType LibraryType, typename ValueType>
        std::ostream& operator<<(std::ostream& out, Add<LibraryType, ValueType> const& add) {
            out << "ADD with " << add.getNonZeroCount() << " nnz, " << add.getNodeCount() << " nodes, " << add.getLeafCount() << " leaves" << std::endl;
            std::vector<std::string> variableNames;
            for (auto const& variable : add.getContainedMetaVariables()) {
                variableNames.push_back(variable.getName());
            }
            out << "contained variables: " << boost::algorithm::join(variableNames, ", ") << std::endl;
            return out;
        }
        
        template<DdType LibraryType, typename ValueType>
        void Add<LibraryType, ValueType>::addToVector(Odd<LibraryType> const& odd, std::vector<uint_fast64_t> const& ddVariableIndices, std::vector<ValueType>& targetVector) const {
            std::function<ValueType (ValueType const&, ValueType const&)> fct = [] (ValueType const& a, ValueType const& b) -> ValueType { return a + b; };
            return internalAdd.composeVector(odd, ddVariableIndices, targetVector, fct);
        }

        template<DdType LibraryType, typename ValueType>
        Add<LibraryType, ValueType> Add<LibraryType, ValueType>::fromVector(std::shared_ptr<DdManager<LibraryType> const> ddManager, std::vector<ValueType> const& values, Odd<LibraryType> const& odd, std::set<storm::expressions::Variable> const& metaVariables) {
            return Add<LibraryType, ValueType>(ddManager, InternalAdd<LibraryType, ValueType>::fromVector(ddManager, values, odd, ddManager->getSortedVariableIndices(metaVariables)), metaVariables);
        }
        
        template<DdType LibraryType, typename ValueType>
        Bdd<LibraryType> Add<LibraryType, ValueType>::toBdd() const {
            return Bdd<DdType::CUDD>(this->getDdManager(), internalAdd.toBdd(), this->getContainedMetaVariables());
        }

        template class Add<storm::dd::DdType::CUDD, double>;
    }
}