#include "src/storage/expressions/SimpleValuation.h"

#include <boost/functional/hash.hpp>

#include "src/storage/expressions/ExpressionManager.h"
#include "src/storage/expressions/Variable.h"

namespace storm {
    namespace expressions {
        SimpleValuation::SimpleValuation() : Valuation(nullptr) {
            // Intentionally left empty.
        }

        SimpleValuation::SimpleValuation(std::shared_ptr<storm::expressions::ExpressionManager const> const& manager) : Valuation(manager), booleanValues(nullptr), integerValues(nullptr), boundedIntegerValues(nullptr), rationalValues(nullptr) {
            if (this->getManager().getNumberOfBooleanVariables() > 0) {
                booleanValues = std::unique_ptr<std::vector<bool>>(new std::vector<bool>(this->getManager().getNumberOfBooleanVariables()));
            }
            if (this->getManager().getNumberOfIntegerVariables() > 0) {
                integerValues = std::unique_ptr<std::vector<int_fast64_t>>(new std::vector<int_fast64_t>(this->getManager().getNumberOfIntegerVariables()));
            }
            if (this->getManager().getNumberOfBoundedIntegerVariables() > 0) {
                boundedIntegerValues = std::unique_ptr<std::vector<int_fast64_t>>(new std::vector<int_fast64_t>(this->getManager().getNumberOfBoundedIntegerVariables()));
            }
            if (this->getManager().getNumberOfRationalVariables() > 0) {
                rationalValues = std::unique_ptr<std::vector<double>>(new std::vector<double>(this->getManager().getNumberOfRationalVariables()));
            }
        }
        
        SimpleValuation::SimpleValuation(SimpleValuation const& other) : Valuation(other.getManager().getSharedPointer()) {
            if (other.booleanValues != nullptr) {
                booleanValues = std::unique_ptr<std::vector<bool>>(new std::vector<bool>(*other.booleanValues));
            }
            if (other.integerValues != nullptr) {
                integerValues = std::unique_ptr<std::vector<int_fast64_t>>(new std::vector<int_fast64_t>(*other.integerValues));
            }
            if (other.boundedIntegerValues != nullptr) {
                boundedIntegerValues = std::unique_ptr<std::vector<int_fast64_t>>(new std::vector<int_fast64_t>(*other.boundedIntegerValues));
            }
            if (other.rationalValues != nullptr) {
                rationalValues = std::unique_ptr<std::vector<double>>(new std::vector<double>(*other.rationalValues));
            }
        }
                
        SimpleValuation& SimpleValuation::operator=(SimpleValuation const& other) {
            if (this != &other) {
                this->setManager(other.getManager().getSharedPointer());
                if (other.booleanValues != nullptr) {
                    booleanValues = std::unique_ptr<std::vector<bool>>(new std::vector<bool>(*other.booleanValues));
                }
                if (other.integerValues != nullptr) {
                    integerValues = std::unique_ptr<std::vector<int_fast64_t>>(new std::vector<int_fast64_t>(*other.integerValues));
                }
                if (other.boundedIntegerValues != nullptr) {
                    boundedIntegerValues = std::unique_ptr<std::vector<int_fast64_t>>(new std::vector<int_fast64_t>(*other.boundedIntegerValues));
                }
                if (other.booleanValues != nullptr) {
                    rationalValues = std::unique_ptr<std::vector<double>>(new std::vector<double>(*other.rationalValues));
                }
            }
            return *this;
        }
        
        SimpleValuation::SimpleValuation(SimpleValuation&& other) : Valuation(other.getManager().getSharedPointer()) {
            if (this != &other) {
                booleanValues = std::move(other.booleanValues);
                integerValues = std::move(other.integerValues);
                rationalValues = std::move(other.rationalValues);
            }
        }
        
        SimpleValuation& SimpleValuation::operator=(SimpleValuation&& other) {
            if (this != &other) {
                this->setManager(other.getManager().getSharedPointer());
                booleanValues = std::move(other.booleanValues);
                integerValues = std::move(other.integerValues);
                rationalValues = std::move(other.rationalValues);
            }
            return *this;
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

        int_fast64_t SimpleValuation::getBoundedIntegerValue(Variable const& integerVariable) const {
            return (*boundedIntegerValues)[integerVariable.getOffset()];
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
        
        void SimpleValuation::setBoundedIntegerValue(Variable const& integerVariable, int_fast64_t value) {
            (*boundedIntegerValues)[integerVariable.getOffset()] = value;
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