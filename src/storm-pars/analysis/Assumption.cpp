#include "Assumption.h"

namespace storm {
namespace analysis {
Assumption::Assumption(bool stateAssumption, std::shared_ptr<expressions::ExpressionManager> expressionManager, uint_fast64_t val1, uint_fast64_t val2,
                       expressions::BinaryRelationExpression::RelationType type) {
    this->stateAssumption = stateAssumption;
    expressions::Variable var1 = expressionManager->getVariable(std::to_string(val1));
    expressions::Variable var2 = expressionManager->getVariable(std::to_string(val2));
    assumption = std::make_shared<expressions::BinaryRelationExpression>(
        expressions::BinaryRelationExpression(*expressionManager, expressionManager->getBooleanType(), var1.getExpression().getBaseExpressionPointer(),
                                              var2.getExpression().getBaseExpressionPointer(), type));
}

std::shared_ptr<expressions::BinaryRelationExpression> Assumption::getAssumption() const {
    return assumption;
}

bool Assumption::isStateAssumption() const {
    return stateAssumption;
}

bool Assumption::isActionAssumption() const {
    return !stateAssumption;
}

bool operator<(const storm::analysis::Assumption &assumption1, const storm::analysis::Assumption &assumption2) {
    return assumption1.getAssumption() < assumption2.getAssumption();
}

std::ostream &operator<<(std::ostream &output, const storm::analysis::Assumption &assumption1) {
    if (assumption1.isStateAssumption()) {
        output << "state assumption " << *assumption1.getAssumption();
    } else {
        output << "action assumption " << *assumption1.getAssumption();
    }
    return output;
}

}  // namespace analysis
}  // namespace storm
