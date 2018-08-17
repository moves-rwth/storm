#include "storm/storage/jani/ArrayVariable.h"

namespace storm {
    namespace jani {
        
        ArrayVariable::ArrayVariable(std::string const& name, storm::expressions::Variable const& variable) : Variable(name, variable) {
            // Intentionally left empty.
        }
        
        ArrayVariable::ArrayVariable(std::string const& name, storm::expressions::Variable const& variable, storm::expressions::Expression const& initValue) : Variable(name, variable, initValue, transient) {
            // Intentionally left empty.
        }
        
        void ArrayVariable::setElementTypeBounds(storm::expressions::Expression lowerBound, storm::expressions::Expression upperBound) {
            elementTypeBounds = std::make_pair(lowerBound, upperBound);
        }
        
        bool ArrayVariable::hasElementTypeBounds() const {
            return elementTypeBounds.is_initialized();
        }
        
        std::pair<storm::expressions::Expression, storm::expressions::Expression> const& ArrayVariable::getElementTypeBounds() const {
            return elementTypeBounds.get();
        }
        
        void ArrayVariable::setMaxSize(uint64_t size) {
            maxSize = size;
        }
        
        bool ArrayVariable::hasMaxSize() const {
            return maxSize.is_initialized();
        }
        
        uint64_t ArrayVariable::getMaxSize() const {
            return maxSize.get();
        }
        
        ElementType ArrayVariable::getElementType() const {
            return elementType;
        }
        
        std::unique_ptr<Variable> ArrayVariable::clone() const {
            return std::make_unique<ArrayVariable>(*this);
        }
        
        bool ArrayVariable::isArrayVariable() const {
            return true;
        }
        
        void ArrayVariable::substitute(std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution) {
            Variable::substitute(substitution);
            if (hasElementTypeBounds()) {
                setElementTypeBounds(elementTypeBounds->first.substitute(substitution), elementTypeBounds->second.substitute(substitution));
            }
        }
    }
}
