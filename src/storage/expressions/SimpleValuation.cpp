#include "src/storage/expressions/SimpleValuation.h"

#include <boost/functional/hash.hpp>
#include <boost/algorithm/string/join.hpp>

#include "src/storage/expressions/ExpressionManager.h"
#include "src/storage/expressions/Variable.h"

#include "src/utility/macros.h"
#include "src/exceptions/InvalidTypeException.h"

namespace storm {
    namespace expressions {
        SimpleValuation::SimpleValuation() : Valuation(nullptr) {
            // Intentionally left empty.
        }

        SimpleValuation::SimpleValuation(std::shared_ptr<storm::expressions::ExpressionManager const> const& manager) : Valuation(manager), booleanValues(this->getManager().getNumberOfBooleanVariables()), integerValues(this->getManager().getNumberOfIntegerVariables() + this->getManager().getNumberOfBitVectorVariables()), rationalValues(this->getManager().getNumberOfRationalVariables()) {
            // Intentionally left empty.
        }
        
        SimpleValuation::SimpleValuation(SimpleValuation const& other) : Valuation(other.getManager().getSharedPointer()) {
            if (this != &other) {
                booleanValues = other.booleanValues;
                integerValues = other.integerValues;
                rationalValues = other.rationalValues;
            }
        }
                
        SimpleValuation& SimpleValuation::operator=(SimpleValuation const& other) {
            if (this != &other) {
                this->setManager(other.getManager().getSharedPointer());
                booleanValues = other.booleanValues;
                integerValues = other.integerValues;
                rationalValues = other.rationalValues;
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
            return booleanValues[booleanVariable.getOffset()];
        }
        
        int_fast64_t SimpleValuation::getIntegerValue(Variable const& integerVariable) const {
            return integerValues[integerVariable.getOffset()];
        }

        int_fast64_t SimpleValuation::getBitVectorValue(Variable const& bitVectorVariable) const {
            return integerValues[bitVectorVariable.getOffset()];
        }
        
        double SimpleValuation::getRationalValue(Variable const& rationalVariable) const {
            return rationalValues[rationalVariable.getOffset()];
        }
        
        void SimpleValuation::setBooleanValue(Variable const& booleanVariable, bool value) {
            booleanValues[booleanVariable.getOffset()] = value;
        }
        
        void SimpleValuation::setIntegerValue(Variable const& integerVariable, int_fast64_t value) {
            integerValues[integerVariable.getOffset()] = value;
        }
        
        void SimpleValuation::setBitVectorValue(Variable const& bitVectorVariable, int_fast64_t value) {
            integerValues[bitVectorVariable.getOffset()] = value;
        }
        
        void SimpleValuation::setRationalValue(Variable const& rationalVariable, double value) {
            rationalValues[rationalVariable.getOffset()] = value;
        }
        
        std::string SimpleValuation::toPrettyString() const {
            std::vector<std::string> assignments;
            for (auto const& variable : this->getManager()) {
                std::stringstream stream;
                stream << variable.first.getName() << "=";
                if (variable.second.isBooleanType()) {
                    stream << std::boolalpha << this->getBooleanValue(variable.first) << std::noboolalpha;
                } else if (variable.second.isIntegerType()) {
                    stream << this->getIntegerValue(variable.first);
                } else if (variable.second.isRationalType()) {
                    stream << this->getRationalValue(variable.first);
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidTypeException, "Unexpected variable type.");
                }
                assignments.push_back(stream.str());
            }
            return "[" + boost::join(assignments, ", ") + "]";
        }
        
        std::ostream& operator<<(std::ostream& out, SimpleValuation const& valuation) {
            out << "valuation {" << std::endl;
            out << valuation.getManager() << std::endl;
            if (!valuation.booleanValues.empty()) {
                for (auto const& element : valuation.booleanValues) {
                    out << element << " ";
                }
                out << std::endl;
            }
            if (!valuation.integerValues.empty()) {
                for (auto const& element : valuation.integerValues) {
                    out << element << " ";
                }
                out << std::endl;
            }
            if (!valuation.rationalValues.empty()) {
                for (auto const& element : valuation.rationalValues) {
                    out << element << " ";
                }
                out << std::endl;
            }
            out << "}" << std::endl;
            
            return out;
        }
        
        std::size_t SimpleValuationPointerHash::operator()(SimpleValuation* valuation) const {
            size_t seed = std::hash<std::vector<bool>>()(valuation->booleanValues);
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