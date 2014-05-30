#include <cstring>
#include <algorithm>

#include "src/storage/dd/CuddDd.h"
#include "src/storage/dd/CuddDdManager.h"

#include "src/exceptions/InvalidArgumentException.h"

namespace storm {
    namespace dd {
        Dd<DdType::CUDD>::Dd(std::shared_ptr<DdManager<DdType::CUDD>> ddManager, ADD cuddAdd, std::set<std::string> const& containedMetaVariableNames) : ddManager(ddManager), cuddAdd(cuddAdd), containedMetaVariableNames(containedMetaVariableNames) {
            // Intentionally left empty.
        }
        
        bool Dd<DdType::CUDD>::operator==(Dd<DdType::CUDD> const& other) const {
            return this->cuddAdd == other.getCuddAdd();
        }
        
        bool Dd<DdType::CUDD>::operator!=(Dd<DdType::CUDD> const& other) const {
            return this->cuddAdd != other.getCuddAdd();
        }
        
        Dd<DdType::CUDD> Dd<DdType::CUDD>::ite(Dd<DdType::CUDD> const& thenDd, Dd<DdType::CUDD> const& elseDd) const {
            std::set<std::string> metaVariableNames(this->getContainedMetaVariableNames());
            metaVariableNames.insert(thenDd.getContainedMetaVariableNames().begin(), thenDd.getContainedMetaVariableNames().end());
            metaVariableNames.insert(elseDd.getContainedMetaVariableNames().begin(), elseDd.getContainedMetaVariableNames().end());
            
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
            this->getContainedMetaVariableNames().insert(other.getContainedMetaVariableNames().begin(), other.getContainedMetaVariableNames().end());
            
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
            this->getContainedMetaVariableNames().insert(other.getContainedMetaVariableNames().begin(), other.getContainedMetaVariableNames().end());
            
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
            this->getContainedMetaVariableNames().insert(other.getContainedMetaVariableNames().begin(), other.getContainedMetaVariableNames().end());
            
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
            this->getContainedMetaVariableNames().insert(other.getContainedMetaVariableNames().begin(), other.getContainedMetaVariableNames().end());
            
            return *this;
        }
        
        Dd<DdType::CUDD> Dd<DdType::CUDD>::operator!() const {
            Dd<DdType::CUDD> result(*this);
            result.complement();
            return result;
        }

        Dd<DdType::CUDD> Dd<DdType::CUDD>::operator&&(Dd<DdType::CUDD> const& other) const {
            std::set<std::string> metaVariableNames(this->getContainedMetaVariableNames());
            metaVariableNames.insert(other.getContainedMetaVariableNames().begin(), other.getContainedMetaVariableNames().end());
            
            // Rewrite a and b to not((not a) or (not b)). 
            return Dd<DdType::CUDD>(this->getDdManager(), ~(~this->getCuddAdd()).Or(~other.getCuddAdd()), metaVariableNames);
        }
        
        Dd<DdType::CUDD> Dd<DdType::CUDD>::operator||(Dd<DdType::CUDD> const& other) const {
            std::set<std::string> metaVariableNames(this->getContainedMetaVariableNames());
            metaVariableNames.insert(other.getContainedMetaVariableNames().begin(), other.getContainedMetaVariableNames().end());
            return Dd<DdType::CUDD>(this->getDdManager(), this->getCuddAdd().Or(other.getCuddAdd()), metaVariableNames);
        }
        
        Dd<DdType::CUDD>& Dd<DdType::CUDD>::complement() {
            this->cuddAdd = ~this->cuddAdd;
            return *this;
        }
        
        Dd<DdType::CUDD> Dd<DdType::CUDD>::equals(Dd<DdType::CUDD> const& other) const {
            Dd<DdType::CUDD> result(*this);
            result.cuddAdd = result.cuddAdd.Equals(other.getCuddAdd());
            return result;
        }
        
        Dd<DdType::CUDD> Dd<DdType::CUDD>::notEquals(Dd<DdType::CUDD> const& other) const {
            Dd<DdType::CUDD> result(*this);
            result.cuddAdd = result.cuddAdd.NotEquals(other.getCuddAdd());
            return result;
        }
        
        Dd<DdType::CUDD> Dd<DdType::CUDD>::less(Dd<DdType::CUDD> const& other) const {
            Dd<DdType::CUDD> result(*this);
            result.cuddAdd = result.cuddAdd.LessThan(other.getCuddAdd());
            return result;
        }
        
        Dd<DdType::CUDD> Dd<DdType::CUDD>::lessOrEqual(Dd<DdType::CUDD> const& other) const {
            Dd<DdType::CUDD> result(*this);
            result.cuddAdd = result.cuddAdd.LessThanOrEqual(other.getCuddAdd());
            return result;
        }
        
        Dd<DdType::CUDD> Dd<DdType::CUDD>::greater(Dd<DdType::CUDD> const& other) const {
            Dd<DdType::CUDD> result(*this);
            result.cuddAdd = result.cuddAdd.GreaterThan(other.getCuddAdd());
            return result;
        }
        
        Dd<DdType::CUDD> Dd<DdType::CUDD>::greaterOrEqual(Dd<DdType::CUDD> const& other) const {
            Dd<DdType::CUDD> result(*this);
            result.cuddAdd = result.cuddAdd.GreaterThanOrEqual(other.getCuddAdd());
            return result;
        }
        
        Dd<DdType::CUDD> Dd<DdType::CUDD>::minimum(Dd<DdType::CUDD> const& other) const {
            std::set<std::string> metaVariableNames(this->getContainedMetaVariableNames());
            metaVariableNames.insert(other.getContainedMetaVariableNames().begin(), other.getContainedMetaVariableNames().end());
            
            return Dd<DdType::CUDD>(this->getDdManager(), this->getCuddAdd().Minimum(other.getCuddAdd()), metaVariableNames);
        }

        Dd<DdType::CUDD> Dd<DdType::CUDD>::maximum(Dd<DdType::CUDD> const& other) const {
            std::set<std::string> metaVariableNames(this->getContainedMetaVariableNames());
            metaVariableNames.insert(other.getContainedMetaVariableNames().begin(), other.getContainedMetaVariableNames().end());
            
            return Dd<DdType::CUDD>(this->getDdManager(), this->getCuddAdd().Maximum(other.getCuddAdd()), metaVariableNames);
        }

        void Dd<DdType::CUDD>::existsAbstract(std::set<std::string> const& metaVariableNames) {
            Dd<DdType::CUDD> cubeDd(this->getDdManager()->getOne());
            
            for (auto const& metaVariableName : metaVariableNames) {
                // First check whether the DD contains the meta variable and erase it, if this is the case.
                if (!this->containsMetaVariable(metaVariableName)) {
                    throw storm::exceptions::InvalidArgumentException() << "Cannot abstract from meta variable that is not present in the DD.";
                }
                this->getContainedMetaVariableNames().erase(metaVariableName);
                
                DdMetaVariable<DdType::CUDD> const& metaVariable = this->getDdManager()->getMetaVariable(metaVariableName);
                cubeDd *= metaVariable.getCube();
            }
            
            this->cuddAdd = this->cuddAdd.OrAbstract(cubeDd.getCuddAdd());
        }
        
        void Dd<DdType::CUDD>::universalAbstract(std::set<std::string> const& metaVariableNames) {
            Dd<DdType::CUDD> cubeDd(this->getDdManager()->getOne());
            
            for (auto const& metaVariableName : metaVariableNames) {
                // First check whether the DD contains the meta variable and erase it, if this is the case.
                if (!this->containsMetaVariable(metaVariableName)) {
                    throw storm::exceptions::InvalidArgumentException() << "Cannot abstract from meta variable that is not present in the DD.";
                }
                this->getContainedMetaVariableNames().erase(metaVariableName);
                
                DdMetaVariable<DdType::CUDD> const& metaVariable = this->getDdManager()->getMetaVariable(metaVariableName);
                cubeDd *= metaVariable.getCube();
            }
            
            this->cuddAdd = this->cuddAdd.UnivAbstract(cubeDd.getCuddAdd());
        }
        
        void Dd<DdType::CUDD>::sumAbstract(std::set<std::string> const& metaVariableNames) {
            Dd<DdType::CUDD> cubeDd(this->getDdManager()->getOne());
            
            for (auto const& metaVariableName : metaVariableNames) {
                // First check whether the DD contains the meta variable and erase it, if this is the case.
                if (!this->containsMetaVariable(metaVariableName)) {
                    throw storm::exceptions::InvalidArgumentException() << "Cannot abstract from meta variable that is not present in the DD.";
                }
                this->getContainedMetaVariableNames().erase(metaVariableName);
                
                DdMetaVariable<DdType::CUDD> const& metaVariable = this->getDdManager()->getMetaVariable(metaVariableName);
                cubeDd *= metaVariable.getCube();
            }
            
            this->cuddAdd = this->cuddAdd.ExistAbstract(cubeDd.getCuddAdd());
        }
        
        void Dd<DdType::CUDD>::minAbstract(std::set<std::string> const& metaVariableNames) {
            Dd<DdType::CUDD> cubeDd(this->getDdManager()->getOne());
            
            for (auto const& metaVariableName : metaVariableNames) {
                // First check whether the DD contains the meta variable and erase it, if this is the case.
                if (!this->containsMetaVariable(metaVariableName)) {
                    throw storm::exceptions::InvalidArgumentException() << "Cannot abstract from meta variable that is not present in the DD.";
                }
                this->getContainedMetaVariableNames().erase(metaVariableName);
                
                DdMetaVariable<DdType::CUDD> const& metaVariable = this->getDdManager()->getMetaVariable(metaVariableName);
                cubeDd *= metaVariable.getCube();
            }
            
            this->cuddAdd = this->cuddAdd.MinAbstract(cubeDd.getCuddAdd());
        }
        
        void Dd<DdType::CUDD>::maxAbstract(std::set<std::string> const& metaVariableNames) {
            Dd<DdType::CUDD> cubeDd(this->getDdManager()->getOne());
            
            for (auto const& metaVariableName : metaVariableNames) {
                // First check whether the DD contains the meta variable and erase it, if this is the case.
                if (!this->containsMetaVariable(metaVariableName)) {
                    throw storm::exceptions::InvalidArgumentException() << "Cannot abstract from meta variable that is not present in the DD.";
                }
                this->getContainedMetaVariableNames().erase(metaVariableName);
                
                DdMetaVariable<DdType::CUDD> const& metaVariable = this->getDdManager()->getMetaVariable(metaVariableName);
                cubeDd *= metaVariable.getCube();
            }
            
            this->cuddAdd = this->cuddAdd.MaxAbstract(cubeDd.getCuddAdd());
        }
        
        bool Dd<DdType::CUDD>::equalModuloPrecision(Dd<DdType::CUDD> const& other, double precision, bool relative) const {
            if (relative) {
                return this->getCuddAdd().EqualSupNormRel(other.getCuddAdd(), precision);
            } else {
                return this->getCuddAdd().EqualSupNorm(other.getCuddAdd(), precision);
            }
        }
        
        void Dd<DdType::CUDD>::swapVariables(std::vector<std::pair<std::string, std::string>> const& metaVariablePairs) {
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
        
        Dd<DdType::CUDD> Dd<DdType::CUDD>::multiplyMatrix(Dd<DdType::CUDD> const& otherMatrix, std::set<std::string> const& summationMetaVariableNames) const {
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
            
            return Dd<DdType::CUDD>(this->getDdManager(), this->cuddAdd.MatrixMultiply(otherMatrix.getCuddAdd(), summationDdVariables), containedMetaVariableNames);
        }
        
        Dd<DdType::CUDD> Dd<DdType::CUDD>::greaterZero() const {
            return Dd<DdType::CUDD>(this->getDdManager(), this->getCuddAdd().BddStrictThreshold(0).Add(), this->getContainedMetaVariableNames());
        }
        
        uint_fast64_t Dd<DdType::CUDD>::getNonZeroCount() const {
            std::size_t numberOfDdVariables = 0;
            for (auto const& metaVariableName : this->containedMetaVariableNames) {
                numberOfDdVariables += this->getDdManager()->getMetaVariable(metaVariableName).getNumberOfDdVariables();
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
        
        void Dd<DdType::CUDD>::setValue(std::string const& metaVariableName, int_fast64_t variableValue, double targetValue) {
            std::map<std::string, int_fast64_t> metaVariableNameToValueMap;
            metaVariableNameToValueMap.emplace(metaVariableName, variableValue);
            this->setValue(metaVariableNameToValueMap, targetValue);
        }
        
        void Dd<DdType::CUDD>::setValue(std::string const& metaVariableName1, int_fast64_t variableValue1, std::string const& metaVariableName2, int_fast64_t variableValue2, double targetValue) {
            std::map<std::string, int_fast64_t> metaVariableNameToValueMap;
            metaVariableNameToValueMap.emplace(metaVariableName1, variableValue1);
            metaVariableNameToValueMap.emplace(metaVariableName2, variableValue2);
            this->setValue(metaVariableNameToValueMap, targetValue);
        }
        
        void Dd<DdType::CUDD>::setValue(std::map<std::string, int_fast64_t> const& metaVariableNameToValueMap, double targetValue) {
            Dd<DdType::CUDD> valueEncoding(this->getDdManager()->getOne());
            for (auto const& nameValuePair : metaVariableNameToValueMap) {
                valueEncoding *= this->getDdManager()->getEncoding(nameValuePair.first, nameValuePair.second);
                // Also record that the DD now contains the meta variable.
                this->addContainedMetaVariable(nameValuePair.first);
            }
            
            this->cuddAdd = valueEncoding.getCuddAdd().Ite(this->getDdManager()->getConstant(targetValue).getCuddAdd(), this->cuddAdd);
        }
        
        double Dd<DdType::CUDD>::getValue(std::map<std::string, int_fast64_t> const& metaVariableNameToValueMap) const {
            std::set<std::string> remainingMetaVariables(this->getContainedMetaVariableNames());
            Dd<DdType::CUDD> valueEncoding(this->getDdManager()->getOne());
            for (auto const& nameValuePair : metaVariableNameToValueMap) {
                valueEncoding *= this->getDdManager()->getEncoding(nameValuePair.first, nameValuePair.second);
                if (this->containsMetaVariable(nameValuePair.first)) {
                    remainingMetaVariables.erase(nameValuePair.first);
                }
            }
            
            if (!remainingMetaVariables.empty()) {
                throw storm::exceptions::InvalidArgumentException() << "Cannot evaluate function for which not all inputs were given.";
            }
            
            Dd<DdType::CUDD> value = *this * valueEncoding;
            value.sumAbstract(this->getContainedMetaVariableNames());
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
        
        bool Dd<DdType::CUDD>::containsMetaVariable(std::string const& metaVariableName) const {
            auto const& metaVariable = containedMetaVariableNames.find(metaVariableName);
            return metaVariable != containedMetaVariableNames.end();
        }
        
        bool Dd<DdType::CUDD>::containsMetaVariables(std::set<std::string> metaVariableNames) const {
            for (auto const& metaVariableName : metaVariableNames) {
                auto const& metaVariable = containedMetaVariableNames.find(metaVariableName);
                
                if (metaVariable == containedMetaVariableNames.end()) {
                    return false;
                }
            }
            return true;
        }
        
        std::set<std::string> const& Dd<DdType::CUDD>::getContainedMetaVariableNames() const {
            return this->containedMetaVariableNames;
        }
        
        std::set<std::string>& Dd<DdType::CUDD>::getContainedMetaVariableNames() {
            return this->containedMetaVariableNames;
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
        
        void Dd<DdType::CUDD>::addContainedMetaVariable(std::string const& metaVariableName) {
            this->getContainedMetaVariableNames().insert(metaVariableName);
        }

        void Dd<DdType::CUDD>::removeContainedMetaVariable(std::string const& metaVariableName) {
            this->getContainedMetaVariableNames().erase(metaVariableName);
        }
        
        std::shared_ptr<DdManager<DdType::CUDD>> Dd<DdType::CUDD>::getDdManager() const {
            return this->ddManager;
        }
        
        DdForwardIterator<DdType::CUDD> Dd<DdType::CUDD>::begin(bool enumerateDontCareMetaVariables) const {
            int* cube;
            double value;
            DdGen* generator = this->getCuddAdd().FirstCube(&cube, &value);
            return DdForwardIterator<DdType::CUDD>(this->getDdManager(), generator, cube, value, static_cast<bool>(Cudd_IsGenEmpty(generator)), &this->getContainedMetaVariableNames(), enumerateDontCareMetaVariables);
        }
        
        DdForwardIterator<DdType::CUDD> Dd<DdType::CUDD>::end(bool enumerateDontCareMetaVariables) const {
            return DdForwardIterator<DdType::CUDD>(this->getDdManager(), nullptr, nullptr, 0, true, nullptr, enumerateDontCareMetaVariables);
        }
        
        std::ostream & operator<<(std::ostream& out, const Dd<DdType::CUDD>& dd) {
            dd.exportToDot();
            return out;
        }

    }
}