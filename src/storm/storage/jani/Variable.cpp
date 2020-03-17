#include "storm/storage/jani/Variable.h"

#include "storm/storage/jani/BooleanVariable.h"
#include "storm/storage/jani/BoundedIntegerVariable.h"
#include "storm/storage/jani/UnboundedIntegerVariable.h"
#include "storm/storage/jani/RealVariable.h"
#include "storm/storage/jani/ArrayVariable.h"
#include "storm/storage/jani/ClockVariable.h"
#include "storm/storage/jani/expressions/JaniExpressionSubstitutionVisitor.h"

namespace storm {
    namespace jani {

        Variable::Variable(std::string const& name, storm::expressions::Variable const& variable, storm::expressions::Expression const& init, bool transient) : name(name), variable(variable),  transient(transient), init(init) {
            // Intentionally left empty.
        }

        Variable::Variable(std::string const& name, storm::expressions::Variable const& variable) : name(name), variable(variable), transient(false), init() {
            // Intentionally left empty.
        }

        Variable::~Variable() {
            // Intentionally left empty.
        }
        
        storm::expressions::Variable const& Variable::getExpressionVariable() const {
            return variable;
        }
        
        void Variable::setExpressionVariable(storm::expressions::Variable const& newVariable) {
            variable = newVariable;
        }

        std::string const& Variable::getName() const {
            return name;
        }
        
        void Variable::setName(std::string const& newName) {
            name = newName;
        }
        
        bool Variable::isBooleanVariable() const {
            return false;
        }
        
        bool Variable::isBoundedIntegerVariable() const {
            return false;
        }
        
        bool Variable::isUnboundedIntegerVariable() const {
            return false;
        }

        bool Variable::isRealVariable() const {
            return false;
        }
        
        bool Variable::isArrayVariable() const {
            return false;
        }
        
        bool Variable::isClockVariable() const {
            return false;
        }
        
        bool Variable::isTransient() const {
            return transient;
        }

        bool Variable::hasInitExpression() const {
            return static_cast<bool>(init);
        }

        storm::expressions::Expression const& Variable::getInitExpression() const {
            return this->init.get();
        }
        
        void Variable::setInitExpression(storm::expressions::Expression const& initialExpression) {
            this->init = initialExpression;
        }
        
        BooleanVariable& Variable::asBooleanVariable() {
            return static_cast<BooleanVariable&>(*this);
        }
        
        BooleanVariable const& Variable::asBooleanVariable() const {
            return static_cast<BooleanVariable const&>(*this);
        }
        
        BoundedIntegerVariable& Variable::asBoundedIntegerVariable() {
            return static_cast<BoundedIntegerVariable&>(*this);
        }
        
        BoundedIntegerVariable const& Variable::asBoundedIntegerVariable() const {
            return static_cast<BoundedIntegerVariable const&>(*this);
        }
        
        UnboundedIntegerVariable& Variable::asUnboundedIntegerVariable() {
            return static_cast<UnboundedIntegerVariable&>(*this);
        }
        
        UnboundedIntegerVariable const& Variable::asUnboundedIntegerVariable() const {
            return static_cast<UnboundedIntegerVariable const&>(*this);
        }
        
        RealVariable& Variable::asRealVariable() {
            return static_cast<RealVariable&>(*this);
        }
        
        RealVariable const& Variable::asRealVariable() const {
            return static_cast<RealVariable const&>(*this);
        }
        
        ArrayVariable& Variable::asArrayVariable() {
            return static_cast<ArrayVariable&>(*this);
        }
        
        ArrayVariable const& Variable::asArrayVariable() const {
            return static_cast<ArrayVariable const&>(*this);
        }
        
        ClockVariable& Variable::asClockVariable() {
            return static_cast<ClockVariable&>(*this);
        }
        
        ClockVariable const& Variable::asClockVariable() const {
            return static_cast<ClockVariable const&>(*this);
        }
        
        void Variable::substitute(std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution) {
            if (this->hasInitExpression()) {
                this->setInitExpression(substituteJaniExpression(this->getInitExpression(), substitution));
            }
        }

        bool operator==(Variable const& lhs, Variable const& rhs) {
            return lhs.getExpressionVariable() == rhs.getExpressionVariable();
        }

        bool operator!=(Variable const& lhs, Variable const& rhs) {
            return !(lhs == rhs);
        }
    }
}
