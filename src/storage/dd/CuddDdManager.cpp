#include <cmath>

#include "src/storage/dd/CuddDdManager.h"
#include "src/exceptions/InvalidArgumentException.h"

namespace storm {
    namespace dd {
        DdManager<CUDD>::DdManager() noexcept : metaVariableMap(), cuddManager() {
            // Intentionally left empty.
        }
        
        Dd<CUDD> DdManager<CUDD>::getOne() {
            return Dd<CUDD>(this->shared_from_this(), cuddManager.addOne(), {""});
        }
        
        Dd<CUDD> DdManager<CUDD>::getZero() {
            return Dd<CUDD>(this->shared_from_this(), cuddManager.addZero(), {""});
        }
        
        Dd<CUDD> DdManager<CUDD>::getConstant(double value) {
            return Dd<CUDD>(this->shared_from_this(), cuddManager.constant(value), {""});
        }
        
        Dd<CUDD> DdManager<CUDD>::getEncoding(std::string const& metaVariableName, int_fast64_t value) {
            std::vector<Dd<CUDD>> ddVariables = this->getMetaVariable(metaVariableName).getDdVariables();

            Dd<CUDD> result;
            if (value & (1ull << (ddVariables.size() - 1))) {
                result = ddVariables[0];
            } else {
                result = ddVariables[0];
            }
            
            for (std::size_t i = 1; i < ddVariables.size(); ++i) {
                if (value & (1ull << (ddVariables.size() - i - 1))) {
                    result *= ddVariables[i];
                } else {
                    result *= ~ddVariables[i];
                }
            }
            
            return result;
        }
        
        Dd<CUDD> DdManager<CUDD>::getRange(std::string const metaVariableName) {
            storm::dd::DdMetaVariable<CUDD> const& metaVariable = this->getMetaVariable(metaVariableName);
            
            Dd<CUDD> result = this->getZero();
            for (int_fast64_t value = metaVariable.getLow(); value <= metaVariable.getHigh(); ++value) {
                result.setValue(metaVariableName, value - metaVariable.getLow(), static_cast<double>(value));
            }
            return result;
        }

        void DdManager<CUDD>::addMetaVariable(std::string const& name, int_fast64_t low, int_fast64_t high) {
            if (high == low) {
                throw storm::exceptions::InvalidArgumentException() << "Range of meta variable must be at least 2 elements.";
            }
            
            std::size_t numberOfBits = static_cast<std::size_t>(std::ceil(std::log2(high - low)));
            
            std::vector<Dd<CUDD>> variables;
            for (std::size_t i = 0; i < numberOfBits; ++i) {
                variables.emplace_back(Dd<CUDD>(this->shared_from_this(), cuddManager.addVar(), {name}));
            }
            
            metaVariableMap.emplace(name, DdMetaVariable<CUDD>(name, low, high, variables, this->shared_from_this()));
        }
        
        void DdManager<CUDD>::addMetaVariablesInterleaved(std::vector<std::string> const& names, int_fast64_t low, int_fast64_t high) {
            if (names.size() == 0) {
                throw storm::exceptions::InvalidArgumentException() << "Illegal to add zero meta variables.";
            }
            
            // Add the variables in interleaved order.
            std::size_t numberOfBits = static_cast<std::size_t>(std::ceil(std::log2(high - low)));
            std::vector<std::vector<Dd<CUDD>>> variables;
            for (uint_fast64_t bit = 0; bit < numberOfBits; ++bit) {
                for (uint_fast64_t i = 0; i < names.size(); ++i) {
                    variables[i].emplace_back(Dd<CUDD>(this->shared_from_this(), cuddManager.addVar(), {names[i]}));
                }
            }
            
            // Now add the meta variables.
            for (uint_fast64_t i = 0; i < names.size(); ++i) {
                metaVariableMap.emplace(names[i], DdMetaVariable<CUDD>(names[i], low, high, variables[i], this->shared_from_this()));
            }
        }
        
        DdMetaVariable<CUDD> const& DdManager<CUDD>::getMetaVariable(std::string const& metaVariableName) const {
            auto const& nameVariablePair = metaVariableMap.find(metaVariableName);
            
            if (nameVariablePair == metaVariableMap.end()) {
                throw storm::exceptions::InvalidArgumentException() << "Unknown meta variable name.";
            }
            
            return nameVariablePair->second;
        }
        
        std::unordered_set<std::string> DdManager<CUDD>::getAllMetaVariableNames() const {
            std::unordered_set<std::string> result;
            for (auto const& nameValuePair : metaVariableMap) {
                result.insert(nameValuePair.first);
            }
            return result;
        }
        
        Cudd& DdManager<CUDD>::getCuddManager() {
            return this->cuddManager;
        }
    }
}