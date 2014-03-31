#include "src/storage/expressions/SimpleValuation.h"

namespace storm {
    namespace expressions {
        SimpleValuation::SimpleValuation(std::size_t booleanVariableCount, std::size_t integerVariableCount, std::size_t doubleVariableCount) : identifierToIndexMap(), booleanValues(booleanVariableCount), integerValues(integerVariableCount), doubleValues(doubleVariableCount) {
            // Intentionally left empty.
        }
        
        SimpleValuation::SimpleValuation(std::shared_ptr<std::unordered_map<std::string, uint_fast64_t>> identifierToIndexMap, std::vector<bool> booleanValues, std::vector<int_fast64_t> integerValues, std::vector<double> doubleValues) : identifierToIndexMap(identifierToIndexMap), booleanValues(booleanValues), integerValues(integerValues), doubleValues(doubleValues) {
            // Intentionally left empty.
        }

        void SimpleValuation::setIdentifierIndex(std::string const& name, uint_fast64_t index) {
            (*this->identifierToIndexMap)[name] = index;
        }
        
        void SimpleValuation::setBooleanValue(std::string const& name, bool value) {
            this->booleanValues[(*this->identifierToIndexMap)[name]] = value;
        }
        
        void SimpleValuation::setIntegerValue(std::string const& name, int_fast64_t value) {
            this->integerValues[(*this->identifierToIndexMap)[name]] = value;
        }
        
        void SimpleValuation::setDoubleValue(std::string const& name, double value) {
            this->doubleValues[(*this->identifierToIndexMap)[name]] = value;
        }
        
        bool SimpleValuation::getBooleanValue(std::string const& name) const {
            auto const& nameIndexPair = this->identifierToIndexMap->find(name);
            return this->booleanValues[nameIndexPair->second];
        }
        
        int_fast64_t SimpleValuation::getIntegerValue(std::string const& name) const {
            auto const& nameIndexPair = this->identifierToIndexMap->find(name);
            return this->integerValues[nameIndexPair->second];
        }
        
        double SimpleValuation::getDoubleValue(std::string const& name) const {
            auto const& nameIndexPair = this->identifierToIndexMap->find(name);
            return this->doubleValues[nameIndexPair->second];
        }
    }
}