#ifndef STORM_STORAGE_EXPRESSIONS_SIMPLEVALUATION_H_
#define STORM_STORAGE_EXPRESSIONS_SIMPLEVALUATION_H_

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>

#include "src/storage/expressions/Valuation.h"

namespace storm {
    namespace expressions {
        class SimpleValuation : public Valuation {
        public:
            SimpleValuation(std::size_t booleanVariableCount, std::size_t integerVariableCount, std::size_t doubleVariableCount);
            
            SimpleValuation(std::shared_ptr<std::unordered_map<std::string, uint_fast64_t>> identifierToIndexMap, std::vector<bool> booleanValues, std::vector<int_fast64_t> integerValues, std::vector<double> doubleValues);

            SimpleValuation() = default;
            SimpleValuation(SimpleValuation const&) = default;
            SimpleValuation(SimpleValuation&&) = default;
            SimpleValuation& operator=(SimpleValuation const&) = default;
            SimpleValuation& operator=(SimpleValuation&&) = default;
            
            void setIdentifierIndex(std::string const& name, uint_fast64_t index);
            
            void setBooleanValue(std::string const& name, bool value);
            void setIntegerValue(std::string const& name, int_fast64_t value);
            void setDoubleValue(std::string const& name, double value);
            
            virtual bool getBooleanValue(std::string const& name) const override;
            virtual int_fast64_t getIntegerValue(std::string const& name) const override;
            virtual double getDoubleValue(std::string const& name) const override;
            
        private:
            std::shared_ptr<std::unordered_map<std::string, uint_fast64_t>> identifierToIndexMap;
            std::vector<bool> booleanValues;
            std::vector<int_fast64_t> integerValues;
            std::vector<double> doubleValues;
        };
    }
}

#endif /* STORM_STORAGE_EXPRESSIONS_SIMPLEVALUATION_H_ */