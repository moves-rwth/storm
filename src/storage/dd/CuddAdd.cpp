#include <cstring>
#include <algorithm>
#include <boost/algorithm/string/join.hpp>

#include "src/storage/dd/CuddAdd.h"
#include "src/storage/dd/CuddBdd.h"
#include "src/storage/dd/CuddOdd.h"
#include "src/storage/dd/CuddDdManager.h"
#include "src/utility/vector.h"
#include "src/utility/macros.h"


#include "src/storage/SparseMatrix.h"


#include "src/exceptions/IllegalFunctionCallException.h"
#include "src/exceptions/InvalidArgumentException.h"
#include "src/exceptions/NotImplementedException.h"

namespace storm {
    namespace dd {
        Add<DdType::CUDD>::Add(std::shared_ptr<DdManager<DdType::CUDD> const> ddManager, ADD cuddAdd, std::set<storm::expressions::Variable> const& containedMetaVariables) : Dd<DdType::CUDD>(ddManager, containedMetaVariables), cuddAdd(cuddAdd) {
            // Intentionally left empty.
        }
        
        Add<DdType::CUDD>::Add(std::shared_ptr<DdManager<DdType::CUDD> const> ddManager, std::vector<double> const& values, Odd<DdType::CUDD> const& odd, std::set<storm::expressions::Variable> const& metaVariables) : Dd<DdType::CUDD>(ddManager, metaVariables) {
            cuddAdd = fromVector(ddManager, values, odd, metaVariables);
        }
        
        Bdd<DdType::CUDD> Add<DdType::CUDD>::toBdd() const {
            return Bdd<DdType::CUDD>(this->getDdManager(), this->getCuddAdd().BddPattern(), this->getContainedMetaVariables());
        }
        
        ADD Add<DdType::CUDD>::getCuddAdd() const {
            return this->cuddAdd;
        }
        
        DdNode* Add<DdType::CUDD>::getCuddDdNode() const {
            return this->getCuddAdd().getNode();
        }
        
        bool Add<DdType::CUDD>::operator==(Add<DdType::CUDD> const& other) const {
            return this->getCuddAdd() == other.getCuddAdd();
        }
        
        bool Add<DdType::CUDD>::operator!=(Add<DdType::CUDD> const& other) const {
            return !(*this == other);
        }
        
        Add<DdType::CUDD> Add<DdType::CUDD>::ite(Add<DdType::CUDD> const& thenDd, Add<DdType::CUDD> const& elseDd) const {
            std::set<storm::expressions::Variable> metaVariableNames;
            std::set_union(thenDd.getContainedMetaVariables().begin(), thenDd.getContainedMetaVariables().end(), elseDd.getContainedMetaVariables().begin(), elseDd.getContainedMetaVariables().end(), std::inserter(metaVariableNames, metaVariableNames.begin()));
            metaVariableNames.insert(this->getContainedMetaVariables().begin(), this->getContainedMetaVariables().end());
            
            return Add<DdType::CUDD>(this->getDdManager(), this->getCuddAdd().Ite(thenDd.getCuddAdd(), elseDd.getCuddAdd()), metaVariableNames);
        }
        
        Add<DdType::CUDD> Add<DdType::CUDD>::operator!() const {
            return Add<DdType::CUDD>(this->getDdManager(), ~this->getCuddAdd(), this->getContainedMetaVariables());
        }
        
        Add<DdType::CUDD> Add<DdType::CUDD>::operator||(Add<DdType::CUDD> const& other) const {
            Add<DdType::CUDD> result(*this);
            result |= other;
            return result;
        }
        
        Add<DdType::CUDD>& Add<DdType::CUDD>::operator|=(Add<DdType::CUDD> const& other) {
            this->addMetaVariables(other.getContainedMetaVariables());
            this->cuddAdd = this->getCuddAdd() | other.getCuddAdd();
            return *this;
        }
        
        Add<DdType::CUDD> Add<DdType::CUDD>::operator+(Add<DdType::CUDD> const& other) const {
            Add<DdType::CUDD> result(*this);
            result += other;
            return result;
        }
        
        Add<DdType::CUDD>& Add<DdType::CUDD>::operator+=(Add<DdType::CUDD> const& other) {
            this->addMetaVariables(other.getContainedMetaVariables());
            this->cuddAdd = this->getCuddAdd() + other.getCuddAdd();
            return *this;
        }
        
        Add<DdType::CUDD> Add<DdType::CUDD>::operator*(Add<DdType::CUDD> const& other) const {
            Add<DdType::CUDD> result(*this);
            result *= other;
            return result;
        }
        
        Add<DdType::CUDD>& Add<DdType::CUDD>::operator*=(Add<DdType::CUDD> const& other) {
            this->addMetaVariables(other.getContainedMetaVariables());
            this->cuddAdd = this->getCuddAdd() * other.getCuddAdd();
            return *this;
        }
        
        Add<DdType::CUDD> Add<DdType::CUDD>::operator-(Add<DdType::CUDD> const& other) const {
            Add<DdType::CUDD> result(*this);
            result -= other;
            return result;
        }
        
        Add<DdType::CUDD> Add<DdType::CUDD>::operator-() const {
            return this->getDdManager()->getAddZero() - *this;
        }
        
        Add<DdType::CUDD>& Add<DdType::CUDD>::operator-=(Add<DdType::CUDD> const& other) {
            this->addMetaVariables(other.getContainedMetaVariables());
            this->cuddAdd = this->getCuddAdd() - other.getCuddAdd();
            return *this;
        }
        
        Add<DdType::CUDD> Add<DdType::CUDD>::operator/(Add<DdType::CUDD> const& other) const {
            Add<DdType::CUDD> result(*this);
            result /= other;
            return result;
        }
        
        Add<DdType::CUDD>& Add<DdType::CUDD>::operator/=(Add<DdType::CUDD> const& other) {
            this->addMetaVariables(other.getContainedMetaVariables());
            this->cuddAdd = this->getCuddAdd().Divide(other.getCuddAdd());
            return *this;
        }
        
        Add<DdType::CUDD> Add<DdType::CUDD>::equals(Add<DdType::CUDD> const& other) const {
            std::set<storm::expressions::Variable> metaVariables;
            std::set_union(this->getContainedMetaVariables().begin(), this->getContainedMetaVariables().end(), other.getContainedMetaVariables().begin(), other.getContainedMetaVariables().end(), std::inserter(metaVariables, metaVariables.begin()));
            return Add<DdType::CUDD>(this->getDdManager(), this->getCuddAdd().Equals(other.getCuddAdd()), metaVariables);
        }
        
        Add<DdType::CUDD> Add<DdType::CUDD>::notEquals(Add<DdType::CUDD> const& other) const {
            std::set<storm::expressions::Variable> metaVariables;
            std::set_union(this->getContainedMetaVariables().begin(), this->getContainedMetaVariables().end(), other.getContainedMetaVariables().begin(), other.getContainedMetaVariables().end(), std::inserter(metaVariables, metaVariables.begin()));
            return Add<DdType::CUDD>(this->getDdManager(), this->getCuddAdd().NotEquals(other.getCuddAdd()), metaVariables);
        }
        
        Add<DdType::CUDD> Add<DdType::CUDD>::less(Add<DdType::CUDD> const& other) const {
            std::set<storm::expressions::Variable> metaVariables;
            std::set_union(this->getContainedMetaVariables().begin(), this->getContainedMetaVariables().end(), other.getContainedMetaVariables().begin(), other.getContainedMetaVariables().end(), std::inserter(metaVariables, metaVariables.begin()));
            return Add<DdType::CUDD>(this->getDdManager(), this->getCuddAdd().LessThan(other.getCuddAdd()), metaVariables);
        }
        
        Add<DdType::CUDD> Add<DdType::CUDD>::lessOrEqual(Add<DdType::CUDD> const& other) const {
            std::set<storm::expressions::Variable> metaVariables;
            std::set_union(this->getContainedMetaVariables().begin(), this->getContainedMetaVariables().end(), other.getContainedMetaVariables().begin(), other.getContainedMetaVariables().end(), std::inserter(metaVariables, metaVariables.begin()));
            return Add<DdType::CUDD>(this->getDdManager(), this->getCuddAdd().LessThanOrEqual(other.getCuddAdd()), metaVariables);
        }
        
        Add<DdType::CUDD> Add<DdType::CUDD>::greater(Add<DdType::CUDD> const& other) const {
            std::set<storm::expressions::Variable> metaVariables;
            std::set_union(this->getContainedMetaVariables().begin(), this->getContainedMetaVariables().end(), other.getContainedMetaVariables().begin(), other.getContainedMetaVariables().end(), std::inserter(metaVariables, metaVariables.begin()));
            return Add<DdType::CUDD>(this->getDdManager(), this->getCuddAdd().GreaterThan(other.getCuddAdd()), metaVariables);
        }
        
        Add<DdType::CUDD> Add<DdType::CUDD>::greaterOrEqual(Add<DdType::CUDD> const& other) const {
            std::set<storm::expressions::Variable> metaVariables;
            std::set_union(this->getContainedMetaVariables().begin(), this->getContainedMetaVariables().end(), other.getContainedMetaVariables().begin(), other.getContainedMetaVariables().end(), std::inserter(metaVariables, metaVariables.begin()));
            return Add<DdType::CUDD>(this->getDdManager(), this->getCuddAdd().GreaterThanOrEqual(other.getCuddAdd()), metaVariables);
        }
        
        Add<DdType::CUDD> Add<DdType::CUDD>::pow(Add<DdType::CUDD> const& other) const {
            std::set<storm::expressions::Variable> metaVariables;
            std::set_union(this->getContainedMetaVariables().begin(), this->getContainedMetaVariables().end(), other.getContainedMetaVariables().begin(), other.getContainedMetaVariables().end(), std::inserter(metaVariables, metaVariables.begin()));
            return Add<DdType::CUDD>(this->getDdManager(), this->getCuddAdd().Pow(other.getCuddAdd()), metaVariables);
        }
        
        Add<DdType::CUDD> Add<DdType::CUDD>::mod(Add<DdType::CUDD> const& other) const {
            std::set<storm::expressions::Variable> metaVariables;
            std::set_union(this->getContainedMetaVariables().begin(), this->getContainedMetaVariables().end(), other.getContainedMetaVariables().begin(), other.getContainedMetaVariables().end(), std::inserter(metaVariables, metaVariables.begin()));
            return Add<DdType::CUDD>(this->getDdManager(), this->getCuddAdd().Mod(other.getCuddAdd()), metaVariables);
        }
        
        Add<DdType::CUDD> Add<DdType::CUDD>::logxy(Add<DdType::CUDD> const& other) const {
            std::set<storm::expressions::Variable> metaVariables;
            std::set_union(this->getContainedMetaVariables().begin(), this->getContainedMetaVariables().end(), other.getContainedMetaVariables().begin(), other.getContainedMetaVariables().end(), std::inserter(metaVariables, metaVariables.begin()));
            return Add<DdType::CUDD>(this->getDdManager(), this->getCuddAdd().LogXY(other.getCuddAdd()), metaVariables);
        }
        
        Add<DdType::CUDD> Add<DdType::CUDD>::floor() const {
            return Add<DdType::CUDD>(this->getDdManager(), this->getCuddAdd().Floor(), this->getContainedMetaVariables());
        }
        
        Add<DdType::CUDD> Add<DdType::CUDD>::ceil() const {
            return Add<DdType::CUDD>(this->getDdManager(), this->getCuddAdd().Ceil(), this->getContainedMetaVariables());
        }
        
        Add<DdType::CUDD> Add<DdType::CUDD>::minimum(Add<DdType::CUDD> const& other) const {
            std::set<storm::expressions::Variable> metaVariables;
            std::set_union(this->getContainedMetaVariables().begin(), this->getContainedMetaVariables().end(), other.getContainedMetaVariables().begin(), other.getContainedMetaVariables().end(), std::inserter(metaVariables, metaVariables.begin()));
            return Add<DdType::CUDD>(this->getDdManager(), this->getCuddAdd().Minimum(other.getCuddAdd()), metaVariables);
        }
        
        Add<DdType::CUDD> Add<DdType::CUDD>::maximum(Add<DdType::CUDD> const& other) const {
            std::set<storm::expressions::Variable> metaVariables;
            std::set_union(this->getContainedMetaVariables().begin(), this->getContainedMetaVariables().end(), other.getContainedMetaVariables().begin(), other.getContainedMetaVariables().end(), std::inserter(metaVariables, metaVariables.begin()));
            return Add<DdType::CUDD>(this->getDdManager(), this->getCuddAdd().Maximum(other.getCuddAdd()), metaVariables);
        }
        
        Add<DdType::CUDD> Add<DdType::CUDD>::sumAbstract(std::set<storm::expressions::Variable> const& metaVariables) const {
            Bdd<DdType::CUDD> cubeDd = this->getDdManager()->getBddOne();
            std::set<storm::expressions::Variable> newMetaVariables = this->getContainedMetaVariables();
            for (auto const& metaVariable : metaVariables) {
                // First check whether the DD contains the meta variable and erase it, if this is the case.
                STORM_LOG_THROW(this->containsMetaVariable(metaVariable), storm::exceptions::InvalidArgumentException, "Cannot abstract from meta variable '" << metaVariable.getName() << "' that is not present in the DD.");
                newMetaVariables.erase(metaVariable);
                
                DdMetaVariable<DdType::CUDD> const& ddMetaVariable = this->getDdManager()->getMetaVariable(metaVariable);
                cubeDd &= ddMetaVariable.getCube();
            }
            
            return Add<DdType::CUDD>(this->getDdManager(), this->getCuddAdd().ExistAbstract(cubeDd.toAdd().getCuddAdd()), newMetaVariables);
        }
        
        Add<DdType::CUDD> Add<DdType::CUDD>::minAbstract(std::set<storm::expressions::Variable> const& metaVariables) const {
            Bdd<DdType::CUDD> cubeDd = this->getDdManager()->getBddOne();
            std::set<storm::expressions::Variable> newMetaVariables = this->getContainedMetaVariables();
            for (auto const& metaVariable : metaVariables) {
                // First check whether the DD contains the meta variable and erase it, if this is the case.
                STORM_LOG_THROW(this->containsMetaVariable(metaVariable), storm::exceptions::InvalidArgumentException, "Cannot abstract from meta variable '" << metaVariable.getName() << "' that is not present in the DD.");
                newMetaVariables.erase(metaVariable);
                
                DdMetaVariable<DdType::CUDD> const& ddMetaVariable = this->getDdManager()->getMetaVariable(metaVariable);
                cubeDd &= ddMetaVariable.getCube();
            }
            
            return Add<DdType::CUDD>(this->getDdManager(), this->getCuddAdd().MinAbstract(cubeDd.toAdd().getCuddAdd()), newMetaVariables);
        }
        
        Add<DdType::CUDD> Add<DdType::CUDD>::maxAbstract(std::set<storm::expressions::Variable> const& metaVariables) const {
            Bdd<DdType::CUDD> cubeDd = this->getDdManager()->getBddOne();
            std::set<storm::expressions::Variable> newMetaVariables = this->getContainedMetaVariables();
            for (auto const& metaVariable : metaVariables) {
                // First check whether the DD contains the meta variable and erase it, if this is the case.
                STORM_LOG_THROW(this->containsMetaVariable(metaVariable), storm::exceptions::InvalidArgumentException, "Cannot abstract from meta variable '" << metaVariable.getName() << "' that is not present in the DD.");
                newMetaVariables.erase(metaVariable);
                
                DdMetaVariable<DdType::CUDD> const& ddMetaVariable = this->getDdManager()->getMetaVariable(metaVariable);
                cubeDd &= ddMetaVariable.getCube();
            }
            
            return Add<DdType::CUDD>(this->getDdManager(), this->getCuddAdd().MaxAbstract(cubeDd.toAdd().getCuddAdd()), newMetaVariables);
        }
        
        bool Add<DdType::CUDD>::equalModuloPrecision(Add<DdType::CUDD> const& other, double precision, bool relative) const {
            if (relative) {
                return this->getCuddAdd().EqualSupNormRel(other.getCuddAdd(), precision);
            } else {
                return this->getCuddAdd().EqualSupNorm(other.getCuddAdd(), precision);
            }
        }
        
        Add<DdType::CUDD> Add<DdType::CUDD>::swapVariables(std::vector<std::pair<storm::expressions::Variable, storm::expressions::Variable>> const& metaVariablePairs) const {
            std::set<storm::expressions::Variable> newContainedMetaVariables;
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
                if (this->containsMetaVariable(metaVariablePair.first)) {
                    newContainedMetaVariables.insert(metaVariablePair.second);
                }
                if (this->containsMetaVariable(metaVariablePair.second)) {
                    newContainedMetaVariables.insert(metaVariablePair.first);
                }
                
                // Add the variables to swap to the corresponding vectors.
                for (auto const& ddVariable : variable1.getDdVariables()) {
                    from.push_back(ddVariable.toAdd().getCuddAdd());
                }
                for (auto const& ddVariable : variable2.getDdVariables()) {
                    to.push_back(ddVariable.toAdd().getCuddAdd());
                }
            }
            
            // Finally, call CUDD to swap the variables.
            return Add<DdType::CUDD>(this->getDdManager(), this->getCuddAdd().SwapVariables(from, to), newContainedMetaVariables);
        }
        
        Add<DdType::CUDD> Add<DdType::CUDD>::multiplyMatrix(Add<DdType::CUDD> const& otherMatrix, std::set<storm::expressions::Variable> const& summationMetaVariables) const {
            // Create the CUDD summation variables.
            std::vector<ADD> summationDdVariables;
            for (auto const& metaVariable : summationMetaVariables) {
                for (auto const& ddVariable : this->getDdManager()->getMetaVariable(metaVariable).getDdVariables()) {
                    summationDdVariables.push_back(ddVariable.toAdd().getCuddAdd());
                }
            }
            
            std::set<storm::expressions::Variable> unionOfMetaVariables;
            std::set_union(this->getContainedMetaVariables().begin(), this->getContainedMetaVariables().end(), otherMatrix.getContainedMetaVariables().begin(), otherMatrix.getContainedMetaVariables().end(), std::inserter(unionOfMetaVariables, unionOfMetaVariables.begin()));
            std::set<storm::expressions::Variable> containedMetaVariables;
            std::set_difference(unionOfMetaVariables.begin(), unionOfMetaVariables.end(), summationMetaVariables.begin(), summationMetaVariables.end(), std::inserter(containedMetaVariables, containedMetaVariables.begin()));
            
            return Add<DdType::CUDD>(this->getDdManager(), this->getCuddAdd().MatrixMultiply(otherMatrix.getCuddAdd(), summationDdVariables), containedMetaVariables);
        }
        
        Bdd<DdType::CUDD> Add<DdType::CUDD>::greater(double value) const {
            return Bdd<DdType::CUDD>(this->getDdManager(), this->getCuddAdd().BddStrictThreshold(value), this->getContainedMetaVariables());
        }
        
        Bdd<DdType::CUDD> Add<DdType::CUDD>::greaterOrEqual(double value) const {
            return Bdd<DdType::CUDD>(this->getDdManager(), this->getCuddAdd().BddThreshold(value), this->getContainedMetaVariables());
        }
        
        Bdd<DdType::CUDD> Add<DdType::CUDD>::less(double value) const {
            return Bdd<DdType::CUDD>(this->getDdManager(), ~this->getCuddAdd().BddThreshold(value), this->getContainedMetaVariables());
        }
        
        Bdd<DdType::CUDD> Add<DdType::CUDD>::lessOrEqual(double value) const {
            return Bdd<DdType::CUDD>(this->getDdManager(), ~this->getCuddAdd().BddStrictThreshold(value), this->getContainedMetaVariables());
        }
        
        Bdd<DdType::CUDD> Add<DdType::CUDD>::notZero() const {
            return this->toBdd();
        }
        
        Add<DdType::CUDD> Add<DdType::CUDD>::constrain(Add<DdType::CUDD> const& constraint) const {
            std::set<storm::expressions::Variable> metaVariables;
            std::set_union(this->getContainedMetaVariables().begin(), this->getContainedMetaVariables().end(), constraint.getContainedMetaVariables().begin(), constraint.getContainedMetaVariables().end(), std::inserter(metaVariables, metaVariables.begin()));
            return Add<DdType::CUDD>(this->getDdManager(), this->getCuddAdd().Constrain(constraint.getCuddAdd()), metaVariables);
        }
        
        Add<DdType::CUDD> Add<DdType::CUDD>::restrict(Add<DdType::CUDD> const& constraint) const {
            std::set<storm::expressions::Variable> metaVariables;
            std::set_union(this->getContainedMetaVariables().begin(), this->getContainedMetaVariables().end(), constraint.getContainedMetaVariables().begin(), constraint.getContainedMetaVariables().end(), std::inserter(metaVariables, metaVariables.begin()));
            return Add<DdType::CUDD>(this->getDdManager(), this->getCuddAdd().Restrict(constraint.getCuddAdd()), metaVariables);
        }
        
        Bdd<DdType::CUDD> Add<DdType::CUDD>::getSupport() const {
            return Bdd<DdType::CUDD>(this->getDdManager(), this->getCuddAdd().Support(), this->getContainedMetaVariables());
        }
        
        uint_fast64_t Add<DdType::CUDD>::getNonZeroCount() const {
            std::size_t numberOfDdVariables = 0;
            for (auto const& metaVariable : this->getContainedMetaVariables()) {
                numberOfDdVariables += this->getDdManager()->getMetaVariable(metaVariable).getNumberOfDdVariables();
            }
            return static_cast<uint_fast64_t>(this->getCuddAdd().CountMinterm(static_cast<int>(numberOfDdVariables)));
        }
        
        uint_fast64_t Add<DdType::CUDD>::getLeafCount() const {
            return static_cast<uint_fast64_t>(this->getCuddAdd().CountLeaves());
        }
        
        uint_fast64_t Add<DdType::CUDD>::getNodeCount() const {
            return static_cast<uint_fast64_t>(this->getCuddAdd().nodeCount());
        }
        
        double Add<DdType::CUDD>::getMin() const {
            ADD constantMinAdd = this->getCuddAdd().FindMin();
            return static_cast<double>(Cudd_V(constantMinAdd.getNode()));
        }
        
        double Add<DdType::CUDD>::getMax() const {
            ADD constantMaxAdd = this->getCuddAdd().FindMax();
            return static_cast<double>(Cudd_V(constantMaxAdd.getNode()));
        }
        
        void Add<DdType::CUDD>::setValue(storm::expressions::Variable const& metaVariable, int_fast64_t variableValue, double targetValue) {
            std::map<storm::expressions::Variable, int_fast64_t> metaVariableToValueMap;
            metaVariableToValueMap.emplace(metaVariable, variableValue);
            this->setValue(metaVariableToValueMap, targetValue);
        }
        
        void Add<DdType::CUDD>::setValue(storm::expressions::Variable const& metaVariable1, int_fast64_t variableValue1, storm::expressions::Variable const& metaVariable2, int_fast64_t variableValue2, double targetValue) {
            std::map<storm::expressions::Variable, int_fast64_t> metaVariableToValueMap;
            metaVariableToValueMap.emplace(metaVariable1, variableValue1);
            metaVariableToValueMap.emplace(metaVariable2, variableValue2);
            this->setValue(metaVariableToValueMap, targetValue);
        }
        
        void Add<DdType::CUDD>::setValue(std::map<storm::expressions::Variable, int_fast64_t> const& metaVariableToValueMap, double targetValue) {
            Bdd<DdType::CUDD> valueEncoding = this->getDdManager()->getBddOne();
            for (auto const& nameValuePair : metaVariableToValueMap) {
                valueEncoding &= this->getDdManager()->getEncoding(nameValuePair.first, nameValuePair.second);
                // Also record that the DD now contains the meta variable.
                this->addMetaVariable(nameValuePair.first);
            }
            
            this->cuddAdd = valueEncoding.toAdd().getCuddAdd().Ite(this->getDdManager()->getConstant(targetValue).getCuddAdd(), this->getCuddAdd());
        }
        
        double Add<DdType::CUDD>::getValue(std::map<storm::expressions::Variable, int_fast64_t> const& metaVariableToValueMap) const {
            std::set<storm::expressions::Variable> remainingMetaVariables(this->getContainedMetaVariables());
            Bdd<DdType::CUDD> valueEncoding = this->getDdManager()->getBddOne();
            for (auto const& nameValuePair : metaVariableToValueMap) {
                valueEncoding &= this->getDdManager()->getEncoding(nameValuePair.first, nameValuePair.second);
                if (this->containsMetaVariable(nameValuePair.first)) {
                    remainingMetaVariables.erase(nameValuePair.first);
                }
            }
            
            STORM_LOG_THROW(remainingMetaVariables.empty(), storm::exceptions::InvalidArgumentException, "Cannot evaluate function for which not all inputs were given.");
            
            Add<DdType::CUDD> value = *this * valueEncoding.toAdd();
            value = value.sumAbstract(this->getContainedMetaVariables());
            return static_cast<double>(Cudd_V(value.getCuddAdd().getNode()));
        }
        
        bool Add<DdType::CUDD>::isOne() const {
            return this->getCuddAdd().IsOne();
        }
        
        bool Add<DdType::CUDD>::isZero() const {
            return this->getCuddAdd().IsZero();
        }
        
        bool Add<DdType::CUDD>::isConstant() const {
            return Cudd_IsConstant(this->getCuddAdd().getNode());
        }
        
        uint_fast64_t Add<DdType::CUDD>::getIndex() const {
            return static_cast<uint_fast64_t>(this->getCuddAdd().NodeReadIndex());
        }
        
        uint_fast64_t Add<DdType::CUDD>::getLevel() const {
            return static_cast<uint_fast64_t>(this->getDdManager()->getCuddManager().ReadPerm(this->getIndex()));
        }
        
        template<typename ValueType>
        std::vector<ValueType> Add<DdType::CUDD>::toVector() const {
            return this->toVector<ValueType>(Odd<DdType::CUDD>(*this));
        }
        
        template<typename ValueType>
        std::vector<ValueType> Add<DdType::CUDD>::toVector(Odd<DdType::CUDD> const& rowOdd) const {
            std::vector<ValueType> result(rowOdd.getTotalOffset());
            std::vector<uint_fast64_t> ddVariableIndices = this->getSortedVariableIndices();
            addToVector(rowOdd, ddVariableIndices, result);
            return result;
        }
        
        template<typename ValueType>
        std::vector<ValueType> Add<DdType::CUDD>::toVector(std::set<storm::expressions::Variable> const& groupMetaVariables, storm::dd::Odd<DdType::CUDD> const& rowOdd, std::vector<uint_fast64_t> groupOffsets) const {
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
                DdMetaVariable<DdType::CUDD> const& metaVariable = this->getDdManager()->getMetaVariable(variable);
                for (auto const& ddVariable : metaVariable.getDdVariables()) {
                    ddGroupVariableIndices.push_back(ddVariable.getIndex());
                }
            }
            std::vector<uint_fast64_t> ddRowVariableIndices;
            for (auto const& variable : rowMetaVariables) {
                DdMetaVariable<DdType::CUDD> const& metaVariable = this->getDdManager()->getMetaVariable(variable);
                for (auto const& ddVariable : metaVariable.getDdVariables()) {
                    ddRowVariableIndices.push_back(ddVariable.getIndex());
                }
            }
            
            // Start by splitting the symbolic vector into groups.
            std::vector<Add<DdType::CUDD>> groups;
            splitGroupsRec(this->getCuddDdNode(), groups, ddGroupVariableIndices, 0, ddGroupVariableIndices.size(), rowMetaVariables);
            
            // Now iterate over the groups and add them to the resulting vector.
            std::vector<ValueType> result(groupOffsets.back(), storm::utility::zero<ValueType>());
            for (uint_fast64_t i = 0; i < groups.size(); ++i) {
                toVectorRec(groups[i].getCuddDdNode(), result, groupOffsets, rowOdd, 0, ddRowVariableIndices.size(), 0, ddRowVariableIndices);
            }
            
            return result;
        }
        
        storm::storage::SparseMatrix<double> Add<DdType::CUDD>::toMatrix() const {
            std::set<storm::expressions::Variable> rowVariables;
            std::set<storm::expressions::Variable> columnVariables;
            
            for (auto const& variable : this->getContainedMetaVariables()) {
                if (variable.getName().size() > 0 && variable.getName().back() == '\'') {
                    columnVariables.insert(variable);
                } else {
                    rowVariables.insert(variable);
                }
            }
            
            return toMatrix(rowVariables, columnVariables, Odd<DdType::CUDD>(this->sumAbstract(rowVariables)), Odd<DdType::CUDD>(this->sumAbstract(columnVariables)));
        }
        
        storm::storage::SparseMatrix<double> Add<DdType::CUDD>::toMatrix(storm::dd::Odd<DdType::CUDD> const& rowOdd, storm::dd::Odd<DdType::CUDD> const& columnOdd) const {
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
        
        storm::storage::SparseMatrix<double> Add<DdType::CUDD>::toMatrix(std::set<storm::expressions::Variable> const& rowMetaVariables, std::set<storm::expressions::Variable> const& columnMetaVariables, storm::dd::Odd<DdType::CUDD> const& rowOdd, storm::dd::Odd<DdType::CUDD> const& columnOdd) const {
            std::vector<uint_fast64_t> ddRowVariableIndices;
            std::vector<uint_fast64_t> ddColumnVariableIndices;
            
            for (auto const& variable : rowMetaVariables) {
                DdMetaVariable<DdType::CUDD> const& metaVariable = this->getDdManager()->getMetaVariable(variable);
                for (auto const& ddVariable : metaVariable.getDdVariables()) {
                    ddRowVariableIndices.push_back(ddVariable.getIndex());
                }
            }
            std::sort(ddRowVariableIndices.begin(), ddRowVariableIndices.end());
            
            for (auto const& variable : columnMetaVariables) {
                DdMetaVariable<DdType::CUDD> const& metaVariable = this->getDdManager()->getMetaVariable(variable);
                for (auto const& ddVariable : metaVariable.getDdVariables()) {
                    ddColumnVariableIndices.push_back(ddVariable.getIndex());
                }
            }
            std::sort(ddColumnVariableIndices.begin(), ddColumnVariableIndices.end());
            
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
            toMatrixRec(this->getCuddDdNode(), rowIndications, columnsAndValues, trivialRowGroupIndices, rowOdd, columnOdd, 0, 0, ddRowVariableIndices.size() + ddColumnVariableIndices.size(), 0, 0, ddRowVariableIndices, ddColumnVariableIndices, false);
            
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
            toMatrixRec(this->getCuddDdNode(), rowIndications, columnsAndValues, trivialRowGroupIndices, rowOdd, columnOdd, 0, 0, ddRowVariableIndices.size() + ddColumnVariableIndices.size(), 0, 0, ddRowVariableIndices, ddColumnVariableIndices, true);
            
            // Since the last call to toMatrixRec modified the rowIndications, we need to restore the correct values.
            for (uint_fast64_t i = rowIndications.size() - 1; i > 0; --i) {
                rowIndications[i] = rowIndications[i - 1];
            }
            rowIndications[0] = 0;
            
            // Construct matrix and return result.
            return storm::storage::SparseMatrix<double>(columnOdd.getTotalOffset(), std::move(rowIndications), std::move(columnsAndValues), std::move(trivialRowGroupIndices), false);
        }
        
        storm::storage::SparseMatrix<double> Add<DdType::CUDD>::toMatrix(std::set<storm::expressions::Variable> const& groupMetaVariables, storm::dd::Odd<DdType::CUDD> const& rowOdd, storm::dd::Odd<DdType::CUDD> const& columnOdd) const {
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
        
        storm::storage::SparseMatrix<double> Add<DdType::CUDD>::toMatrix(std::set<storm::expressions::Variable> const& rowMetaVariables, std::set<storm::expressions::Variable> const& columnMetaVariables, std::set<storm::expressions::Variable> const& groupMetaVariables, storm::dd::Odd<DdType::CUDD> const& rowOdd, storm::dd::Odd<DdType::CUDD> const& columnOdd) const {
            std::vector<uint_fast64_t> ddRowVariableIndices;
            std::vector<uint_fast64_t> ddColumnVariableIndices;
            std::vector<uint_fast64_t> ddGroupVariableIndices;
            std::set<storm::expressions::Variable> rowAndColumnMetaVariables;
            boost::optional<std::vector<double>> optionalExplicitVector;
            
            for (auto const& variable : rowMetaVariables) {
                DdMetaVariable<DdType::CUDD> const& metaVariable = this->getDdManager()->getMetaVariable(variable);
                for (auto const& ddVariable : metaVariable.getDdVariables()) {
                    ddRowVariableIndices.push_back(ddVariable.getIndex());
                }
                rowAndColumnMetaVariables.insert(variable);
            }
            std::sort(ddRowVariableIndices.begin(), ddRowVariableIndices.end());
            for (auto const& variable : columnMetaVariables) {
                DdMetaVariable<DdType::CUDD> const& metaVariable = this->getDdManager()->getMetaVariable(variable);
                for (auto const& ddVariable : metaVariable.getDdVariables()) {
                    ddColumnVariableIndices.push_back(ddVariable.getIndex());
                }
                rowAndColumnMetaVariables.insert(variable);
            }
            std::sort(ddColumnVariableIndices.begin(), ddColumnVariableIndices.end());
            for (auto const& variable : groupMetaVariables) {
                DdMetaVariable<DdType::CUDD> const& metaVariable = this->getDdManager()->getMetaVariable(variable);
                for (auto const& ddVariable : metaVariable.getDdVariables()) {
                    ddGroupVariableIndices.push_back(ddVariable.getIndex());
                }
            }
            std::sort(ddGroupVariableIndices.begin(), ddGroupVariableIndices.end());
            
            // Start by computing the offsets (in terms of rows) for each row group.
            Add<DdType::CUDD> stateToNumberOfChoices = this->notZero().existsAbstract(columnMetaVariables).toAdd().sumAbstract(groupMetaVariables);
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
            std::vector<Add<DdType::CUDD>> groups;
            splitGroupsRec(this->getCuddDdNode(), groups, ddGroupVariableIndices, 0, ddGroupVariableIndices.size(), rowAndColumnMetaVariables);
            
            // Create the actual storage for the non-zero entries.
            std::vector<storm::storage::MatrixEntry<uint_fast64_t, double>> columnsAndValues(this->getNonZeroCount());
            
            // Now compute the indices at which the individual rows start.
            std::vector<uint_fast64_t> rowIndications(rowGroupIndices.back() + 1);
            
            std::vector<storm::dd::Add<DdType::CUDD>> statesWithGroupEnabled(groups.size());
            storm::dd::Add<storm::dd::DdType::CUDD> stateToRowGroupCount = this->getDdManager()->getAddZero();
            for (uint_fast64_t i = 0; i < groups.size(); ++i) {
                auto const& dd = groups[i];
                
                toMatrixRec(dd.getCuddDdNode(), rowIndications, columnsAndValues, rowGroupIndices, rowOdd, columnOdd, 0, 0, ddRowVariableIndices.size() + ddColumnVariableIndices.size(), 0, 0, ddRowVariableIndices, ddColumnVariableIndices, false);
                
                statesWithGroupEnabled[i] = dd.notZero().existsAbstract(columnMetaVariables).toAdd();
                stateToRowGroupCount += statesWithGroupEnabled[i];
                statesWithGroupEnabled[i].addToVector(rowOdd, ddRowVariableIndices, rowGroupIndices);
            }
            
            // Since we modified the rowGroupIndices, we need to restore the correct values.
            std::function<uint_fast64_t (uint_fast64_t const&, double const&)> fct = [] (uint_fast64_t const& a, double const& b) -> uint_fast64_t { return a - static_cast<uint_fast64_t>(b); };
            modifyVectorRec(stateToRowGroupCount.getCuddDdNode(), 0, ddRowVariableIndices.size(), 0, rowOdd, ddRowVariableIndices, rowGroupIndices, fct);
            
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
                
                toMatrixRec(dd.getCuddDdNode(), rowIndications, columnsAndValues, rowGroupIndices, rowOdd, columnOdd, 0, 0, ddRowVariableIndices.size() + ddColumnVariableIndices.size(), 0, 0, ddRowVariableIndices, ddColumnVariableIndices, true);
                
                statesWithGroupEnabled[i].addToVector(rowOdd, ddRowVariableIndices, rowGroupIndices);
            }
            
            // Since we modified the rowGroupIndices, we need to restore the correct values.
            modifyVectorRec(stateToRowGroupCount.getCuddDdNode(), 0, ddRowVariableIndices.size(), 0, rowOdd, ddRowVariableIndices, rowGroupIndices, fct);
            
            // Since the last call to toMatrixRec modified the rowIndications, we need to restore the correct values.
            for (uint_fast64_t i = rowIndications.size() - 1; i > 0; --i) {
                rowIndications[i] = rowIndications[i - 1];
            }
            rowIndications[0] = 0;
            
            return storm::storage::SparseMatrix<double>(columnOdd.getTotalOffset(), std::move(rowIndications), std::move(columnsAndValues), std::move(rowGroupIndices), true);
        }
        
        std::pair<storm::storage::SparseMatrix<double>, std::vector<double>> Add<DdType::CUDD>::toMatrixVector(storm::dd::Add<storm::dd::DdType::CUDD> const& vector, std::vector<uint_fast64_t>&& rowGroupSizes, std::set<storm::expressions::Variable> const& groupMetaVariables, storm::dd::Odd<DdType::CUDD> const& rowOdd, storm::dd::Odd<DdType::CUDD> const& columnOdd) const {
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
        
        std::pair<storm::storage::SparseMatrix<double>,std::vector<double>> Add<DdType::CUDD>::toMatrixVector(storm::dd::Add<storm::dd::DdType::CUDD> const& vector, std::vector<uint_fast64_t>&& rowGroupIndices, std::set<storm::expressions::Variable> const& rowMetaVariables, std::set<storm::expressions::Variable> const& columnMetaVariables, std::set<storm::expressions::Variable> const& groupMetaVariables, storm::dd::Odd<DdType::CUDD> const& rowOdd, storm::dd::Odd<DdType::CUDD> const& columnOdd) const {
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
            std::sort(ddRowVariableIndices.begin(), ddRowVariableIndices.end());
            for (auto const& variable : columnMetaVariables) {
                DdMetaVariable<DdType::CUDD> const& metaVariable = this->getDdManager()->getMetaVariable(variable);
                for (auto const& ddVariable : metaVariable.getDdVariables()) {
                    ddColumnVariableIndices.push_back(ddVariable.getIndex());
                }
                rowAndColumnMetaVariables.insert(variable);
            }
            std::sort(ddColumnVariableIndices.begin(), ddColumnVariableIndices.end());
            for (auto const& variable : groupMetaVariables) {
                DdMetaVariable<DdType::CUDD> const& metaVariable = this->getDdManager()->getMetaVariable(variable);
                for (auto const& ddVariable : metaVariable.getDdVariables()) {
                    ddGroupVariableIndices.push_back(ddVariable.getIndex());
                }
            }
            std::sort(ddGroupVariableIndices.begin(), ddGroupVariableIndices.end());
            
            // Transform the row group sizes to the actual row group indices.
            rowGroupIndices.resize(rowGroupIndices.size() + 1);
            uint_fast64_t tmp = 0;
            uint_fast64_t tmp2 = 0;
            for (uint_fast64_t i = 1; i < rowGroupIndices.size(); ++i) {
                tmp2 = rowGroupIndices[i];
                rowGroupIndices[i] = rowGroupIndices[i - 1] + tmp;
                std::swap(tmp, tmp2);
            }
            rowGroupIndices[0] = 0;
            
            // Create the explicit vector we need to fill later.
            std::vector<double> explicitVector(rowGroupIndices.back());
            
            // Next, we split the matrix into one for each group. This only works if the group variables are at the very top.
            std::vector<std::pair<Add<DdType::CUDD>, Add<DdType::CUDD>>> groups;
            splitGroupsRec(this->getCuddDdNode(), vector.getCuddDdNode(), groups, ddGroupVariableIndices, 0, ddGroupVariableIndices.size(), rowAndColumnMetaVariables, rowMetaVariables);
            
            // Create the actual storage for the non-zero entries.
            std::vector<storm::storage::MatrixEntry<uint_fast64_t, double>> columnsAndValues(this->getNonZeroCount());
            
            // Now compute the indices at which the individual rows start.
            std::vector<uint_fast64_t> rowIndications(rowGroupIndices.back() + 1);

            std::vector<storm::dd::Add<DdType::CUDD>> statesWithGroupEnabled(groups.size());
            storm::dd::Add<storm::dd::DdType::CUDD> stateToRowGroupCount = this->getDdManager()->getAddZero();
            for (uint_fast64_t i = 0; i < groups.size(); ++i) {
                std::pair<storm::dd::Add<DdType::CUDD>, storm::dd::Add<DdType::CUDD>> ddPair = groups[i];
                
                toMatrixRec(ddPair.first.getCuddDdNode(), rowIndications, columnsAndValues, rowGroupIndices, rowOdd, columnOdd, 0, 0, ddRowVariableIndices.size() + ddColumnVariableIndices.size(), 0, 0, ddRowVariableIndices, ddColumnVariableIndices, false);
                toVectorRec(ddPair.second.getCuddDdNode(), explicitVector, rowGroupIndices, rowOdd, 0, ddRowVariableIndices.size(), 0, ddRowVariableIndices);
                
                statesWithGroupEnabled[i] = (ddPair.first.notZero().existsAbstract(columnMetaVariables) || ddPair.second.notZero()).toAdd();
                stateToRowGroupCount += statesWithGroupEnabled[i];
                statesWithGroupEnabled[i].addToVector(rowOdd, ddRowVariableIndices, rowGroupIndices);
            }
            
            // Since we modified the rowGroupIndices, we need to restore the correct values.
            std::function<uint_fast64_t (uint_fast64_t const&, double const&)> fct = [] (uint_fast64_t const& a, double const& b) -> uint_fast64_t { return a - static_cast<uint_fast64_t>(b); };
            modifyVectorRec(stateToRowGroupCount.getCuddDdNode(), 0, ddRowVariableIndices.size(), 0, rowOdd, ddRowVariableIndices, rowGroupIndices, fct);
            
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
                auto const& dd = groups[i].first;
                
                toMatrixRec(dd.getCuddDdNode(), rowIndications, columnsAndValues, rowGroupIndices, rowOdd, columnOdd, 0, 0, ddRowVariableIndices.size() + ddColumnVariableIndices.size(), 0, 0, ddRowVariableIndices, ddColumnVariableIndices, true);
                
                statesWithGroupEnabled[i].addToVector(rowOdd, ddRowVariableIndices, rowGroupIndices);
            }
            
            // Since we modified the rowGroupIndices, we need to restore the correct values.
            modifyVectorRec(stateToRowGroupCount.getCuddDdNode(), 0, ddRowVariableIndices.size(), 0, rowOdd, ddRowVariableIndices, rowGroupIndices, fct);
            
            // Since the last call to toMatrixRec modified the rowIndications, we need to restore the correct values.
            for (uint_fast64_t i = rowIndications.size() - 1; i > 0; --i) {
                rowIndications[i] = rowIndications[i - 1];
            }
            rowIndications[0] = 0;
            
            return std::make_pair(storm::storage::SparseMatrix<double>(columnOdd.getTotalOffset(), std::move(rowIndications), std::move(columnsAndValues), std::move(rowGroupIndices), true), std::move(explicitVector));
        }
        
        template<typename ValueType>
        void Add<DdType::CUDD>::toVectorRec(DdNode const* dd, std::vector<ValueType>& result, std::vector<uint_fast64_t> const& rowGroupOffsets, Odd<DdType::CUDD> const& rowOdd, uint_fast64_t currentRowLevel, uint_fast64_t maxLevel, uint_fast64_t currentRowOffset, std::vector<uint_fast64_t> const& ddRowVariableIndices) const {
            // For the empty DD, we do not need to add any entries.
            if (dd == Cudd_ReadZero(this->getDdManager()->getCuddManager().getManager())) {
                return;
            }

            // If we are at the maximal level, the value to be set is stored as a constant in the DD.
            if (currentRowLevel == maxLevel) {
                result[rowGroupOffsets[currentRowOffset]] = Cudd_V(dd);
            } else if (ddRowVariableIndices[currentRowLevel] < dd->index) {
                toVectorRec(dd, result, rowGroupOffsets, rowOdd.getElseSuccessor(), currentRowLevel + 1, maxLevel, currentRowOffset, ddRowVariableIndices);
                toVectorRec(dd, result, rowGroupOffsets, rowOdd.getThenSuccessor(), currentRowLevel + 1, maxLevel, currentRowOffset + rowOdd.getElseOffset(), ddRowVariableIndices);
            } else {
                toVectorRec(Cudd_E(dd), result, rowGroupOffsets, rowOdd.getElseSuccessor(), currentRowLevel + 1, maxLevel, currentRowOffset, ddRowVariableIndices);
                toVectorRec(Cudd_T(dd), result, rowGroupOffsets, rowOdd.getThenSuccessor(), currentRowLevel + 1, maxLevel, currentRowOffset + rowOdd.getElseOffset(), ddRowVariableIndices);
            }
        }
        
        void Add<DdType::CUDD>::toMatrixRec(DdNode const* dd, std::vector<uint_fast64_t>& rowIndications, std::vector<storm::storage::MatrixEntry<uint_fast64_t, double>>& columnsAndValues, std::vector<uint_fast64_t> const& rowGroupOffsets, Odd<DdType::CUDD> const& rowOdd, Odd<DdType::CUDD> const& columnOdd, uint_fast64_t currentRowLevel, uint_fast64_t currentColumnLevel, uint_fast64_t maxLevel, uint_fast64_t currentRowOffset, uint_fast64_t currentColumnOffset, std::vector<uint_fast64_t> const& ddRowVariableIndices, std::vector<uint_fast64_t> const& ddColumnVariableIndices, bool generateValues) const {
            // For the empty DD, we do not need to add any entries.
            if (dd == Cudd_ReadZero(this->getDdManager()->getCuddManager().getManager())) {
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
        
        void Add<DdType::CUDD>::splitGroupsRec(DdNode* dd, std::vector<Add<DdType::CUDD>>& groups, std::vector<uint_fast64_t> const& ddGroupVariableIndices, uint_fast64_t currentLevel, uint_fast64_t maxLevel, std::set<storm::expressions::Variable> const& remainingMetaVariables) const {
            // For the empty DD, we do not need to create a group.
            if (dd == Cudd_ReadZero(this->getDdManager()->getCuddManager().getManager())) {
                return;
            }
            
            if (currentLevel == maxLevel) {
                groups.push_back(Add<DdType::CUDD>(this->getDdManager(), ADD(this->getDdManager()->getCuddManager(), dd), remainingMetaVariables));
            } else if (ddGroupVariableIndices[currentLevel] < dd->index) {
                splitGroupsRec(dd, groups, ddGroupVariableIndices, currentLevel + 1, maxLevel, remainingMetaVariables);
                splitGroupsRec(dd, groups, ddGroupVariableIndices, currentLevel + 1, maxLevel, remainingMetaVariables);
            } else {
                splitGroupsRec(Cudd_E(dd), groups, ddGroupVariableIndices, currentLevel + 1, maxLevel, remainingMetaVariables);
                splitGroupsRec(Cudd_T(dd), groups, ddGroupVariableIndices, currentLevel + 1, maxLevel, remainingMetaVariables);
            }
        }
        
        void Add<DdType::CUDD>::splitGroupsRec(DdNode* dd1, DdNode* dd2, std::vector<std::pair<Add<DdType::CUDD>, Add<DdType::CUDD>>>& groups, std::vector<uint_fast64_t> const& ddGroupVariableIndices, uint_fast64_t currentLevel, uint_fast64_t maxLevel, std::set<storm::expressions::Variable> const& remainingMetaVariables1, std::set<storm::expressions::Variable> const& remainingMetaVariables2) const {
            // For the empty DD, we do not need to create a group.
            if (dd1 == Cudd_ReadZero(this->getDdManager()->getCuddManager().getManager()) && dd2 == Cudd_ReadZero(this->getDdManager()->getCuddManager().getManager())) {
                return;
            }
            
            if (currentLevel == maxLevel) {
                groups.push_back(std::make_pair(Add<DdType::CUDD>(this->getDdManager(), ADD(this->getDdManager()->getCuddManager(), dd1), remainingMetaVariables1), Add<DdType::CUDD>(this->getDdManager(), ADD(this->getDdManager()->getCuddManager(), dd2), remainingMetaVariables2)));
            } else if (ddGroupVariableIndices[currentLevel] < dd1->index) {
                if (ddGroupVariableIndices[currentLevel] < dd2->index) {
                    splitGroupsRec(dd1, dd2, groups, ddGroupVariableIndices, currentLevel + 1, maxLevel, remainingMetaVariables1, remainingMetaVariables2);
                    splitGroupsRec(dd1, dd2, groups, ddGroupVariableIndices, currentLevel + 1, maxLevel, remainingMetaVariables1, remainingMetaVariables2);
                } else {
                    splitGroupsRec(dd1, Cudd_T(dd2), groups, ddGroupVariableIndices, currentLevel + 1, maxLevel, remainingMetaVariables1, remainingMetaVariables2);
                    splitGroupsRec(dd1, Cudd_E(dd2), groups, ddGroupVariableIndices, currentLevel + 1, maxLevel, remainingMetaVariables1, remainingMetaVariables2);
                }
            } else if (ddGroupVariableIndices[currentLevel] < dd2->index) {
                splitGroupsRec(Cudd_T(dd1), dd2, groups, ddGroupVariableIndices, currentLevel + 1, maxLevel, remainingMetaVariables1, remainingMetaVariables2);
                splitGroupsRec(Cudd_E(dd1), dd2, groups, ddGroupVariableIndices, currentLevel + 1, maxLevel, remainingMetaVariables1, remainingMetaVariables2);
            } else {
                splitGroupsRec(Cudd_T(dd1), Cudd_T(dd2), groups, ddGroupVariableIndices, currentLevel + 1, maxLevel, remainingMetaVariables1, remainingMetaVariables2);
                splitGroupsRec(Cudd_E(dd1), Cudd_E(dd2), groups, ddGroupVariableIndices, currentLevel + 1, maxLevel, remainingMetaVariables1, remainingMetaVariables2);
            }
        }
        
        template<typename ValueType>
        void Add<DdType::CUDD>::addToVector(Odd<DdType::CUDD> const& odd, std::vector<uint_fast64_t> const& ddVariableIndices, std::vector<ValueType>& targetVector) const {
            std::function<ValueType (ValueType const&, double const&)> fct = [] (ValueType const& a, double const& b) -> ValueType { return a + b; };
            modifyVectorRec(this->getCuddDdNode(), 0, ddVariableIndices.size(), 0, odd, ddVariableIndices, targetVector, fct);
        }
        
        template<typename ValueType>
        void Add<DdType::CUDD>::modifyVectorRec(DdNode const* dd, uint_fast64_t currentLevel, uint_fast64_t maxLevel, uint_fast64_t currentOffset, Odd<DdType::CUDD> const& odd, std::vector<uint_fast64_t> const& ddVariableIndices, std::vector<ValueType>& targetVector, std::function<ValueType (ValueType const&, double const&)> const& function) const {
            // For the empty DD, we do not need to add any entries.
            if (dd == Cudd_ReadZero(this->getDdManager()->getCuddManager().getManager())) {
                return;
            }
            
            // If we are at the maximal level, the value to be set is stored as a constant in the DD.
            if (currentLevel == maxLevel) {
                targetVector[currentOffset] = function(targetVector[currentOffset], Cudd_V(dd));
            } else if (ddVariableIndices[currentLevel] < dd->index) {
                // If we skipped a level, we need to enumerate the explicit entries for the case in which the bit is set
                // and for the one in which it is not set.
                modifyVectorRec(dd, currentLevel + 1, maxLevel, currentOffset, odd.getElseSuccessor(), ddVariableIndices, targetVector, function);
                modifyVectorRec(dd, currentLevel + 1, maxLevel, currentOffset + odd.getElseOffset(), odd.getThenSuccessor(), ddVariableIndices, targetVector, function);
            } else {
                // Otherwise, we simply recursively call the function for both (different) cases.
                modifyVectorRec(Cudd_E(dd), currentLevel + 1, maxLevel, currentOffset, odd.getElseSuccessor(), ddVariableIndices, targetVector, function);
                modifyVectorRec(Cudd_T(dd), currentLevel + 1, maxLevel, currentOffset + odd.getElseOffset(), odd.getThenSuccessor(), ddVariableIndices, targetVector, function);
            }
        }
        
        template<typename ValueType>
        ADD Add<DdType::CUDD>::fromVector(std::shared_ptr<DdManager<DdType::CUDD> const> ddManager, std::vector<ValueType> const& values, Odd<DdType::CUDD> const& odd, std::set<storm::expressions::Variable> const& metaVariables) {
            std::vector<uint_fast64_t> ddVariableIndices = getSortedVariableIndices(*ddManager, metaVariables);
            uint_fast64_t offset = 0;
            return ADD(ddManager->getCuddManager(), fromVectorRec(ddManager->getCuddManager().getManager(), offset, 0, ddVariableIndices.size(), values, odd, ddVariableIndices));
        }
        
        template<typename ValueType>
        DdNode* Add<DdType::CUDD>::fromVectorRec(::DdManager* manager, uint_fast64_t& currentOffset, uint_fast64_t currentLevel, uint_fast64_t maxLevel, std::vector<ValueType> const& values, Odd<DdType::CUDD> const& odd, std::vector<uint_fast64_t> const& ddVariableIndices) {
            if (currentLevel == maxLevel) {
                // If we are in a terminal node of the ODD, we need to check whether the then-offset of the ODD is one
                // (meaning the encoding is a valid one) or zero (meaning the encoding is not valid). Consequently, we
                // need to copy the next value of the vector iff the then-offset is greater than zero.
                if (odd.getThenOffset() > 0) {
                    return Cudd_addConst(manager, values[currentOffset++]);
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
                    elseSuccessor = fromVectorRec(manager, currentOffset, currentLevel + 1, maxLevel, values, odd.getElseSuccessor(), ddVariableIndices);
                } else {
                    elseSuccessor = Cudd_ReadZero(manager);
                }
                Cudd_Ref(elseSuccessor);
                
                // Determine the new then-successor.
                DdNode* thenSuccessor = nullptr;
                if (odd.getThenOffset() > 0) {
                    thenSuccessor = fromVectorRec(manager, currentOffset, currentLevel + 1, maxLevel, values, odd.getThenSuccessor(), ddVariableIndices);
                } else {
                    thenSuccessor = Cudd_ReadZero(manager);
                }
                Cudd_Ref(thenSuccessor);
                
                // Create a node representing ITE(currentVar, thenSuccessor, elseSuccessor);
                DdNode* result = Cudd_addIthVar(manager, static_cast<int>(ddVariableIndices[currentLevel]));
                Cudd_Ref(result);
                DdNode* newResult = Cudd_addIte(manager, result, thenSuccessor, elseSuccessor);
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
        
        void Add<DdType::CUDD>::exportToDot(std::string const& filename) const {
            if (filename.empty()) {
                std::vector<ADD> cuddAddVector = { this->getCuddAdd() };
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
                std::vector<ADD> cuddAddVector = { this->getCuddAdd() };
                this->getDdManager()->getCuddManager().DumpDot(cuddAddVector, &ddVariableNames[0], &ddNames[0], filePointer);
                fclose(filePointer);
                
                // Finally, delete the names.
                for (char* element : ddNames) {
                    delete[] element;
                }
                for (char* element : ddVariableNames) {
                    delete[] element;
                }
            }
        }
        
        DdForwardIterator<DdType::CUDD> Add<DdType::CUDD>::begin(bool enumerateDontCareMetaVariables) const {
            int* cube;
            double value;
            DdGen* generator = this->getCuddAdd().FirstCube(&cube, &value);
            return DdForwardIterator<DdType::CUDD>(this->getDdManager(), generator, cube, value, (Cudd_IsGenEmpty(generator) != 0), &this->getContainedMetaVariables(), enumerateDontCareMetaVariables);
        }
        
        DdForwardIterator<DdType::CUDD> Add<DdType::CUDD>::end(bool enumerateDontCareMetaVariables) const {
            return DdForwardIterator<DdType::CUDD>(this->getDdManager(), nullptr, nullptr, 0, true, nullptr, enumerateDontCareMetaVariables);
        }
        
        std::ostream& operator<<(std::ostream& out, const Add<DdType::CUDD>& add) {
            out << "ADD with " << add.getNonZeroCount() << " nnz, " << add.getNodeCount() << " nodes, " << add.getLeafCount() << " leaves" << std::endl;
            std::vector<std::string> variableNames;
            for (auto const& variable : add.getContainedMetaVariables()) {
                variableNames.push_back(variable.getName());
            }
            out << "contained variables: " << boost::algorithm::join(variableNames, ", ") << std::endl;
            return out;
        }
        
        // Explicitly instantiate some templated functions.
        template std::vector<double> Add<DdType::CUDD>::toVector() const;
        template std::vector<double> Add<DdType::CUDD>::toVector(Odd<DdType::CUDD> const& rowOdd) const;
        template void Add<DdType::CUDD>::addToVector(Odd<DdType::CUDD> const& odd, std::vector<uint_fast64_t> const& ddVariableIndices, std::vector<double>& targetVector) const;
        template void Add<DdType::CUDD>::modifyVectorRec(DdNode const* dd, uint_fast64_t currentLevel, uint_fast64_t maxLevel, uint_fast64_t currentOffset, Odd<DdType::CUDD> const& odd, std::vector<uint_fast64_t> const& ddVariableIndices, std::vector<double>& targetVector, std::function<double (double const&, double const&)> const& function) const;
        template std::vector<double> Add<DdType::CUDD>::toVector(std::set<storm::expressions::Variable> const& groupMetaVariables, storm::dd::Odd<DdType::CUDD> const& rowOdd, std::vector<uint_fast64_t> groupOffsets) const;
        template void Add<DdType::CUDD>::toVectorRec(DdNode const* dd, std::vector<double>& result, std::vector<uint_fast64_t> const& rowGroupOffsets, Odd<DdType::CUDD> const& rowOdd, uint_fast64_t currentRowLevel, uint_fast64_t maxLevel, uint_fast64_t currentRowOffset, std::vector<uint_fast64_t> const& ddRowVariableIndices) const;
        template std::vector<uint_fast64_t> Add<DdType::CUDD>::toVector() const;
        template std::vector<uint_fast64_t> Add<DdType::CUDD>::toVector(Odd<DdType::CUDD> const& rowOdd) const;
        template void Add<DdType::CUDD>::addToVector(Odd<DdType::CUDD> const& odd, std::vector<uint_fast64_t> const& ddVariableIndices, std::vector<uint_fast64_t>& targetVector) const;
        template void Add<DdType::CUDD>::modifyVectorRec(DdNode const* dd, uint_fast64_t currentLevel, uint_fast64_t maxLevel, uint_fast64_t currentOffset, Odd<DdType::CUDD> const& odd, std::vector<uint_fast64_t> const& ddVariableIndices, std::vector<uint_fast64_t>& targetVector, std::function<uint_fast64_t (uint_fast64_t const&, double const&)> const& function) const;

    }
}