#include "storm/storage/jani/BoundedIntegerVariable.h"
#include "storm/exceptions/NotImplementedException.h"
#include "storm/storage/jani/expressions/JaniExpressionSubstitutionVisitor.h"
#include "storm/utility/macros.h"

namespace storm {
    namespace jani {
        
        BoundedIntegerVariable::BoundedIntegerVariable(std::string const& name, storm::expressions::Variable const& variable, storm::expressions::Expression const& initValue, bool transient, storm::expressions::Expression const& lowerBound, storm::expressions::Expression const& upperBound) : Variable(name, variable, initValue, transient), lowerBound(lowerBound), upperBound(upperBound) {
            // Intentionally left empty.
        }
        
        BoundedIntegerVariable::BoundedIntegerVariable(std::string const& name, storm::expressions::Variable const& variable, storm::expressions::Expression const& initValue, storm::expressions::Expression const& lowerBound, storm::expressions::Expression const& upperBound) : Variable(name, variable, initValue), lowerBound(lowerBound), upperBound(upperBound) {
            // Intentionally left empty.
        }
 
        BoundedIntegerVariable::BoundedIntegerVariable(std::string const& name, storm::expressions::Variable const& variable, storm::expressions::Expression const& lowerBound, storm::expressions::Expression const& upperBound) : Variable(name, variable), lowerBound(lowerBound), upperBound(upperBound) {
            // Intentionally left empty.
        }

        std::unique_ptr<Variable> BoundedIntegerVariable::clone() const {
            return std::make_unique<BoundedIntegerVariable>(*this);
        }
        
        storm::expressions::Expression const& BoundedIntegerVariable::getLowerBound() const {
            return lowerBound;
        }
        
        void BoundedIntegerVariable::setLowerBound(storm::expressions::Expression const& expression) {
            this->lowerBound = expression;
        }
        
        bool BoundedIntegerVariable::hasLowerBound() const {
            return this->lowerBound.isInitialized();
        }
        
        storm::expressions::Expression const& BoundedIntegerVariable::getUpperBound() const {
            return upperBound;
        }
        
        void BoundedIntegerVariable::setUpperBound(storm::expressions::Expression const& expression) {
            this->upperBound = expression;
        }
        
        bool BoundedIntegerVariable::hasUpperBound() const {
            return this->upperBound.isInitialized();
        }
        
        storm::expressions::Expression BoundedIntegerVariable::getRangeExpression() const {
            storm::expressions::Expression range;
            if (this->hasLowerBound()) {
                range = this->getLowerBound() <= this->getExpressionVariable();
            }
            if (this->hasUpperBound()) {
                if (range.isInitialized()) {
                    range = range && this->getExpressionVariable() <= this->getUpperBound();
                } else {
                    range = this->getExpressionVariable() <= this->getUpperBound();
                }
            }
            return range;
        }

        bool BoundedIntegerVariable::isBoundedIntegerVariable() const {
            return true;
        }
        
        void BoundedIntegerVariable::substitute(std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution) {
            Variable::substitute(substitution);
            if (this->hasLowerBound()) {
                this->setLowerBound(substituteJaniExpression(this->getLowerBound(), substitution));
            }
            if (this->hasUpperBound()) {
                this->setUpperBound(substituteJaniExpression(this->getUpperBound(), substitution));
            }
        }
        
        std::shared_ptr<BoundedIntegerVariable> makeBoundedIntegerVariable(std::string const& name, storm::expressions::Variable const& variable, boost::optional<storm::expressions::Expression> initValue, bool transient, boost::optional<storm::expressions::Expression> lowerBound, boost::optional<storm::expressions::Expression> upperBound) {
            if (initValue) {
                return std::make_shared<BoundedIntegerVariable>(name, variable, initValue.get(), transient, lowerBound ? lowerBound.get() : storm::expressions::Expression(), upperBound ? upperBound.get() : storm::expressions::Expression());
            } else {
                assert(!transient);
                return std::make_shared<BoundedIntegerVariable>(name, variable, lowerBound ? lowerBound.get() : storm::expressions::Expression(), upperBound ? upperBound.get() : storm::expressions::Expression());
            }
        }
    }
}
