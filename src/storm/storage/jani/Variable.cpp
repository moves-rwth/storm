#include "storm/storage/jani/Variable.h"
#include "storm/storage/jani/visitor/JaniExpressionSubstitutionVisitor.h"

namespace storm {
    namespace jani {

        Variable::Variable(std::string const& name, JaniType* type, storm::expressions::Variable const& variable, storm::expressions::Expression const& init, bool transient) : name(name), variable(variable),  transient(transient), type(type), init(init){
            if (type->isArrayType()) {
                arrayType = type;
                while (type->isArrayType()) {
                    type = type->getChildType();
                }
            }
        }

        Variable::Variable(std::string const& name, JaniType* type, storm::expressions::Variable const& variable) : name(name), variable(variable), transient(false), type(type) , init(){
            if (type->isArrayType()) {
                arrayType = type;
                while (type->isArrayType()) {
                    type = type->getChildType();
                }
            }
        }

        Variable::~Variable() {
            // Intentionally left empty.
        }

        std::unique_ptr<Variable> Variable::clone() const {
            return std::make_unique<Variable>(*this);
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
            auto ptr = dynamic_cast<BasicType const*>(type);
            return ptr != nullptr && ptr->isBooleanType();
        }
        
        bool Variable::isBoundedVariable() const {
            if (isArrayVariable()) {
                auto ptr = dynamic_cast<ArrayType const*>(type);
                return ptr != nullptr && ptr->isBoundedType();
            } else {
                auto ptr = dynamic_cast<BoundedType const*>(type);
                return ptr != nullptr;
            }
        }

        bool Variable::isRealVariable() const {
            if (isBoundedVariable()) {
                auto ptr = dynamic_cast<BoundedType const*>(type);
                return ptr != nullptr && ptr->isRealType();
            } else {
                auto ptr = dynamic_cast<BasicType const*>(type);
                return ptr != nullptr && ptr->isRealType();
            }
        }

        bool Variable::isIntegerVariable() const {
            if (isBoundedVariable()) {
                auto ptr = dynamic_cast<BoundedType const*>(type);
                return ptr != nullptr && ptr->isIntegerType();
            } else {
                auto ptr = dynamic_cast<BasicType const*>(type);
                return ptr != nullptr && ptr->isIntegerType();
            }
        }
        
        bool Variable::isArrayVariable() const {
            auto ptr = dynamic_cast<ArrayType const*>(type);
            return ptr != nullptr && ptr->isArrayType();
        }
        
        bool Variable::isClockVariable() const {
            auto ptr = dynamic_cast<ClockType const*>(type);
            return ptr != nullptr && ptr->isClockType();
        }

        bool Variable::isContinuousVariable() const {
            auto ptr = dynamic_cast<ContinuousType const*>(type);
            return ptr != nullptr && ptr->isContinuousType();
        }
        
        bool Variable::isTransient() const {
            return transient;
        }

        bool Variable::hasInitExpression() const {
            return init.is_initialized();
        }

        storm::expressions::Expression const& Variable::getInitExpression() const {
            return this->init.get();
        }
        
        void Variable::setInitExpression(storm::expressions::Expression const& initialExpression) {
            this->init = initialExpression;
        }
        
        void Variable::substitute(std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution) {
            if (this->hasInitExpression()) {
                this->setInitExpression(substituteJaniExpression(this->getInitExpression(), substitution));
            }
            if (this->isBoundedVariable() && this->hasLowerBound()) {
                this->setLowerBound(substituteJaniExpression(this->getLowerBound(), substitution));
            }
            if (this->isBoundedVariable() && this->hasUpperBound()) {
                this->setUpperBound(substituteJaniExpression(this->getUpperBound(), substitution));
            }
        }

        storm::expressions::Expression const& Variable::getLowerBound() const {
            STORM_LOG_ASSERT(this->isBoundedVariable(), "Trying to get lowerBound for variable without lowerBound");
            return type->getLowerBound();
        }

        void Variable::setLowerBound(storm::expressions::Expression const& expression) {
            STORM_LOG_ASSERT(this->isBoundedVariable(), "Trying to set lowerBound for unbounded variable");
            type->setLowerBound(expression);
        }

        bool Variable::hasLowerBound() const {
            return this->isBoundedVariable() && this->getLowerBound().isInitialized();
        }

        storm::expressions::Expression const& Variable::getUpperBound() const {
            STORM_LOG_ASSERT(this->isBoundedVariable(), "Trying to get upperBound for variable without upperBound");
            return type->getUpperBound();
        }

        void Variable::setUpperBound(storm::expressions::Expression const& expression) {
            STORM_LOG_ASSERT(this->isBoundedVariable(), "Trying to set upperBound for unbounded variable");
            type->setUpperBound(expression);
        }

        bool Variable::hasUpperBound() const {
            return this->isBoundedVariable() && this->getUpperBound().isInitialized();
        }

        storm::expressions::Expression Variable::getRangeExpression() const {
            STORM_LOG_ASSERT(this->isBoundedVariable(), "Trying to get rangeExpression for unbounded variable");

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

        JaniType* Variable::getType() const {
            return type;
        }

        JaniType* Variable::getArrayType() const {
            return arrayType;
        }

        std::shared_ptr<Variable> Variable::makeBoundedVariable(const std::string &name, JaniType::ElementType type, const expressions::Variable &variable, boost::optional<storm::expressions::Expression> initValue, bool transient, boost::optional<storm::expressions::Expression> lowerBound, boost::optional<storm::expressions::Expression> upperBound) {
            if (initValue) {
                auto res = std::make_shared<Variable>(name, new storm::jani::BoundedType(type, lowerBound ? lowerBound.get() : storm::expressions::Expression(), upperBound ? upperBound.get() : storm::expressions::Expression()), variable, initValue.get(), transient);
                return res;
            } else {
                assert(!transient);
                auto res = std::make_shared<Variable>(name, new storm::jani::BoundedType(type, lowerBound ? lowerBound.get() : storm::expressions::Expression(), upperBound ? upperBound.get() : storm::expressions::Expression()), variable);
                return res;
            }
        }

        std::shared_ptr<Variable> Variable::makeBasicVariable(const std::string &name, JaniType::ElementType type, const expressions::Variable &variable, boost::optional<storm::expressions::Expression> initValue, bool transient) {
            if (initValue) {
                return std::make_shared<Variable>(name, new storm::jani::BasicType(type), variable, initValue.get(), transient);
            } else {
                assert(!transient);
                return std::make_shared<Variable>(name, new storm::jani::BasicType(type), variable);
            }
        }

        std::shared_ptr<Variable> Variable::makeClockVariable(const std::string &name, const expressions::Variable &variable, boost::optional<storm::expressions::Expression> initValue, bool transient) {
            if (initValue) {
                return std::make_shared<Variable>(name, new storm::jani::ClockType(), variable, initValue.get(), transient);
            } else {
                assert(!transient);
                return std::make_shared<Variable>(name, new storm::jani::ClockType(), variable);
            }
        }

        std::shared_ptr<Variable> Variable::makeArrayVariable(const std::string &name, JaniType* type, expressions::Variable &variable, boost::optional<storm::expressions::Expression> initValue, bool transient) {
            assert (type->isArrayType());
            if (initValue) {
                return std::make_shared<Variable>(name, type, variable, initValue.get(), transient);
            } else {
                assert(!transient);
                return std::make_shared<Variable>(name, type, variable);
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
