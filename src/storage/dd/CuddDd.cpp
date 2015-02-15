#include <cstring>
#include <algorithm>

#include "src/storage/dd/CuddDd.h"
#include "src/storage/dd/CuddOdd.h"
#include "src/storage/dd/CuddDdManager.h"
#include "src/utility/vector.h"
#include "src/utility/macros.h"

#include "src/exceptions/InvalidArgumentException.h"
#include "src/exceptions/NotImplementedException.h"

namespace storm {
    namespace dd {
        Dd<DdType::CUDD>::Dd(std::shared_ptr<DdManager<DdType::CUDD> const> ddManager, ADD cuddAdd, std::set<storm::expressions::Variable> const& containedMetaVariables) : ddManager(ddManager), cuddAdd(cuddAdd), containedMetaVariables(containedMetaVariables) {
            // Intentionally left empty.
        }
        
        bool Dd<DdType::CUDD>::operator==(Dd<DdType::CUDD> const& other) const {
            return this->cuddAdd == other.getCuddAdd();
        }
        
        bool Dd<DdType::CUDD>::operator!=(Dd<DdType::CUDD> const& other) const {
            return this->cuddAdd != other.getCuddAdd();
        }
        
        Dd<DdType::CUDD> Dd<DdType::CUDD>::ite(Dd<DdType::CUDD> const& thenDd, Dd<DdType::CUDD> const& elseDd) const {
            std::set<storm::expressions::Variable> metaVariableNames(this->getContainedMetaVariables());
            metaVariableNames.insert(thenDd.getContainedMetaVariables().begin(), thenDd.getContainedMetaVariables().end());
            metaVariableNames.insert(elseDd.getContainedMetaVariables().begin(), elseDd.getContainedMetaVariables().end());
            
            return Dd<DdType::CUDD>(this->getDdManager(), this->getCuddAdd().Ite(thenDd.getCuddAdd(), elseDd.getCuddAdd()), metaVariableNames);
        }
        
        Dd<DdType::CUDD> Dd<DdType::CUDD>::operator+(Dd<DdType::CUDD> const& other) const {
            Dd<DdType::CUDD> result(*this);
            result += other;
            return result;
        }
        
        Dd<DdType::CUDD>& Dd<DdType::CUDD>::operator+=(Dd<DdType::CUDD> const& other) {
            this->cuddAdd += other.getCuddAdd();
            
            // Join the variable sets of the two participating DDs.
            this->getContainedMetaVariables().insert(other.getContainedMetaVariables().begin(), other.getContainedMetaVariables().end());
            
            return *this;
        }
        
        Dd<DdType::CUDD> Dd<DdType::CUDD>::operator*(Dd<DdType::CUDD> const& other) const {
            Dd<DdType::CUDD> result(*this);
            result *= other;
            return result;
        }
        
        Dd<DdType::CUDD>& Dd<DdType::CUDD>::operator*=(Dd<DdType::CUDD> const& other) {
            this->cuddAdd *= other.getCuddAdd();
            
            // Join the variable sets of the two participating DDs.
            this->getContainedMetaVariables().insert(other.getContainedMetaVariables().begin(), other.getContainedMetaVariables().end());
            
            return *this;
        }
        
        Dd<DdType::CUDD> Dd<DdType::CUDD>::operator-(Dd<DdType::CUDD> const& other) const {
            Dd<DdType::CUDD> result(*this);
            result -= other;
            return result;
        }
        
        Dd<DdType::CUDD> Dd<DdType::CUDD>::operator-() const {
            return this->getDdManager()->getZero() - *this;
        }
        
        Dd<DdType::CUDD>& Dd<DdType::CUDD>::operator-=(Dd<DdType::CUDD> const& other) {
            this->cuddAdd -= other.getCuddAdd();
            
            // Join the variable sets of the two participating DDs.
            this->getContainedMetaVariables().insert(other.getContainedMetaVariables().begin(), other.getContainedMetaVariables().end());
            
            return *this;
        }
        
        Dd<DdType::CUDD> Dd<DdType::CUDD>::operator/(Dd<DdType::CUDD> const& other) const {
            Dd<DdType::CUDD> result(*this);
            result /= other;
            return result;
        }
        
        Dd<DdType::CUDD>& Dd<DdType::CUDD>::operator/=(Dd<DdType::CUDD> const& other) {
            this->cuddAdd = this->cuddAdd.Divide(other.getCuddAdd());
            
            // Join the variable sets of the two participating DDs.
            this->getContainedMetaVariables().insert(other.getContainedMetaVariables().begin(), other.getContainedMetaVariables().end());
            
            return *this;
        }
        
        Dd<DdType::CUDD> Dd<DdType::CUDD>::operator!() const {
            Dd<DdType::CUDD> result(*this);
            result.complement();
            return result;
        }

        Dd<DdType::CUDD> Dd<DdType::CUDD>::operator&&(Dd<DdType::CUDD> const& other) const {
            std::set<storm::expressions::Variable> metaVariables(this->getContainedMetaVariables());
            metaVariables.insert(other.getContainedMetaVariables().begin(), other.getContainedMetaVariables().end());
            
            // Rewrite a and b to not((not a) or (not b)). 
            return Dd<DdType::CUDD>(this->getDdManager(), ~(~this->getCuddAdd()).Or(~other.getCuddAdd()), metaVariables);
        }
        
        Dd<DdType::CUDD> Dd<DdType::CUDD>::operator||(Dd<DdType::CUDD> const& other) const {
            std::set<storm::expressions::Variable> metaVariables(this->getContainedMetaVariables());
            metaVariables.insert(other.getContainedMetaVariables().begin(), other.getContainedMetaVariables().end());
            return Dd<DdType::CUDD>(this->getDdManager(), this->getCuddAdd().Or(other.getCuddAdd()), metaVariables);
        }
        
        Dd<DdType::CUDD>& Dd<DdType::CUDD>::complement() {
            this->cuddAdd = ~this->cuddAdd;
            return *this;
        }
        
        Dd<DdType::CUDD> Dd<DdType::CUDD>::equals(Dd<DdType::CUDD> const& other) const {
            std::set<storm::expressions::Variable> metaVariables(this->getContainedMetaVariables());
            metaVariables.insert(other.getContainedMetaVariables().begin(), other.getContainedMetaVariables().end());
            return Dd<DdType::CUDD>(this->getDdManager(), this->getCuddAdd().Equals(other.getCuddAdd()), metaVariables);
        }
        
        Dd<DdType::CUDD> Dd<DdType::CUDD>::notEquals(Dd<DdType::CUDD> const& other) const {
            std::set<storm::expressions::Variable> metaVariables(this->getContainedMetaVariables());
            metaVariables.insert(other.getContainedMetaVariables().begin(), other.getContainedMetaVariables().end());
            return Dd<DdType::CUDD>(this->getDdManager(), this->getCuddAdd().NotEquals(other.getCuddAdd()), metaVariables);
        }
        
        Dd<DdType::CUDD> Dd<DdType::CUDD>::less(Dd<DdType::CUDD> const& other) const {
            std::set<storm::expressions::Variable> metaVariables(this->getContainedMetaVariables());
            metaVariables.insert(other.getContainedMetaVariables().begin(), other.getContainedMetaVariables().end());
            return Dd<DdType::CUDD>(this->getDdManager(), this->getCuddAdd().LessThan(other.getCuddAdd()), metaVariables);
        }
        
        Dd<DdType::CUDD> Dd<DdType::CUDD>::lessOrEqual(Dd<DdType::CUDD> const& other) const {
            std::set<storm::expressions::Variable> metaVariables(this->getContainedMetaVariables());
            metaVariables.insert(other.getContainedMetaVariables().begin(), other.getContainedMetaVariables().end());
            return Dd<DdType::CUDD>(this->getDdManager(), this->getCuddAdd().LessThanOrEqual(other.getCuddAdd()), metaVariables);
        }
        
        Dd<DdType::CUDD> Dd<DdType::CUDD>::greater(Dd<DdType::CUDD> const& other) const {
            std::set<storm::expressions::Variable> metaVariables(this->getContainedMetaVariables());
            metaVariables.insert(other.getContainedMetaVariables().begin(), other.getContainedMetaVariables().end());
            return Dd<DdType::CUDD>(this->getDdManager(), this->getCuddAdd().GreaterThan(other.getCuddAdd()), metaVariables);
        }
        
        Dd<DdType::CUDD> Dd<DdType::CUDD>::greaterOrEqual(Dd<DdType::CUDD> const& other) const {
            std::set<storm::expressions::Variable> metaVariables(this->getContainedMetaVariables());
            metaVariables.insert(other.getContainedMetaVariables().begin(), other.getContainedMetaVariables().end());
            return Dd<DdType::CUDD>(this->getDdManager(), this->getCuddAdd().GreaterThanOrEqual(other.getCuddAdd()), metaVariables);
        }
        
        Dd<DdType::CUDD> Dd<DdType::CUDD>::minimum(Dd<DdType::CUDD> const& other) const {
            std::set<storm::expressions::Variable> metaVariables(this->getContainedMetaVariables());
            metaVariables.insert(other.getContainedMetaVariables().begin(), other.getContainedMetaVariables().end());
            return Dd<DdType::CUDD>(this->getDdManager(), this->getCuddAdd().Minimum(other.getCuddAdd()), metaVariables);
        }

        Dd<DdType::CUDD> Dd<DdType::CUDD>::maximum(Dd<DdType::CUDD> const& other) const {
            std::set<storm::expressions::Variable> metaVariables(this->getContainedMetaVariables());
            metaVariables.insert(other.getContainedMetaVariables().begin(), other.getContainedMetaVariables().end());
            return Dd<DdType::CUDD>(this->getDdManager(), this->getCuddAdd().Maximum(other.getCuddAdd()), metaVariables);
        }

        Dd<DdType::CUDD> Dd<DdType::CUDD>::existsAbstract(std::set<storm::expressions::Variable> const& metaVariables) const {
            Dd<DdType::CUDD> cubeDd(this->getDdManager()->getOne());
            
            std::set<storm::expressions::Variable> newMetaVariables = this->getContainedMetaVariables();
            for (auto const& metaVariable : metaVariables) {
                // First check whether the DD contains the meta variable and erase it, if this is the case.
                STORM_LOG_THROW(this->containsMetaVariable(metaVariable), storm::exceptions::InvalidArgumentException, "Cannot abstract from meta variable '" << metaVariable.getName() << "' that is not present in the DD.");
                newMetaVariables.erase(metaVariable);
                
                DdMetaVariable<DdType::CUDD> const& ddMetaVariable = this->getDdManager()->getMetaVariable(metaVariable);
                cubeDd *= ddMetaVariable.getCube();
            }
            
            return Dd<DdType::CUDD>(this->getDdManager(), this->cuddAdd.OrAbstract(cubeDd.getCuddAdd()), newMetaVariables);
        }
        
        Dd<DdType::CUDD> Dd<DdType::CUDD>::universalAbstract(std::set<storm::expressions::Variable> const& metaVariables) const {
            Dd<DdType::CUDD> cubeDd(this->getDdManager()->getOne());
            
            std::set<storm::expressions::Variable> newMetaVariables = this->getContainedMetaVariables();
            for (auto const& metaVariable : metaVariables) {
                // First check whether the DD contains the meta variable and erase it, if this is the case.
                STORM_LOG_THROW(this->containsMetaVariable(metaVariable), storm::exceptions::InvalidArgumentException, "Cannot abstract from meta variable '" << metaVariable.getName() << "' that is not present in the DD.");
                newMetaVariables.erase(metaVariable);
                
                DdMetaVariable<DdType::CUDD> const& ddMetaVariable = this->getDdManager()->getMetaVariable(metaVariable);
                cubeDd *= ddMetaVariable.getCube();
            }
            
            return Dd<DdType::CUDD>(this->getDdManager(), this->cuddAdd.UnivAbstract(cubeDd.getCuddAdd()), newMetaVariables);
        }
        
        Dd<DdType::CUDD> Dd<DdType::CUDD>::sumAbstract(std::set<storm::expressions::Variable> const& metaVariables) const {
            Dd<DdType::CUDD> cubeDd(this->getDdManager()->getOne());
            
            std::set<storm::expressions::Variable> newMetaVariables = this->getContainedMetaVariables();
            for (auto const& metaVariable : metaVariables) {
                // First check whether the DD contains the meta variable and erase it, if this is the case.
                STORM_LOG_THROW(this->containsMetaVariable(metaVariable), storm::exceptions::InvalidArgumentException, "Cannot abstract from meta variable '" << metaVariable.getName() << "' that is not present in the DD.");
                newMetaVariables.erase(metaVariable);
                
                DdMetaVariable<DdType::CUDD> const& ddMetaVariable = this->getDdManager()->getMetaVariable(metaVariable);
                cubeDd *= ddMetaVariable.getCube();
            }
            
            return Dd<DdType::CUDD>(this->getDdManager(), this->cuddAdd.ExistAbstract(cubeDd.getCuddAdd()), newMetaVariables);
        }
        
        Dd<DdType::CUDD> Dd<DdType::CUDD>::minAbstract(std::set<storm::expressions::Variable> const& metaVariables) const {
            Dd<DdType::CUDD> cubeDd(this->getDdManager()->getOne());
            
            std::set<storm::expressions::Variable> newMetaVariables = this->getContainedMetaVariables();
            for (auto const& metaVariable : metaVariables) {
                // First check whether the DD contains the meta variable and erase it, if this is the case.
                STORM_LOG_THROW(this->containsMetaVariable(metaVariable), storm::exceptions::InvalidArgumentException, "Cannot abstract from meta variable '" << metaVariable.getName() << "' that is not present in the DD.");
                newMetaVariables.erase(metaVariable);
                
                DdMetaVariable<DdType::CUDD> const& ddMetaVariable = this->getDdManager()->getMetaVariable(metaVariable);
                cubeDd *= ddMetaVariable.getCube();
            }
            
            return Dd<DdType::CUDD>(this->getDdManager(), this->cuddAdd.MinAbstract(cubeDd.getCuddAdd()), newMetaVariables);
        }
        
        Dd<DdType::CUDD> Dd<DdType::CUDD>::maxAbstract(std::set<storm::expressions::Variable> const& metaVariables) const {
            Dd<DdType::CUDD> cubeDd(this->getDdManager()->getOne());
            
            std::set<storm::expressions::Variable> newMetaVariables = this->getContainedMetaVariables();
            for (auto const& metaVariable : metaVariables) {
                // First check whether the DD contains the meta variable and erase it, if this is the case.
                STORM_LOG_THROW(this->containsMetaVariable(metaVariable), storm::exceptions::InvalidArgumentException, "Cannot abstract from meta variable '" << metaVariable.getName() << "' that is not present in the DD.");
                newMetaVariables.erase(metaVariable);
                
                DdMetaVariable<DdType::CUDD> const& ddMetaVariable = this->getDdManager()->getMetaVariable(metaVariable);
                cubeDd *= ddMetaVariable.getCube();
            }
            
            return Dd<DdType::CUDD>(this->getDdManager(), this->cuddAdd.MaxAbstract(cubeDd.getCuddAdd()), newMetaVariables);
        }
        
        bool Dd<DdType::CUDD>::equalModuloPrecision(Dd<DdType::CUDD> const& other, double precision, bool relative) const {
            if (relative) {
                return this->getCuddAdd().EqualSupNormRel(other.getCuddAdd(), precision);
            } else {
                return this->getCuddAdd().EqualSupNorm(other.getCuddAdd(), precision);
            }
        }
        
        void Dd<DdType::CUDD>::swapVariables(std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& metaVariablePairs) {
            std::vector<ADD> from;
            std::vector<ADD> to;
            for (auto const& metaVariablePair : metaVariablePairs) {
                DdMetaVariable<DdType::CUDD> const& variable1 = this->getDdManager()->getMetaVariable(metaVariablePair.first);
                DdMetaVariable<DdType::CUDD> const& variable2 = this->getDdManager()->getMetaVariable(metaVariablePair.second);

                // Check if it's legal so swap the meta variables.
                if (variable1.getNumberOfDdVariables() != variable2.getNumberOfDdVariables()) {
                    throw storm::exceptions::InvalidArgumentException() << "Unable to swap meta variables with different size.";
                }
                
                // Keep track of the contained meta variables in the DD.
                bool containsVariable1 = this->containsMetaVariable(metaVariablePair.first);
                bool containsVariable2 = this->containsMetaVariable(metaVariablePair.second);
                if (containsVariable1 && !containsVariable2) {
                    this->removeContainedMetaVariable(metaVariablePair.first);
                    this->addContainedMetaVariable(metaVariablePair.second);
                } else if (!containsVariable1 && containsVariable2) {
                    this->removeContainedMetaVariable(metaVariablePair.second);
                    this->addContainedMetaVariable(metaVariablePair.first);
                }
                
                // Add the variables to swap to the corresponding vectors.
                for (auto const& ddVariable : variable1.getDdVariables()) {
                    from.push_back(ddVariable.getCuddAdd());
                }
                for (auto const& ddVariable : variable2.getDdVariables()) {
                    to.push_back(ddVariable.getCuddAdd());
                }
            }
            
            // Finally, call CUDD to swap the variables.
            this->cuddAdd = this->cuddAdd.SwapVariables(from, to);
        }
        
        Dd<DdType::CUDD> Dd<DdType::CUDD>::multiplyMatrix(Dd<DdType::CUDD> const& otherMatrix, std::set<storm::expressions::Variable> const& summationMetaVariables) const {
            std::vector<ADD> summationDdVariables;
            
            // Create the CUDD summation variables.
            for (auto const& metaVariable : summationMetaVariables) {
                for (auto const& ddVariable : this->getDdManager()->getMetaVariable(metaVariable).getDdVariables()) {
                    summationDdVariables.push_back(ddVariable.getCuddAdd());
                }
            }
            
            std::set<storm::expressions::Variable> unionOfMetaVariables;
            std::set_union(this->getContainedMetaVariables().begin(), this->getContainedMetaVariables().end(), otherMatrix.getContainedMetaVariables().begin(), otherMatrix.getContainedMetaVariables().end(), std::inserter(unionOfMetaVariables, unionOfMetaVariables.begin()));
            std::set<storm::expressions::Variable> containedMetaVariables;
            std::set_difference(unionOfMetaVariables.begin(), unionOfMetaVariables.end(), summationMetaVariables.begin(), summationMetaVariables.end(), std::inserter(containedMetaVariables, containedMetaVariables.begin()));
            
            return Dd<DdType::CUDD>(this->getDdManager(), this->cuddAdd.MatrixMultiply(otherMatrix.getCuddAdd(), summationDdVariables), containedMetaVariables);
        }
        
        Dd<DdType::CUDD> Dd<DdType::CUDD>::greater(double value) const {
            return Dd<DdType::CUDD>(this->getDdManager(), this->getCuddAdd().BddStrictThreshold(value).Add(), this->getContainedMetaVariables());
        }
        
        Dd<DdType::CUDD> Dd<DdType::CUDD>::greaterOrEqual(double value) const {
            return Dd<DdType::CUDD>(this->getDdManager(), this->getCuddAdd().BddThreshold(value).Add(), this->getContainedMetaVariables());
        }

        Dd<DdType::CUDD> Dd<DdType::CUDD>::notZero() const {
            return Dd<DdType::CUDD>(this->getDdManager(), this->getCuddAdd().BddPattern().Add(), this->getContainedMetaVariables());
        }
        
        Dd<DdType::CUDD> Dd<DdType::CUDD>::constrain(Dd<DdType::CUDD> const& constraint) const {
            std::set<storm::expressions::Variable> metaVariables(this->getContainedMetaVariables());
            metaVariables.insert(constraint.getContainedMetaVariables().begin(), constraint.getContainedMetaVariables().end());
            
            return Dd<DdType::CUDD>(this->getDdManager(), this->getCuddAdd().Constrain(constraint.getCuddAdd()), metaVariables);
        }
        
        Dd<DdType::CUDD> Dd<DdType::CUDD>::restrict(Dd<DdType::CUDD> const& constraint) const {
            std::set<storm::expressions::Variable> metaVariables(this->getContainedMetaVariables());
            metaVariables.insert(constraint.getContainedMetaVariables().begin(), constraint.getContainedMetaVariables().end());
            
            return Dd<DdType::CUDD>(this->getDdManager(), this->getCuddAdd().Restrict(constraint.getCuddAdd()), metaVariables);
        }
        
        Dd<DdType::CUDD> Dd<DdType::CUDD>::getSupport() const {
            return Dd<DdType::CUDD>(this->getDdManager(), this->getCuddAdd().Support().Add(), this->getContainedMetaVariables());
        }

        uint_fast64_t Dd<DdType::CUDD>::getNonZeroCount() const {
            std::size_t numberOfDdVariables = 0;
            for (auto const& metaVariable : this->getContainedMetaVariables()) {
                numberOfDdVariables += this->getDdManager()->getMetaVariable(metaVariable).getNumberOfDdVariables();
            }
            return static_cast<uint_fast64_t>(this->cuddAdd.CountMinterm(static_cast<int>(numberOfDdVariables)));
        }
        
        uint_fast64_t Dd<DdType::CUDD>::getLeafCount() const {
            return static_cast<uint_fast64_t>(this->cuddAdd.CountLeaves());
        }
        
        uint_fast64_t Dd<DdType::CUDD>::getNodeCount() const {
            return static_cast<uint_fast64_t>(this->cuddAdd.nodeCount());
        }
        
        double Dd<DdType::CUDD>::getMin() const {
            ADD constantMinAdd = this->cuddAdd.FindMin();
            return static_cast<double>(Cudd_V(constantMinAdd.getNode()));
        }
        
        double Dd<DdType::CUDD>::getMax() const {
            ADD constantMaxAdd = this->cuddAdd.FindMax();
            return static_cast<double>(Cudd_V(constantMaxAdd.getNode()));
        }
        
        void Dd<DdType::CUDD>::setValue(storm::expressions::Variable const& metaVariable, int_fast64_t variableValue, double targetValue) {
            std::map<storm::expressions::Variable, int_fast64_t> metaVariableToValueMap;
            metaVariableToValueMap.emplace(metaVariable, variableValue);
            this->setValue(metaVariableToValueMap, targetValue);
        }
        
        void Dd<DdType::CUDD>::setValue(storm::expressions::Variable const& metaVariable1, int_fast64_t variableValue1, storm::expressions::Variable const& metaVariable2, int_fast64_t variableValue2, double targetValue) {
            std::map<storm::expressions::Variable, int_fast64_t> metaVariableToValueMap;
            metaVariableToValueMap.emplace(metaVariable1, variableValue1);
            metaVariableToValueMap.emplace(metaVariable2, variableValue2);
            this->setValue(metaVariableToValueMap, targetValue);
        }
        
        void Dd<DdType::CUDD>::setValue(std::map<storm::expressions::Variable, int_fast64_t> const& metaVariableToValueMap, double targetValue) {
            Dd<DdType::CUDD> valueEncoding(this->getDdManager()->getOne());
            for (auto const& nameValuePair : metaVariableToValueMap) {
                valueEncoding *= this->getDdManager()->getEncoding(nameValuePair.first, nameValuePair.second);
                // Also record that the DD now contains the meta variable.
                this->addContainedMetaVariable(nameValuePair.first);
            }
            
            this->cuddAdd = valueEncoding.getCuddAdd().Ite(this->getDdManager()->getConstant(targetValue).getCuddAdd(), this->cuddAdd);
        }
        
        double Dd<DdType::CUDD>::getValue(std::map<storm::expressions::Variable, int_fast64_t> const& metaVariableToValueMap) const {
            std::set<storm::expressions::Variable> remainingMetaVariables(this->getContainedMetaVariables());
            Dd<DdType::CUDD> valueEncoding(this->getDdManager()->getOne());
            for (auto const& nameValuePair : metaVariableToValueMap) {
                valueEncoding *= this->getDdManager()->getEncoding(nameValuePair.first, nameValuePair.second);
                if (this->containsMetaVariable(nameValuePair.first)) {
                    remainingMetaVariables.erase(nameValuePair.first);
                }
            }
            
            if (!remainingMetaVariables.empty()) {
                throw storm::exceptions::InvalidArgumentException() << "Cannot evaluate function for which not all inputs were given.";
            }
            
            Dd<DdType::CUDD> value = *this * valueEncoding;
            value = value.sumAbstract(this->getContainedMetaVariables());
            return static_cast<double>(Cudd_V(value.getCuddAdd().getNode()));
        }
        
        bool Dd<DdType::CUDD>::isOne() const {
            return *this == this->getDdManager()->getOne();
        }
        
        bool Dd<DdType::CUDD>::isZero() const {
            return *this == this->getDdManager()->getZero();
        }
        
        bool Dd<DdType::CUDD>::isConstant() const {
            return Cudd_IsConstant(this->cuddAdd.getNode());
        }
        
        uint_fast64_t Dd<DdType::CUDD>::getIndex() const {
            return static_cast<uint_fast64_t>(this->getCuddAdd().NodeReadIndex());
        }
        
        template<typename ValueType>
        std::vector<ValueType> Dd<DdType::CUDD>::toVector() const {
            return this->toVector<ValueType>(Odd<DdType::CUDD>(*this));
        }
        
        template<typename ValueType>
        std::vector<ValueType> Dd<DdType::CUDD>::toVector(Odd<DdType::CUDD> const& rowOdd) const {
            std::vector<ValueType> result(rowOdd.getTotalOffset());
            std::vector<uint_fast64_t> ddVariableIndices = this->getSortedVariableIndices();
            addToVectorRec(this->getCuddAdd().getNode(), 0, ddVariableIndices.size(), 0, rowOdd, ddVariableIndices, result);
            return result;
        }
        
        storm::storage::SparseMatrix<double> Dd<DdType::CUDD>::toMatrix() const {
            std::set<storm::expressions::Variable> rowVariables;
            std::set<storm::expressions::Variable> columnVariables;
            
            for (auto const& variable : this->getContainedMetaVariables()) {
                if (variable.getName().size() > 0 && variable.getName().back() == '\'') {
                    columnVariables.insert(variable);
                } else {
                    rowVariables.insert(variable);
                }
            }

            return toMatrix(rowVariables, columnVariables, Odd<DdType::CUDD>(this->existsAbstract(rowVariables)), Odd<DdType::CUDD>(this->existsAbstract(columnVariables)));
        }
        
        storm::storage::SparseMatrix<double> Dd<DdType::CUDD>::toMatrix(storm::dd::Odd<DdType::CUDD> const& rowOdd, storm::dd::Odd<DdType::CUDD> const& columnOdd) const {
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
        
        storm::storage::SparseMatrix<double> Dd<DdType::CUDD>::toMatrix(std::set<storm::expressions::Variable> const& rowMetaVariables, std::set<storm::expressions::Variable> const& columnMetaVariables, storm::dd::Odd<DdType::CUDD> const& rowOdd, storm::dd::Odd<DdType::CUDD> const& columnOdd) const {
            std::vector<uint_fast64_t> ddRowVariableIndices;
            std::vector<uint_fast64_t> ddColumnVariableIndices;
            
            for (auto const& variable : rowMetaVariables) {
                DdMetaVariable<DdType::CUDD> const& metaVariable = this->getDdManager()->getMetaVariable(variable);
                for (auto const& ddVariable : metaVariable.getDdVariables()) {
                    ddRowVariableIndices.push_back(ddVariable.getIndex());
                }
            }
            for (auto const& variable : columnMetaVariables) {
                DdMetaVariable<DdType::CUDD> const& metaVariable = this->getDdManager()->getMetaVariable(variable);
                for (auto const& ddVariable : metaVariable.getDdVariables()) {
                    ddColumnVariableIndices.push_back(ddVariable.getIndex());
                }
            }
            
            // Prepare the vectors that represent the matrix.
            std::vector<uint_fast64_t> rowIndications(rowOdd.getTotalOffset() + 1);
            std::vector<storm::storage::MatrixEntry<uint_fast64_t, double>> columnsAndValues(this->getNonZeroCount());
            
            // Create a trivial row grouping.
            std::vector<uint_fast64_t> trivialRowGroupIndices(rowIndications.size());
            uint_fast64_t i = 0;
            for (auto& entry : trivialRowGroupIndices) {
                entry = i;
                ++i;
            }
            
            // Use the toMatrixRec function to compute the number of elements in each row. Using the flag, we prevent
            // it from actually generating the entries in the entry vector.
            toMatrixRec(this->getCuddAdd().getNode(), rowIndications, columnsAndValues, trivialRowGroupIndices, rowOdd, columnOdd, 0, 0, ddRowVariableIndices.size() + ddColumnVariableIndices.size(), 0, 0, ddRowVariableIndices, ddColumnVariableIndices, false);
            
            // TODO: counting might be faster by just summing over the primed variables and then using the ODD to convert
            // the resulting (DD) vector to an explicit vector.
            
            // Now that we computed the number of entries in each row, compute the corresponding offsets in the entry vector.
            uint_fast64_t tmp = 0;
            uint_fast64_t tmp2 = 0;
            for (uint_fast64_t i = 1; i < rowIndications.size(); ++i) {
                tmp2 = rowIndications[i];
                rowIndications[i] = rowIndications[i - 1] + tmp;
                std::swap(tmp, tmp2);
            }
            rowIndications[0] = 0;
            
            // Now actually fill the entry vector.
            toMatrixRec(this->getCuddAdd().getNode(), rowIndications, columnsAndValues, trivialRowGroupIndices, rowOdd, columnOdd, 0, 0, ddRowVariableIndices.size() + ddColumnVariableIndices.size(), 0, 0, ddRowVariableIndices, ddColumnVariableIndices, true);
            
            // Since the last call to toMatrixRec modified the rowIndications, we need to restore the correct values.
            for (uint_fast64_t i = rowIndications.size() - 1; i > 0; --i) {
                rowIndications[i] = rowIndications[i - 1];
            }
            rowIndications[0] = 0;
            
            // Construct matrix and return result.
            return storm::storage::SparseMatrix<double>(columnOdd.getTotalOffset(), std::move(rowIndications), std::move(columnsAndValues), std::move(trivialRowGroupIndices));
        }
        
        storm::storage::SparseMatrix<double> Dd<DdType::CUDD>::toMatrix(std::set<storm::expressions::Variable> const& rowMetaVariables, std::set<storm::expressions::Variable> const& columnMetaVariables, std::set<storm::expressions::Variable> const& groupMetaVariables, storm::dd::Odd<DdType::CUDD> const& rowOdd, storm::dd::Odd<DdType::CUDD> const& columnOdd) const {
            std::vector<uint_fast64_t> ddRowVariableIndices;
            std::vector<uint_fast64_t> ddColumnVariableIndices;
            std::vector<uint_fast64_t> ddGroupVariableIndices;
            std::set<storm::expressions::Variable> rowAndColumnMetaVariables;
            
            for (auto const& variable : rowMetaVariables) {
                DdMetaVariable<DdType::CUDD> const& metaVariable = this->getDdManager()->getMetaVariable(variable);
                for (auto const& ddVariable : metaVariable.getDdVariables()) {
                    ddRowVariableIndices.push_back(ddVariable.getIndex());
                }
                rowAndColumnMetaVariables.insert(variable);
            }
            for (auto const& variable : columnMetaVariables) {
                DdMetaVariable<DdType::CUDD> const& metaVariable = this->getDdManager()->getMetaVariable(variable);
                for (auto const& ddVariable : metaVariable.getDdVariables()) {
                    ddColumnVariableIndices.push_back(ddVariable.getIndex());
                }
                rowAndColumnMetaVariables.insert(variable);
            }
            for (auto const& variable : groupMetaVariables) {
                DdMetaVariable<DdType::CUDD> const& metaVariable = this->getDdManager()->getMetaVariable(variable);
                for (auto const& ddVariable : metaVariable.getDdVariables()) {
                    ddGroupVariableIndices.push_back(ddVariable.getIndex());
                }
            }

            // TODO: assert that the group variables are at the very top of the variable ordering?
            
            // Start by computing the offsets (in terms of rows) for each row group.
            Dd<DdType::CUDD> stateToNumberOfChoices = this->notZero().existsAbstract(columnMetaVariables).sumAbstract(groupMetaVariables);
            std::vector<uint_fast64_t> rowGroupIndices = stateToNumberOfChoices.toVector<uint_fast64_t>(rowOdd);
            rowGroupIndices.resize(rowGroupIndices.size() + 1);
            uint_fast64_t tmp = 0;
            uint_fast64_t tmp2 = 0;
            for (uint_fast64_t i = 1; i < rowGroupIndices.size(); ++i) {
                tmp2 = rowGroupIndices[i];
                rowGroupIndices[i] = rowGroupIndices[i - 1] + tmp;
                std::swap(tmp, tmp2);
            }
            rowGroupIndices[0] = 0;
            
            // Next, we split the matrix into one for each group. This only works if the group variables are at the very
            // top.
            std::vector<Dd<DdType::CUDD>> groups;
            splitGroupsRec(this->getCuddAdd().getNode(), groups, ddGroupVariableIndices, 0, ddGroupVariableIndices.size(), rowAndColumnMetaVariables);
            
            // Create the actual storage for the non-zero entries.
            std::vector<storm::storage::MatrixEntry<uint_fast64_t, double>> columnsAndValues(this->getNonZeroCount());
            
            // Now compute the indices at which the individual rows start.
            std::vector<uint_fast64_t> rowIndications(rowGroupIndices.back() + 1);
            std::vector<storm::dd::Dd<DdType::CUDD>> statesWithGroupEnabled(groups.size());
            for (uint_fast64_t i = 0; i < groups.size(); ++i) {
                auto const& dd = groups[i];
                
                toMatrixRec(dd.getCuddAdd().getNode(), rowIndications, columnsAndValues, rowGroupIndices, rowOdd, columnOdd, 0, 0, ddRowVariableIndices.size() + ddColumnVariableIndices.size(), 0, 0, ddRowVariableIndices, ddColumnVariableIndices, false);
                
                statesWithGroupEnabled[i] = dd.notZero().existsAbstract(columnMetaVariables);
                addToVectorRec(statesWithGroupEnabled[i].getCuddAdd().getNode(), 0, ddRowVariableIndices.size(), 0, rowOdd, ddRowVariableIndices, rowGroupIndices);
            }
            
            // Since we modified the rowGroupIndices, we need to restore the correct values.
            for (uint_fast64_t i = rowGroupIndices.size() - 1; i > 0; --i) {
                rowGroupIndices[i] = rowGroupIndices[i - 1];
            }
            rowGroupIndices[0] = 0;
            
            // Now that we computed the number of entries in each row, compute the corresponding offsets in the entry vector.
            tmp = 0;
            tmp2 = 0;
            for (uint_fast64_t i = 1; i < rowIndications.size(); ++i) {
                tmp2 = rowIndications[i];
                rowIndications[i] = rowIndications[i - 1] + tmp;
                std::swap(tmp, tmp2);
            }
            rowIndications[0] = 0;
            
            // Now actually fill the entry vector.
            for (uint_fast64_t i = 0; i < groups.size(); ++i) {
                auto const& dd = groups[i];

                toMatrixRec(dd.getCuddAdd().getNode(), rowIndications, columnsAndValues, rowGroupIndices, rowOdd, columnOdd, 0, 0, ddRowVariableIndices.size() + ddColumnVariableIndices.size(), 0, 0, ddRowVariableIndices, ddColumnVariableIndices, true);
                
                addToVectorRec(statesWithGroupEnabled[i].getCuddAdd().getNode(), 0, ddRowVariableIndices.size(), 0, rowOdd, ddRowVariableIndices, rowGroupIndices);
            }
            
            // Since we modified the rowGroupIndices, we need to restore the correct values.
            for (uint_fast64_t i = rowGroupIndices.size() - 1; i > 0; --i) {
                rowGroupIndices[i] = rowGroupIndices[i - 1];
            }
            rowGroupIndices[0] = 0;
            
            // Since the last call to toMatrixRec modified the rowIndications, we need to restore the correct values.
            for (uint_fast64_t i = rowIndications.size() - 1; i > 0; --i) {
                rowIndications[i] = rowIndications[i - 1];
            }
            rowIndications[0] = 0;
            
            return storm::storage::SparseMatrix<double>(columnOdd.getTotalOffset(), std::move(rowIndications), std::move(columnsAndValues), std::move(rowGroupIndices));
        }
        
        void Dd<DdType::CUDD>::toMatrixRec(DdNode const* dd, std::vector<uint_fast64_t>& rowIndications, std::vector<storm::storage::MatrixEntry<uint_fast64_t, double>>& columnsAndValues, std::vector<uint_fast64_t> const& rowGroupOffsets, Odd<DdType::CUDD> const& rowOdd, Odd<DdType::CUDD> const& columnOdd, uint_fast64_t currentRowLevel, uint_fast64_t currentColumnLevel, uint_fast64_t maxLevel, uint_fast64_t currentRowOffset, uint_fast64_t currentColumnOffset, std::vector<uint_fast64_t> const& ddRowVariableIndices, std::vector<uint_fast64_t> const& ddColumnVariableIndices, bool generateValues) const {
            // For the empty DD, we do not need to add any entries.
            if (dd == this->getDdManager()->getZero().getCuddAdd().getNode()) {
                return;
            }

            // If we are at the maximal level, the value to be set is stored as a constant in the DD.
            if (currentRowLevel + currentColumnLevel == maxLevel) {
                if (generateValues) {
                    columnsAndValues[rowIndications[rowGroupOffsets[currentRowOffset]]] = storm::storage::MatrixEntry<uint_fast64_t, double>(currentColumnOffset, Cudd_V(dd));
                }
                ++rowIndications[rowGroupOffsets[currentRowOffset]];
            } else {
                DdNode const* elseElse;
                DdNode const* elseThen;
                DdNode const* thenElse;
                DdNode const* thenThen;
                
                if (ddColumnVariableIndices[currentColumnLevel] < dd->index) {
                    elseElse = elseThen = thenElse = thenThen = dd;
                } else if (ddRowVariableIndices[currentColumnLevel] < dd->index) {
                    elseElse = thenElse = Cudd_E(dd);
                    elseThen = thenThen = Cudd_T(dd);
                } else {
                    DdNode const* elseNode = Cudd_E(dd);
                    if (ddColumnVariableIndices[currentColumnLevel] < elseNode->index) {
                        elseElse = elseThen = elseNode;
                    } else {
                        elseElse = Cudd_E(elseNode);
                        elseThen = Cudd_T(elseNode);
                    }

                    DdNode const* thenNode = Cudd_T(dd);
                    if (ddColumnVariableIndices[currentColumnLevel] < thenNode->index) {
                        thenElse = thenThen = thenNode;
                    } else {
                        thenElse = Cudd_E(thenNode);
                        thenThen = Cudd_T(thenNode);
                    }
                }
                
                // Visit else-else.
                toMatrixRec(elseElse, rowIndications, columnsAndValues, rowGroupOffsets, rowOdd.getElseSuccessor(), columnOdd.getElseSuccessor(), currentRowLevel + 1, currentColumnLevel + 1, maxLevel, currentRowOffset, currentColumnOffset, ddRowVariableIndices, ddColumnVariableIndices, generateValues);
                // Visit else-then.
                toMatrixRec(elseThen, rowIndications, columnsAndValues, rowGroupOffsets, rowOdd.getElseSuccessor(), columnOdd.getThenSuccessor(), currentRowLevel + 1, currentColumnLevel + 1, maxLevel, currentRowOffset, currentColumnOffset + columnOdd.getElseOffset(), ddRowVariableIndices, ddColumnVariableIndices, generateValues);
                // Visit then-else.
                toMatrixRec(thenElse, rowIndications, columnsAndValues, rowGroupOffsets, rowOdd.getThenSuccessor(), columnOdd.getElseSuccessor(), currentRowLevel + 1, currentColumnLevel + 1, maxLevel, currentRowOffset + rowOdd.getElseOffset(), currentColumnOffset, ddRowVariableIndices, ddColumnVariableIndices, generateValues);
                // Visit then-then.
                toMatrixRec(thenThen, rowIndications, columnsAndValues, rowGroupOffsets, rowOdd.getThenSuccessor(), columnOdd.getThenSuccessor(), currentRowLevel + 1, currentColumnLevel + 1, maxLevel, currentRowOffset + rowOdd.getElseOffset(), currentColumnOffset + columnOdd.getElseOffset(), ddRowVariableIndices, ddColumnVariableIndices, generateValues);
            }
        }
        
        void Dd<DdType::CUDD>::splitGroupsRec(DdNode* dd, std::vector<Dd<DdType::CUDD>>& groups, std::vector<uint_fast64_t> const& ddGroupVariableIndices, uint_fast64_t currentLevel, uint_fast64_t maxLevel, std::set<storm::expressions::Variable> const& remainingMetaVariables) const {
            // For the empty DD, we do not need to create a group.
            if (dd == this->getDdManager()->getZero().getCuddAdd().getNode()) {
                return;
            }
            
            if (currentLevel == maxLevel) {
                groups.push_back(Dd<DdType::CUDD>(this->getDdManager(), ADD(this->getDdManager()->getCuddManager(), dd), remainingMetaVariables));
            } else if (ddGroupVariableIndices[currentLevel] < dd->index) {
                splitGroupsRec(dd, groups, ddGroupVariableIndices, currentLevel + 1, maxLevel, remainingMetaVariables);
                splitGroupsRec(dd, groups, ddGroupVariableIndices, currentLevel + 1, maxLevel, remainingMetaVariables);
            } else {
                splitGroupsRec(Cudd_E(dd), groups, ddGroupVariableIndices, currentLevel + 1, maxLevel, remainingMetaVariables);
                splitGroupsRec(Cudd_T(dd), groups, ddGroupVariableIndices, currentLevel + 1, maxLevel, remainingMetaVariables);
            }
        }
        
        template<typename ValueType>
        void Dd<DdType::CUDD>::addToVectorRec(DdNode const* dd, uint_fast64_t currentLevel, uint_fast64_t maxLevel, uint_fast64_t currentOffset, Odd<DdType::CUDD> const& odd, std::vector<uint_fast64_t> const& ddVariableIndices, std::vector<ValueType>& targetVector) const {
            // For the empty DD, we do not need to add any entries.
            if (dd == this->getDdManager()->getZero().getCuddAdd().getNode()) {
                return;
            }
            
            // If we are at the maximal level, the value to be set is stored as a constant in the DD.
            if (currentLevel == maxLevel) {
                targetVector[currentOffset] += static_cast<ValueType>(Cudd_V(dd));
            } else if (ddVariableIndices[currentLevel] < dd->index) {
                // If we skipped a level, we need to enumerate the explicit entries for the case in which the bit is set
                // and for the one in which it is not set.
                addToVectorRec(dd, currentLevel + 1, maxLevel, currentOffset, odd.getElseSuccessor(), ddVariableIndices, targetVector);
                addToVectorRec(dd, currentLevel + 1, maxLevel, currentOffset + odd.getElseOffset(), odd.getThenSuccessor(), ddVariableIndices, targetVector);
            } else {
                // Otherwise, we simply recursively call the function for both (different) cases.
                addToVectorRec(Cudd_E(dd), currentLevel + 1, maxLevel, currentOffset, odd.getElseSuccessor(), ddVariableIndices, targetVector);
                addToVectorRec(Cudd_T(dd), currentLevel + 1, maxLevel, currentOffset + odd.getElseOffset(), odd.getThenSuccessor(), ddVariableIndices, targetVector);
            }
        }
        
        std::vector<uint_fast64_t> Dd<DdType::CUDD>::getSortedVariableIndices() const {
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
        
        bool Dd<DdType::CUDD>::containsMetaVariable(storm::expressions::Variable const& metaVariable) const {
            return containedMetaVariables.find(metaVariable) != containedMetaVariables.end();
        }
        
        bool Dd<DdType::CUDD>::containsMetaVariables(std::set<storm::expressions::Variable> const& metaVariables) const {
            for (auto const& metaVariable : metaVariables) {
                auto const& ddMetaVariable = containedMetaVariables.find(metaVariable);
                
                if (ddMetaVariable == containedMetaVariables.end()) {
                    return false;
                }
            }
            return true;
        }
        
        std::set<storm::expressions::Variable> const& Dd<DdType::CUDD>::getContainedMetaVariables() const {
            return this->containedMetaVariables;
        }
        
        std::set<storm::expressions::Variable>& Dd<DdType::CUDD>::getContainedMetaVariables() {
            return this->containedMetaVariables;
        }
        
        void Dd<DdType::CUDD>::exportToDot(std::string const& filename) const {
			std::vector<ADD> cuddAddVector = { this->cuddAdd };
			if (filename.empty()) {
				this->getDdManager()->getCuddManager().DumpDot(cuddAddVector);
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
				this->getDdManager()->getCuddManager().DumpDot(cuddAddVector, &ddVariableNames[0], &ddNames[0], filePointer);
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
        
        ADD Dd<DdType::CUDD>::getCuddAdd() {
            return this->cuddAdd;
        }
        
        ADD const& Dd<DdType::CUDD>::getCuddAdd() const {
            return this->cuddAdd;
        }
        
        void Dd<DdType::CUDD>::addContainedMetaVariable(storm::expressions::Variable const& metaVariable) {
            this->getContainedMetaVariables().insert(metaVariable);
        }

        void Dd<DdType::CUDD>::removeContainedMetaVariable(storm::expressions::Variable const& metaVariable) {
            this->getContainedMetaVariables().erase(metaVariable);
        }
        
        std::shared_ptr<DdManager<DdType::CUDD> const> Dd<DdType::CUDD>::getDdManager() const {
            return this->ddManager;
        }
        
        DdForwardIterator<DdType::CUDD> Dd<DdType::CUDD>::begin(bool enumerateDontCareMetaVariables) const {
            int* cube;
            double value;
            DdGen* generator = this->getCuddAdd().FirstCube(&cube, &value);
            return DdForwardIterator<DdType::CUDD>(this->getDdManager(), generator, cube, value, (Cudd_IsGenEmpty(generator) != 0), &this->getContainedMetaVariables(), enumerateDontCareMetaVariables);
        }
        
        DdForwardIterator<DdType::CUDD> Dd<DdType::CUDD>::end(bool enumerateDontCareMetaVariables) const {
            return DdForwardIterator<DdType::CUDD>(this->getDdManager(), nullptr, nullptr, 0, true, nullptr, enumerateDontCareMetaVariables);
        }
        
        storm::expressions::Expression Dd<DdType::CUDD>::toExpression() const {
            return toExpressionRecur(this->getCuddAdd().getNode(), this->getDdManager()->getDdVariables());
        }
        
        storm::expressions::Expression Dd<DdType::CUDD>::getMintermExpression() const {
            // Note that we first transform the ADD into a BDD to convert all non-zero terminals to ones and therefore
            // make the DD more compact.
            Dd<DdType::CUDD> tmp(this->getDdManager(), this->getCuddAdd().BddPattern().Add(), this->getContainedMetaVariables());
            return getMintermExpressionRecur(this->getDdManager()->getCuddManager().getManager(), this->getCuddAdd().BddPattern().getNode(), this->getDdManager()->getDdVariables());
        }
        
        storm::expressions::Expression Dd<DdType::CUDD>::toExpressionRecur(DdNode const* dd, std::vector<storm::expressions::Variable> const& variables) {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "This feature is currently unavailable.");
            // If the DD is a terminal node, we can simply return a constant expression.
//            if (Cudd_IsConstant(dd)) {
//                return storm::expressions::Expression::createDoubleLiteral(static_cast<double>(Cudd_V(dd)));
//            } else {
//                return storm::expressions::Expression::createBooleanVariable(variableNames[dd->index]).ite(toExpressionRecur(Cudd_T(dd), variableNames), toExpressionRecur(Cudd_E(dd), variableNames));
//            }
        }
        
        storm::expressions::Expression Dd<DdType::CUDD>::getMintermExpressionRecur(::DdManager* manager, DdNode const* dd, std::vector<storm::expressions::Variable> const& variables) {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "This feature is currently unavailable.");
            // If the DD is a terminal node, we can simply return a constant expression.
//            if (Cudd_IsConstant(dd)) {
//                if (Cudd_IsComplement(dd)) {
//                    return storm::expressions::Expression::createBooleanLiteral(false);
//                } else {
//                    return storm::expressions::Expression::createBooleanLiteral((dd == Cudd_ReadOne(manager)) ? true : false);
//                }
//            } else {
//                // Get regular versions of the pointers.
//                DdNode* regularDd = Cudd_Regular(dd);
//                DdNode* thenDd = Cudd_T(regularDd);
//                DdNode* elseDd = Cudd_E(regularDd);
//                
//                // Compute expression recursively.
//                storm::expressions::Expression result = storm::expressions::Expression::createBooleanVariable(variableNames[dd->index]).ite(getMintermExpressionRecur(manager, thenDd, variableNames), getMintermExpressionRecur(manager, elseDd, variableNames));
//                if (Cudd_IsComplement(dd)) {
//                    result = !result;
//                }
//                
//                return result;
//            }
        }
        
        std::ostream & operator<<(std::ostream& out, const Dd<DdType::CUDD>& dd) {
            dd.exportToDot();
            return out;
        }
        
        // Explicitly instantiate some templated functions.
        template std::vector<double> Dd<DdType::CUDD>::toVector() const;
        template std::vector<double> Dd<DdType::CUDD>::toVector(Odd<DdType::CUDD> const& rowOdd) const;
        template void Dd<DdType::CUDD>::addToVectorRec(DdNode const* dd, uint_fast64_t currentLevel, uint_fast64_t maxLevel, uint_fast64_t currentOffset, Odd<DdType::CUDD> const& odd, std::vector<uint_fast64_t> const& ddVariableIndices, std::vector<double>& targetVector) const;
        template std::vector<uint_fast64_t> Dd<DdType::CUDD>::toVector() const;
        template std::vector<uint_fast64_t> Dd<DdType::CUDD>::toVector(Odd<DdType::CUDD> const& rowOdd) const;
        template void Dd<DdType::CUDD>::addToVectorRec(DdNode const* dd, uint_fast64_t currentLevel, uint_fast64_t maxLevel, uint_fast64_t currentOffset, Odd<DdType::CUDD> const& odd, std::vector<uint_fast64_t> const& ddVariableIndices, std::vector<uint_fast64_t>& targetVector) const;
    }
}