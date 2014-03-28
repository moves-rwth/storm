#include <algorithm>

#include "src/storage/dd/CuddDd.h"
#include "src/storage/dd/CuddDdManager.h"

#include "src/exceptions/InvalidArgumentException.h"

namespace storm {
    namespace dd {
        Dd<CUDD>::Dd(std::shared_ptr<DdManager<CUDD>> ddManager, ADD cuddAdd, std::set<std::string> const& containedMetaVariableNames) : ddManager(ddManager), cuddAdd(cuddAdd), containedMetaVariableNames(containedMetaVariableNames) {
            // Intentionally left empty.
        }
        
        bool Dd<CUDD>::operator==(Dd<CUDD> const& other) const {
            return this->getCuddAdd() == other.getCuddAdd();
        }
        
        Dd<CUDD> Dd<CUDD>::operator+(Dd<CUDD> const& other) const {
            Dd<CUDD> result(*this);
            result += other;
            return result;
        }
        
        Dd<CUDD>& Dd<CUDD>::operator+=(Dd<CUDD> const& other) {
            this->getCuddAdd() += other.getCuddAdd();
            
            // Join the variable sets of the two participating DDs.
            this->getContainedMetaVariableNames().insert(other.getContainedMetaVariableNames().begin(), other.getContainedMetaVariableNames().end());
            
            return *this;
        }
        
        Dd<CUDD> Dd<CUDD>::operator*(Dd<CUDD> const& other) const {
            Dd<CUDD> result(*this);
            result *= other;
            return result;
        }
        
        Dd<CUDD>& Dd<CUDD>::operator*=(Dd<CUDD> const& other) {
            this->getCuddAdd() *= other.getCuddAdd();
            
            // Join the variable sets of the two participating DDs.
            this->getContainedMetaVariableNames().insert(other.getContainedMetaVariableNames().begin(), other.getContainedMetaVariableNames().end());
            
            return *this;
        }
        
        Dd<CUDD> Dd<CUDD>::operator-(Dd<CUDD> const& other) const {
            Dd<CUDD> result(*this);
            result -= other;
            return result;
        }
        
        Dd<CUDD>& Dd<CUDD>::operator-=(Dd<CUDD> const& other) {
            this->getCuddAdd() -= other.getCuddAdd();
            
            // Join the variable sets of the two participating DDs.
            this->getContainedMetaVariableNames().insert(other.getContainedMetaVariableNames().begin(), other.getContainedMetaVariableNames().end());
            
            return *this;
        }
        
        Dd<CUDD> Dd<CUDD>::operator/(Dd<CUDD> const& other) const {
            Dd<CUDD> result(*this);
            result /= other;
            return result;
        }
        
        Dd<CUDD>& Dd<CUDD>::operator/=(Dd<CUDD> const& other) {
            this->getCuddAdd().Divide(other.getCuddAdd());
            
            // Join the variable sets of the two participating DDs.
            this->getContainedMetaVariableNames().insert(other.getContainedMetaVariableNames().begin(), other.getContainedMetaVariableNames().end());
            
            return *this;
        }
        
        Dd<CUDD> Dd<CUDD>::operator~() const {
            Dd<CUDD> result(*this);
            result.complement();
            return result;
        }
        
        Dd<CUDD>& Dd<CUDD>::complement() {
            this->getCuddAdd() = ~this->getCuddAdd();
            return *this;
        }
        
        Dd<CUDD> Dd<CUDD>::equals(Dd<CUDD> const& other) const {
            Dd<CUDD> result(*this);
            result.getCuddAdd().Equals(other.getCuddAdd());
            return result;
        }
        
        Dd<CUDD> Dd<CUDD>::notEquals(Dd<CUDD> const& other) const {
            Dd<CUDD> result(*this);
            result.getCuddAdd().NotEquals(other.getCuddAdd());
            return result;
        }
        
        Dd<CUDD> Dd<CUDD>::less(Dd<CUDD> const& other) const {
            Dd<CUDD> result(*this);
            result.getCuddAdd().LessThan(other.getCuddAdd());
            return result;
        }
        
        Dd<CUDD> Dd<CUDD>::lessOrEqual(Dd<CUDD> const& other) const {
            Dd<CUDD> result(*this);
            result.getCuddAdd().LessThanOrEqual(other.getCuddAdd());
            return result;
        }
        
        Dd<CUDD> Dd<CUDD>::greater(Dd<CUDD> const& other) const {
            Dd<CUDD> result(*this);
            result.getCuddAdd().GreaterThan(other.getCuddAdd());
            return result;
        }
        
        Dd<CUDD> Dd<CUDD>::greaterOrEqual(Dd<CUDD> const& other) const {
            Dd<CUDD> result(*this);
            result.getCuddAdd().GreaterThanOrEqual(other.getCuddAdd());
            return result;
        }
        
        void Dd<CUDD>::existsAbstract(std::set<std::string> const& metaVariableNames) {
            Dd<CUDD> cubeDd(this->getDdManager()->getOne());
            
            for (auto const& metaVariableName : metaVariableNames) {
                // First check whether the DD contains the meta variable and erase it, if this is the case.
                if (!this->containsMetaVariable(metaVariableName)) {
                    throw storm::exceptions::InvalidArgumentException() << "Cannot abstract from meta variable that is not present in the DD.";
                }
                this->getContainedMetaVariableNames().erase(metaVariableName);
                
                DdMetaVariable<CUDD> const& metaVariable = this->getDdManager()->getMetaVariable(metaVariableName);
                cubeDd *= metaVariable.getCube();
            }
            
            this->getCuddAdd().OrAbstract(cubeDd.getCuddAdd());
        }
        
        void Dd<CUDD>::sumAbstract(std::set<std::string> const& metaVariableNames) {
            Dd<CUDD> cubeDd(this->getDdManager()->getOne());
            
            for (auto const& metaVariableName : metaVariableNames) {
                // First check whether the DD contains the meta variable and erase it, if this is the case.
                if (!this->containsMetaVariable(metaVariableName)) {
                    throw storm::exceptions::InvalidArgumentException() << "Cannot abstract from meta variable that is not present in the DD.";
                }
                this->getContainedMetaVariableNames().erase(metaVariableName);
                
                DdMetaVariable<CUDD> const& metaVariable = this->getDdManager()->getMetaVariable(metaVariableName);
                cubeDd *= metaVariable.getCube();
            }
            
            this->getCuddAdd().ExistAbstract(cubeDd.getCuddAdd());
        }
        
        void Dd<CUDD>::minAbstract(std::set<std::string> const& metaVariableNames) {
            Dd<CUDD> cubeDd(this->getDdManager()->getOne());
            
            for (auto const& metaVariableName : metaVariableNames) {
                // First check whether the DD contains the meta variable and erase it, if this is the case.
                if (!this->containsMetaVariable(metaVariableName)) {
                    throw storm::exceptions::InvalidArgumentException() << "Cannot abstract from meta variable that is not present in the DD.";
                }
                this->getContainedMetaVariableNames().erase(metaVariableName);
                
                DdMetaVariable<CUDD> const& metaVariable = this->getDdManager()->getMetaVariable(metaVariableName);
                cubeDd *= metaVariable.getCube();
            }
            
            this->getCuddAdd().Minimum(cubeDd.getCuddAdd());
        }
        
        void Dd<CUDD>::maxAbstract(std::set<std::string> const& metaVariableNames) {
            Dd<CUDD> cubeDd(this->getDdManager()->getOne());
            
            for (auto const& metaVariableName : metaVariableNames) {
                // First check whether the DD contains the meta variable and erase it, if this is the case.
                if (!this->containsMetaVariable(metaVariableName)) {
                    throw storm::exceptions::InvalidArgumentException() << "Cannot abstract from meta variable that is not present in the DD.";
                }
                this->getContainedMetaVariableNames().erase(metaVariableName);
                
                DdMetaVariable<CUDD> const& metaVariable = this->getDdManager()->getMetaVariable(metaVariableName);
                cubeDd *= metaVariable.getCube();
            }
            
            this->getCuddAdd().Maximum(cubeDd.getCuddAdd());
        }
        
        void Dd<CUDD>::swapVariables(std::vector<std::pair<std::string, std::string>> const& metaVariablePairs) {
            std::vector<ADD> from;
            std::vector<ADD> to;
            for (auto const& metaVariablePair : metaVariablePairs) {
                DdMetaVariable<CUDD> const& variable1 = this->getDdManager()->getMetaVariable(metaVariablePair.first);
                DdMetaVariable<CUDD> const& variable2 = this->getDdManager()->getMetaVariable(metaVariablePair.second);

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
            this->getCuddAdd().SwapVariables(from, to);
        }
        
        Dd<CUDD> Dd<CUDD>::multiplyMatrix(Dd<CUDD> const& otherMatrix, std::set<std::string> const& summationMetaVariableNames) {
            std::vector<ADD> summationDdVariables;
            
            // Create the CUDD summation variables.
            for (auto const& metaVariableName : summationMetaVariableNames) {
                for (auto const& ddVariable : this->getDdManager()->getMetaVariable(metaVariableName).getDdVariables()) {
                    summationDdVariables.push_back(ddVariable.getCuddAdd());
                }
            }
            
            std::set<std::string> unionOfMetaVariableNames;
            std::set_union(this->getContainedMetaVariableNames().begin(), this->getContainedMetaVariableNames().end(), otherMatrix.getContainedMetaVariableNames().begin(), otherMatrix.getContainedMetaVariableNames().end(), std::inserter(unionOfMetaVariableNames, unionOfMetaVariableNames.begin()));
            std::set<std::string> containedMetaVariableNames;
            std::set_difference(unionOfMetaVariableNames.begin(), unionOfMetaVariableNames.end(), summationMetaVariableNames.begin(), summationMetaVariableNames.end(), std::inserter(containedMetaVariableNames, containedMetaVariableNames.begin()));
            
            return Dd<CUDD>(this->getDdManager(), this->getCuddAdd().MatrixMultiply(otherMatrix.getCuddAdd(), summationDdVariables), containedMetaVariableNames);
        }

        
        uint_fast64_t Dd<CUDD>::getNonZeroCount() const {
            std::size_t numberOfDdVariables = 0;
            for (auto const& metaVariableName : this->containedMetaVariableNames) {
                numberOfDdVariables += this->getDdManager()->getMetaVariable(metaVariableName).getNumberOfDdVariables();
            }
            return static_cast<uint_fast64_t>(this->cuddAdd.CountMinterm(static_cast<int>(numberOfDdVariables)));
        }
        
        uint_fast64_t Dd<CUDD>::getLeafCount() const {
            return static_cast<uint_fast64_t>(this->cuddAdd.CountLeaves());
        }
        
        uint_fast64_t Dd<CUDD>::getNodeCount() const {
            return static_cast<uint_fast64_t>(this->cuddAdd.nodeCount());
        }
        
        double Dd<CUDD>::getMin() const {
            ADD constantMinAdd = this->getCuddAdd().FindMin();
            return static_cast<double>(Cudd_V(constantMinAdd.getNode()));
        }
        
        double Dd<CUDD>::getMax() const {
            ADD constantMaxAdd = this->getCuddAdd().FindMax();
            return static_cast<double>(Cudd_V(constantMaxAdd.getNode()));
        }
        
        void Dd<CUDD>::setValue(std::string const& metaVariableName, int_fast64_t variableValue, double targetValue) {
            std::unordered_map<std::string, int_fast64_t> metaVariableNameToValueMap;
            metaVariableNameToValueMap.emplace(metaVariableName, variableValue);
            this->setValue(metaVariableNameToValueMap, targetValue);
        }
        
        void Dd<CUDD>::setValue(std::string const& metaVariableName1, int_fast64_t variableValue1, std::string const& metaVariableName2, int_fast64_t variableValue2, double targetValue) {
            std::unordered_map<std::string, int_fast64_t> metaVariableNameToValueMap;
            metaVariableNameToValueMap.emplace(metaVariableName1, variableValue1);
            metaVariableNameToValueMap.emplace(metaVariableName2, variableValue2);
            this->setValue(metaVariableNameToValueMap, targetValue);
        }
        
        void Dd<CUDD>::setValue(std::unordered_map<std::string, int_fast64_t> const& metaVariableNameToValueMap, double targetValue) {
            Dd<CUDD> valueEncoding(this->getDdManager()->getOne());
            for (auto const& nameValuePair : metaVariableNameToValueMap) {
                valueEncoding *= this->getDdManager()->getEncoding(nameValuePair.first, nameValuePair.second);
            }
            
            this->getCuddAdd() = valueEncoding.getCuddAdd().Ite(this->getDdManager()->getConstant(targetValue).getCuddAdd(), this->cuddAdd);
        }
        
        bool Dd<CUDD>::isOne() const {
            return *this == this->getDdManager()->getOne();
        }
        
        bool Dd<CUDD>::isZero() const {
            return *this == this->getDdManager()->getZero();
        }
        
        bool Dd<CUDD>::containsMetaVariable(std::string const& metaVariableName) const {
            auto const& metaVariable = containedMetaVariableNames.find(metaVariableName);
            return metaVariable != containedMetaVariableNames.end();
        }
        
        bool Dd<CUDD>::containsMetaVariables(std::set<std::string> metaVariableNames) const {
            for (auto const& metaVariableName : metaVariableNames) {
                auto const& metaVariable = containedMetaVariableNames.find(metaVariableName);
                
                if (metaVariable == containedMetaVariableNames.end()) {
                    return false;
                }
            }
            return true;
        }
        
        std::set<std::string> const& Dd<CUDD>::getContainedMetaVariableNames() const {
            return this->containedMetaVariableNames;
        }
        
        std::set<std::string>& Dd<CUDD>::getContainedMetaVariableNames() {
            return this->containedMetaVariableNames;
        }
        
        void Dd<CUDD>::exportToDot(std::string const& filename) const {
            FILE* filePointer = fopen(filename.c_str() , "w");
            this->getDdManager()->getCuddManager().DumpDot({this->cuddAdd}, nullptr, nullptr, filePointer);
            fclose(filePointer);
        }
        
        ADD Dd<CUDD>::getCuddAdd() {
            return this->cuddAdd;
        }
        
        ADD const& Dd<CUDD>::getCuddAdd() const {
            return this->cuddAdd;
        }
        
        void Dd<CUDD>::addContainedMetaVariable(std::string const& metaVariableName) {
            this->getContainedMetaVariableNames().insert(metaVariableName);
        }

        void Dd<CUDD>::removeContainedMetaVariable(std::string const& metaVariableName) {
            this->getContainedMetaVariableNames().erase(metaVariableName);
        }
        
        std::shared_ptr<DdManager<CUDD>> Dd<CUDD>::getDdManager() const {
            return this->ddManager;
        }
    }
}