#include "src/storage/expressions/Valuation.h"

#include <boost/functional/hash.hpp>

#include "src/storage/expressions/ExpressionManager.h"
#include "src/storage/expressions/Variable.h"

namespace storm {
    namespace expressions {
        Valuation::Valuation(ExpressionManager const& manager) : manager(manager), booleanValues(nullptr), integerValues(nullptr), rationalValues(nullptr) {
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
        
        Valuation::Valuation(Valuation const& other) : manager(other.manager) {
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
        
        bool Valuation::operator==(Valuation const& other) const {
            return manager == other.manager && booleanValues == other.booleanValues && integerValues == other.integerValues && rationalValues == other.rationalValues;
        }
        
        bool Valuation::getBooleanValue(Variable const& booleanVariable) const {
            return (*booleanValues)[booleanVariable.getOffset()];
        }
        
        int_fast64_t Valuation::getIntegerValue(Variable const& integerVariable) const {
            return (*integerValues)[integerVariable.getOffset()];
        }
        
        double Valuation::getRationalValue(Variable const& rationalVariable) const {
            return (*rationalValues)[rationalVariable.getOffset()];
        }
        
        void Valuation::setBooleanValue(Variable const& booleanVariable, bool value) {
            (*booleanValues)[booleanVariable.getOffset()] = value;
        }
        
        void Valuation::setIntegerValue(Variable const& integerVariable, int_fast64_t value) {
            (*integerValues)[integerVariable.getOffset()] = value;
        }
        
        void Valuation::setRationalValue(Variable const& rationalVariable, double value) {
            (*rationalValues)[rationalVariable.getOffset()] = value;
        }

        ExpressionManager const& Valuation::getManager() const {
            return manager;
        }
        
        std::size_t ValuationPointerHash::operator()(Valuation* valuation) const {
            size_t seed = 0;
            boost::hash_combine(seed, valuation->booleanValues);
            boost::hash_combine(seed, valuation->integerValues);
            boost::hash_combine(seed, valuation->rationalValues);
            return seed;
        }
        
        bool ValuationPointerCompare::operator()(Valuation* valuation1, Valuation* valuation2) const {
            return *valuation1 == *valuation2;
        }
        
        bool ValuationPointerLess::operator()(Valuation* valuation1, Valuation* valuation2) const {
            return valuation1->booleanValues < valuation2->booleanValues || valuation1->integerValues < valuation2->integerValues || valuation1->rationalValues < valuation2->rationalValues;
        }
    }
}