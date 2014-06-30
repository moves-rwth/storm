#include "src/storage/expressions/SimpleValuation.h"

#include <set>

#include <boost/functional/hash.hpp>
#include "src/exceptions/ExceptionMacros.h"
#include "src/exceptions/InvalidArgumentException.h"
#include "src/exceptions/InvalidAccessException.h"

namespace storm {
    namespace expressions {
        bool SimpleValuation::operator==(SimpleValuation const& other) const {
            return this->identifierToValueMap == other.identifierToValueMap;
        }
        
        void SimpleValuation::addBooleanIdentifier(std::string const& name, bool initialValue) {
            LOG_THROW(this->identifierToValueMap.find(name) == this->identifierToValueMap.end(), storm::exceptions::InvalidArgumentException, "Identifier '" << name << "' already registered.");
            this->identifierToValueMap.emplace(name, initialValue);
        }
        
        void SimpleValuation::addIntegerIdentifier(std::string const& name, int_fast64_t initialValue) {
            LOG_THROW(this->identifierToValueMap.find(name) == this->identifierToValueMap.end(), storm::exceptions::InvalidArgumentException, "Identifier '" << name << "' already registered.");
            this->identifierToValueMap.emplace(name, initialValue);
        }
        
        void SimpleValuation::addDoubleIdentifier(std::string const& name, double initialValue) {
            LOG_THROW(this->identifierToValueMap.find(name) == this->identifierToValueMap.end(), storm::exceptions::InvalidArgumentException, "Identifier '" << name << "' already registered.");
            this->identifierToValueMap.emplace(name, initialValue);
        }
        
        void SimpleValuation::setBooleanValue(std::string const& name, bool value) {
            this->identifierToValueMap[name] = value;
        }
        
        void SimpleValuation::setIntegerValue(std::string const& name, int_fast64_t value) {
            this->identifierToValueMap[name] = value;
        }
        
        void SimpleValuation::setDoubleValue(std::string const& name, double value) {
            this->identifierToValueMap[name] = value;
        }
        
        void SimpleValuation::removeIdentifier(std::string const& name) {
            auto nameValuePair = this->identifierToValueMap.find(name);
            LOG_THROW(nameValuePair != this->identifierToValueMap.end(), storm::exceptions::InvalidArgumentException, "Deleting unknown identifier '" << name << "'.");
            this->identifierToValueMap.erase(nameValuePair);
        }
        
        ExpressionReturnType SimpleValuation::getIdentifierType(std::string const& name) const {
            auto nameValuePair = this->identifierToValueMap.find(name);
            LOG_THROW(nameValuePair != this->identifierToValueMap.end(), storm::exceptions::InvalidAccessException, "Access to unkown identifier '" << name << "'.");
            if (nameValuePair->second.type() == typeid(bool)) {
                return ExpressionReturnType::Bool;
            } else if (nameValuePair->second.type() == typeid(int_fast64_t)) {
                return ExpressionReturnType::Int;
            } else {
                return ExpressionReturnType::Double;
            }
        }
        
        bool SimpleValuation::containsBooleanIdentifier(std::string const& name) const {
            auto nameValuePair = this->identifierToValueMap.find(name);
            if (nameValuePair == this->identifierToValueMap.end()) {
                return false;
            }
            return nameValuePair->second.type() == typeid(bool);
        }
        
        bool SimpleValuation::containsIntegerIdentifier(std::string const& name) const {
            auto nameValuePair = this->identifierToValueMap.find(name);
            if (nameValuePair == this->identifierToValueMap.end()) {
                return false;
            }
            return nameValuePair->second.type() == typeid(int_fast64_t);
        }
        
        bool SimpleValuation::containsDoubleIdentifier(std::string const& name) const {
            auto nameValuePair = this->identifierToValueMap.find(name);
            if (nameValuePair == this->identifierToValueMap.end()) {
                return false;
            }
            return nameValuePair->second.type() == typeid(double);
        }
        
        bool SimpleValuation::getBooleanValue(std::string const& name) const {
            auto nameValuePair = this->identifierToValueMap.find(name);
            LOG_THROW(nameValuePair != this->identifierToValueMap.end(), storm::exceptions::InvalidAccessException, "Access to unkown identifier '" << name << "'.");
            return boost::get<bool>(nameValuePair->second);
        }
        
        int_fast64_t SimpleValuation::getIntegerValue(std::string const& name) const {
            auto nameValuePair = this->identifierToValueMap.find(name);
            LOG_THROW(nameValuePair != this->identifierToValueMap.end(), storm::exceptions::InvalidAccessException, "Access to unkown identifier '" << name << "'.");
            return boost::get<int_fast64_t>(nameValuePair->second);
        }
        
        double SimpleValuation::getDoubleValue(std::string const& name) const {
            auto nameValuePair = this->identifierToValueMap.find(name);
            LOG_THROW(nameValuePair != this->identifierToValueMap.end(), storm::exceptions::InvalidAccessException, "Access to unkown identifier '" << name << "'.");
            return boost::get<double>(nameValuePair->second);
        }
        
        std::size_t SimpleValuation::getNumberOfIdentifiers() const {
            return this->identifierToValueMap.size();
        }
        
        std::set<std::string> SimpleValuation::getIdentifiers() const {
            std::set<std::string> result;
            for (auto const& nameValuePair : this->identifierToValueMap) {
                result.insert(nameValuePair.first);
            }
            return result;
        }
        
        std::set<std::string> SimpleValuation::getBooleanIdentifiers() const {
            std::set<std::string> result;
            for (auto const& nameValuePair : this->identifierToValueMap) {
                if (nameValuePair.second.type() == typeid(bool)) {
                    result.insert(nameValuePair.first);
                }
            }
            return result;
        }
        
        std::set<std::string> SimpleValuation::getIntegerIdentifiers() const {
            std::set<std::string> result;
            for (auto const& nameValuePair : this->identifierToValueMap) {
                if (nameValuePair.second.type() == typeid(int_fast64_t)) {
                    result.insert(nameValuePair.first);
                }
            }
            return result;
        }
        
        std::set<std::string> SimpleValuation::getDoubleIdentifiers() const {
            std::set<std::string> result;
            for (auto const& nameValuePair : this->identifierToValueMap) {
                if (nameValuePair.second.type() == typeid(double)) {
                    result.insert(nameValuePair.first);
                }
            }
            return result;
        }
        
        std::ostream& operator<<(std::ostream& stream, SimpleValuation const& valuation) {
            stream << "{ ";
            uint_fast64_t elementIndex = 0;
            for (auto const& nameValuePair : valuation.identifierToValueMap) {
                stream << nameValuePair.first << " -> " << nameValuePair.second << " ";
                ++elementIndex;
                if (elementIndex < valuation.identifierToValueMap.size()) {
                    stream << ", ";
                }
            }
            stream << "}";
            
            return stream;
        }
        
        std::size_t SimpleValuationPointerHash::operator()(SimpleValuation* valuation) const {
            size_t seed = 0;
            for (auto const& nameValuePair : valuation->identifierToValueMap) {
                boost::hash_combine(seed, nameValuePair.first);
                boost::hash_combine(seed, nameValuePair.second);
            }
            return seed;
        }
        
        bool SimpleValuationPointerCompare::operator()(SimpleValuation* valuation1, SimpleValuation* valuation2) const {
            return *valuation1 == *valuation2;
        }
        
        bool SimpleValuationPointerLess::operator()(SimpleValuation* valuation1, SimpleValuation* valuation2) const {
            return valuation1->identifierToValueMap < valuation2->identifierToValueMap;
        }
    }
}