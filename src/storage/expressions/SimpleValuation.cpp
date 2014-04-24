#include "src/storage/expressions/SimpleValuation.h"

#include <boost/functional/hash.hpp>

namespace storm {
    namespace expressions {
        SimpleValuation::SimpleValuation() : booleanIdentifierToIndexMap(new std::unordered_map<std::string, uint_fast64_t>()), integerIdentifierToIndexMap(new std::unordered_map<std::string, uint_fast64_t>()), doubleIdentifierToIndexMap(new std::unordered_map<std::string, uint_fast64_t>()), booleanValues(), integerValues(), doubleValues() {
            // Intentionally left empty.
        }
        
        bool SimpleValuation::operator==(SimpleValuation const& other) const {
            return this->booleanIdentifierToIndexMap.get() == other.booleanIdentifierToIndexMap.get() && this->integerIdentifierToIndexMap.get() == other.integerIdentifierToIndexMap.get() && this->doubleIdentifierToIndexMap.get() == other.doubleIdentifierToIndexMap.get() && this->booleanValues == other.booleanValues && this->integerValues == other.integerValues && this->doubleValues == other.doubleValues;
        }
        
        void SimpleValuation::addBooleanIdentifier(std::string const& name, bool initialValue) {
            this->booleanIdentifierToIndexMap->emplace(name, this->booleanValues.size());
            this->booleanValues.push_back(initialValue);
        }
        
        void SimpleValuation::addIntegerIdentifier(std::string const& name, int_fast64_t initialValue) {
            this->integerIdentifierToIndexMap->emplace(name, this->integerValues.size());
            this->integerValues.push_back(initialValue);
        }
        
        void SimpleValuation::addDoubleIdentifier(std::string const& name, double initialValue) {
            this->doubleIdentifierToIndexMap->emplace(name, this->doubleValues.size());
            this->doubleValues.push_back(initialValue);
        }
        
        void SimpleValuation::setBooleanValue(std::string const& name, bool value) {
            this->booleanValues[this->booleanIdentifierToIndexMap->at(name)] = value;
        }
        
        void SimpleValuation::setIntegerValue(std::string const& name, int_fast64_t value) {
            this->integerValues[this->integerIdentifierToIndexMap->at(name)] = value;
        }
        
        void SimpleValuation::setDoubleValue(std::string const& name, double value) {
            this->doubleValues[this->doubleIdentifierToIndexMap->at(name)] = value;
        }
        
        bool SimpleValuation::getBooleanValue(std::string const& name) const {
            auto const& nameIndexPair = this->booleanIdentifierToIndexMap->find(name);
            return this->booleanValues[nameIndexPair->second];
        }
        
        int_fast64_t SimpleValuation::getIntegerValue(std::string const& name) const {
            auto const& nameIndexPair = this->integerIdentifierToIndexMap->find(name);
            return this->integerValues[nameIndexPair->second];
        }
        
        double SimpleValuation::getDoubleValue(std::string const& name) const {
            auto const& nameIndexPair = this->doubleIdentifierToIndexMap->find(name);
            return this->doubleValues[nameIndexPair->second];
        }
        
        std::ostream& operator<<(std::ostream& stream, SimpleValuation const& valuation) {
            stream << "valuation { bool [";
            if (!valuation.booleanValues.empty()) {
                for (uint_fast64_t i = 0; i < valuation.booleanValues.size() - 1; ++i) {
                    stream << valuation.booleanValues[i] << ", ";
                }
                stream << valuation.booleanValues.back();
            }
            stream << "] int [";
            if (!valuation.integerValues.empty()) {
                for (uint_fast64_t i = 0; i < valuation.integerValues.size() - 1; ++i) {
                    stream << valuation.integerValues[i] << ", ";
                }
                stream << valuation.integerValues.back();
            }
            stream << "] double [";
            if (!valuation.doubleValues.empty()) {
                for (uint_fast64_t i = 0; i < valuation.doubleValues.size() - 1; ++i) {
                    stream << valuation.doubleValues[i] << ", ";
                }
                stream << valuation.doubleValues.back();
            }
            stream << "] }";
            
            return stream;
        }
        
        std::size_t SimpleValuationPointerHash::operator()(SimpleValuation* valuation) const {
            size_t seed = 0;
            for (auto const& value : valuation->booleanValues) {
                boost::hash_combine<bool>(seed, value);
            }
            for (auto const& value : valuation->integerValues) {
                boost::hash_combine<int_fast64_t>(seed, value);
            }
            for (auto const& value : valuation->doubleValues) {
                boost::hash_combine<double>(seed, value);
            }
            return seed;
        }
        
        bool SimpleValuationPointerCompare::operator()(SimpleValuation* valuation1, SimpleValuation* valuation2) const {
            return *valuation1 == *valuation2;
        }
        
        bool SimpleValuationPointerLess::operator()(SimpleValuation* valuation1, SimpleValuation* valuation2) const {
            // Compare boolean variables.
            bool less = valuation1->booleanValues < valuation2->booleanValues;
            if (less) {
                return true;
            }
            less = valuation1->integerValues < valuation2->integerValues;
            if (less) {
                return true;
            }
            less = valuation1->doubleValues < valuation2->doubleValues;
            if (less) {
                return true;
            } else {
                return false;
            }
        }
    }
}