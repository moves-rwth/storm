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
        
        bool SimpleValuation::getBooleanValue(std::string const& name) const {
            return false;
        }
        
        int_fast64_t SimpleValuation::getIntegerValue(std::string const& name) const {
            return 0;
        }
        
        double SimpleValuation::getDoubleValue(std::string const& name) const {
            return 0.0;
        }
    }
}