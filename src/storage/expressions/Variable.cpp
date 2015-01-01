#include "src/storage/expressions/Variable.h"
#include "src/storage/expressions/ExpressionManager.h"

namespace storm {
    namespace expressions {
        Variable::Variable(ExpressionManager const& manager, uint_fast64_t index) : manager(manager), index(index) {
            // Intentionally left empty.
        }

        bool Variable::operator==(Variable const& other) const {
            return manager == other.manager && index == other.index;
        }
        
        storm::expressions::Expression Variable::getExpression() const {
            return storm::expressions::Expression(*this);
        }
        
        uint_fast64_t Variable::getIndex() const {
            return index;
        }
        
        uint_fast64_t Variable::getOffset() const {
            return manager.getOffset(index);
        }
        
        std::string const& Variable::getName() const {
            return manager.getVariableName(index);
        }
        
        ExpressionReturnType Variable::getType() const {
            return manager.getVariableType(index);
        }
        
        ExpressionManager const& Variable::getManager() const {
            return manager;
        }
        
        bool Variable::hasBooleanType() const {
            return this->getType() == ExpressionReturnType::Bool;
        }
        
        bool Variable::hasIntegralType() const {
            return this->getType() == ExpressionReturnType::Int;
        }

        bool Variable::hasRationalType() const {
            return this->getType() == ExpressionReturnType::Double;
        }

        bool Variable::hasNumericType() const {
            return this->getType() == ExpressionReturnType::Int || this->getType() == ExpressionReturnType::Double;
        }
    }
}