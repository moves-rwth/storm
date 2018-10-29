#include "storm/storage/jani/ArrayVariable.h"

#include "storm/storage/jani/expressions/JaniExpressionSubstitutionVisitor.h"

namespace storm {
    namespace jani {
        
        ArrayVariable::ArrayVariable(std::string const& name, storm::expressions::Variable const& variable,  ElementType const& elementType) : Variable(name, variable), elementType(elementType) {
            // Intentionally left empty.
        }
        
        ArrayVariable::ArrayVariable(std::string const& name, storm::expressions::Variable const& variable, ElementType const& elementType, storm::expressions::Expression const& initValue, bool transient) : Variable(name, variable, initValue, transient), elementType(elementType) {
            // Intentionally left empty.
        }
        
        void ArrayVariable::setLowerElementTypeBound(storm::expressions::Expression const& lowerBound) {
            lowerElementTypeBound = lowerBound;
        }
        
        void ArrayVariable::setUpperElementTypeBound(storm::expressions::Expression const& upperBound) {
            upperElementTypeBound = upperBound;
        }
       
        bool ArrayVariable::hasElementTypeBound() const {
            return hasLowerElementTypeBound() || hasUpperElementTypeBound();
        }
        
        bool ArrayVariable::hasLowerElementTypeBound() const {
            return lowerElementTypeBound.isInitialized();
        }
        
        bool ArrayVariable::hasUpperElementTypeBound() const {
            return upperElementTypeBound.isInitialized();
        }
        
        storm::expressions::Expression const& ArrayVariable::getLowerElementTypeBound() const {
            return lowerElementTypeBound;
        }
        
        storm::expressions::Expression const& ArrayVariable::getUpperElementTypeBound() const {
            return upperElementTypeBound;
        }
        
        typename ArrayVariable::ElementType ArrayVariable::getElementType() const {
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
            if (hasLowerElementTypeBound()) {
                setLowerElementTypeBound(substituteJaniExpression(getLowerElementTypeBound(), substitution));
            }
            if (hasUpperElementTypeBound()) {
                setUpperElementTypeBound(substituteJaniExpression(getUpperElementTypeBound(), substitution));
            }
        }
    }
}
