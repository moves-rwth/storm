#include "src/storage/jani/BoundedIntegerVariable.h"

namespace storm {
    namespace jani {
        
        BoundedIntegerVariable::BoundedIntegerVariable(std::string const& name, storm::expressions::Variable const& variable, storm::expressions::Expression const& lowerBound, storm::expressions::Expression const& upperBound, storm::expressions::Expression const& initialValue) : Variable(name, variable, initialValue), lowerBound(lowerBound), upperBound(upperBound) {
            // Intentionally left empty.
        }
        
        storm::expressions::Expression const& BoundedIntegerVariable::getLowerBound() const {
            return lowerBound;
        }
        
        storm::expressions::Expression const& BoundedIntegerVariable::getUpperBound() const {
            return upperBound;
        }
        
    }
}