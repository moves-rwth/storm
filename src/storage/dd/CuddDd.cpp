#include "src/storage/dd/CuddDd.h"
#include "src/storage/dd/CuddDdManager.h"

namespace storm {
    namespace dd {
        Dd<CUDD>::Dd(std::shared_ptr<DdManager<CUDD>> ddManager, ADD cuddAdd, std::unordered_set<std::string> const& containedMetaVariableNames) noexcept : ddManager(ddManager), cuddAdd(cuddAdd), containedMetaVariableNames(containedMetaVariableNames) {
            // Intentionally left empty.
        }
        
        Dd<CUDD> Dd<CUDD>::operator+(Dd<CUDD> const& other) const {
            Dd<CUDD> result(*this);
            result += other;
            return result;
        }
        
        Dd<CUDD>& Dd<CUDD>::operator+=(Dd<CUDD> const& other) {
            cuddAdd += other.getCuddAdd();
            
            // Join the variable sets of the two participating DDs.
            std::copy(other.getContainedMetaVariableNames().begin(), other.getContainedMetaVariableNames().end(), std::inserter(this->containedMetaVariableNames, this->containedMetaVariableNames.end()));
            
            return *this;
        }
        
        Dd<CUDD> Dd<CUDD>::operator*(Dd<CUDD> const& other) const {
            Dd<CUDD> result(*this);
            result *= other;
            return result;
        }
        
        Dd<CUDD>& Dd<CUDD>::operator*=(Dd<CUDD> const& other) {
            cuddAdd *= other.getCuddAdd();
            
            // Join the variable sets of the two participating DDs.
            std::copy(other.getContainedMetaVariableNames().begin(), other.getContainedMetaVariableNames().end(), std::inserter(this->containedMetaVariableNames, this->containedMetaVariableNames.end()));
            
            return *this;
        }
        
        Dd<CUDD> Dd<CUDD>::operator-(Dd<CUDD> const& other) const {
            Dd<CUDD> result(*this);
            result -= other;
            return result;
        }
        
        Dd<CUDD>& Dd<CUDD>::operator-=(Dd<CUDD> const& other) {
            cuddAdd -= other.getCuddAdd();
            
            // Join the variable sets of the two participating DDs.
            std::copy(other.getContainedMetaVariableNames().begin(), other.getContainedMetaVariableNames().end(), std::inserter(this->containedMetaVariableNames, this->containedMetaVariableNames.end()));
            
            return *this;
        }
        
        Dd<CUDD> Dd<CUDD>::operator/(Dd<CUDD> const& other) const {
            Dd<CUDD> result(*this);
            result /= other;
            return result;
        }
        
        Dd<CUDD>& Dd<CUDD>::operator/=(Dd<CUDD> const& other) {
            cuddAdd.Divide(other.getCuddAdd());
            
            // Join the variable sets of the two participating DDs.
            std::copy(other.getContainedMetaVariableNames().begin(), other.getContainedMetaVariableNames().end(), std::inserter(this->containedMetaVariableNames, this->containedMetaVariableNames.end()));
            
            return *this;
        }
        
        Dd<CUDD> Dd<CUDD>::operator~() const {
            Dd<CUDD> result(*this);
            result.complement();
            return result;
        }
        
        Dd<CUDD>& Dd<CUDD>::complement() {
            cuddAdd = ~cuddAdd;
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
        
        void Dd<CUDD>::existsAbstract(std::unordered_set<std::string> const& metaVariableNames) {
            Dd<CUDD> cubeDd(this->getDdManager()->getOne());
            
            for (auto const& metaVariableName : metaVariableNames) {
                DdMetaVariable<CUDD> const& metaVariable = this->getDdManager()->getMetaVariable(metaVariableName);
                cubeDd *= metaVariable.getCube();
            }
            
            this->getCuddAdd().OrAbstract(cubeDd.getCuddAdd());
        }
        
        void Dd<CUDD>::sumAbstract(std::unordered_set<std::string> const& metaVariableNames) {
            Dd<CUDD> cubeDd(this->getDdManager()->getOne());
            
            for (auto const& metaVariableName : metaVariableNames) {
                DdMetaVariable<CUDD> const& metaVariable = this->getDdManager()->getMetaVariable(metaVariableName);
                cubeDd *= metaVariable.getCube();
            }
            
            this->getCuddAdd().ExistAbstract(cubeDd.getCuddAdd());
        }
        
        void Dd<CUDD>::minAbstract(std::unordered_set<std::string> const& metaVariableNames) {
            Dd<CUDD> cubeDd(this->getDdManager()->getOne());
            
            for (auto const& metaVariableName : metaVariableNames) {
                DdMetaVariable<CUDD> const& metaVariable = this->getDdManager()->getMetaVariable(metaVariableName);
                cubeDd *= metaVariable.getCube();
            }
            
            this->getCuddAdd().Minimum(cubeDd.getCuddAdd());
        }
        
        void Dd<CUDD>::maxAbstract(std::unordered_set<std::string> const& metaVariableNames) {
            Dd<CUDD> cubeDd(this->getDdManager()->getOne());
            
            for (auto const& metaVariableName : metaVariableNames) {
                DdMetaVariable<CUDD> const& metaVariable = this->getDdManager()->getMetaVariable(metaVariableName);
                cubeDd *= metaVariable.getCube();
            }
            
            this->getCuddAdd().Maximum(cubeDd.getCuddAdd());
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
            // TODO: Fill this
        }
        
        bool Dd<CUDD>::containsMetaVariable(std::string const& metaVariableName) const {
            auto const& metaVariable = containedMetaVariableNames.find(metaVariableName);
            return metaVariable != containedMetaVariableNames.end();
        }
        
        bool Dd<CUDD>::containsMetaVariables(std::unordered_set<std::string> metaVariableNames) const {
            for (auto const& metaVariableName : metaVariableNames) {
                auto const& metaVariable = containedMetaVariableNames.find(metaVariableName);
                
                if (metaVariable == containedMetaVariableNames.end()) {
                    return false;
                }
            }
            return true;
        }
        
        std::unordered_set<std::string> const& Dd<CUDD>::getContainedMetaVariableNames() const {
            return containedMetaVariableNames;
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
        
        std::shared_ptr<DdManager<CUDD>> Dd<CUDD>::getDdManager() const {
            return this->ddManager;
        }
    }
}