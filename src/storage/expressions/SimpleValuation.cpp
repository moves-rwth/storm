#include "src/storage/expressions/SimpleValuation.h"

#include <boost/functional/hash.hpp>

#include "src/storage/expressions/ExpressionManager.h"
#include "src/storage/expressions/Variable.h"

namespace storm {
    namespace expressions {
        SimpleValuation::SimpleValuation(ExpressionManager const& manager) : Valuation(manager), booleanValues(nullptr), integerValues(nullptr), rationalValues(nullptr) {
            if (manager.getNumberOfBooleanVariables() > 0) {
                booleanValues = std::unique_ptr<std::vector<bool>>(new std::vector<bool>(manager.getNumberOfBooleanVariables()));
            }
            if (manager.getNumberOfIntegerVariables() > 0) {
                integerValues = std::unique_ptr<std::vector<int_fast64_t>>(new std::vector<int_fast64_t>(manager.getNumberOfIntegerVariables()));
            }
            if (manager.getNumberOfRationalVariables() > 0) {
                rationalValues = std::unique_ptr<std::vector<double>>(new std::vector<double>(manager.getNumberOfRationalVariables()));
            }
        }
        
        SimpleValuation::SimpleValuation(SimpleValuation const& other) : Valuation(other.getManager()) {
            if (other.booleanValues != nullptr) {
                booleanValues = std::unique_ptr<std::vector<bool>>(new std::vector<bool>(*other.booleanValues));
            }
            if (other.integerValues != nullptr) {
                integerValues = std::unique_ptr<std::vector<int_fast64_t>>(new std::vector<int_fast64_t>(*other.integerValues));
            }
            if (other.booleanValues != nullptr) {
                rationalValues = std::unique_ptr<std::vector<double>>(new std::vector<double>(*other.rationalValues));
            }
        }
        
        bool SimpleValuation::operator==(SimpleValuation const& other) const {
            return this->getManager() == other.getManager() && booleanValues == other.booleanValues && integerValues == other.integerValues && rationalValues == other.rationalValues;
        }
        
        bool SimpleValuation::getBooleanValue(Variable const& booleanVariable) const {
            return (*booleanValues)[booleanVariable.getOffset()];
        }
        
        int_fast64_t SimpleValuation::getIntegerValue(Variable const& integerVariable) const {
            return (*integerValues)[integerVariable.getOffset()];
        }
        
        double SimpleValuation::getRationalValue(Variable const& rationalVariable) const {
            return (*rationalValues)[rationalVariable.getOffset()];
        }
        
        void SimpleValuation::setBooleanValue(Variable const& booleanVariable, bool value) {
            (*booleanValues)[booleanVariable.getOffset()] = value;
        }
        
        void SimpleValuation::setIntegerValue(Variable const& integerVariable, int_fast64_t value) {
            (*integerValues)[integerVariable.getOffset()] = value;
        }
        
        void SimpleValuation::setRationalValue(Variable const& rationalVariable, double value) {
            (*rationalValues)[rationalVariable.getOffset()] = value;
        }
                
        std::size_t SimpleValuationPointerHash::operator()(SimpleValuation* valuation) const {
            size_t seed = 0;
            boost::hash_combine(seed, valuation->booleanValues);
            boost::hash_combine(seed, valuation->integerValues);
            boost::hash_combine(seed, valuation->rationalValues);
            return seed;
        }
        
        bool SimpleValuationPointerCompare::operator()(SimpleValuation* SimpleValuation1, SimpleValuation* SimpleValuation2) const {
            return *SimpleValuation1 == *SimpleValuation2;
        }
        
        bool SimpleValuationPointerLess::operator()(SimpleValuation* SimpleValuation1, SimpleValuation* SimpleValuation2) const {
            return SimpleValuation1->booleanValues < SimpleValuation2->booleanValues || SimpleValuation1->integerValues < SimpleValuation2->integerValues || SimpleValuation1->rationalValues < SimpleValuation2->rationalValues;
        }
    }
}