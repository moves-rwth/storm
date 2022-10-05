#pragma once
#include "storm/storage/expressions/BinaryRelationExpression.h"
#include "storm/storage/expressions/ExpressionManager.h"

namespace storm {
namespace analysis {
class Assumption {
   public:
    Assumption(bool stateAssumption, std::shared_ptr<expressions::ExpressionManager> expressionManager, uint_fast64_t val1, uint_fast64_t val2,
               expressions::BinaryRelationExpression::RelationType type);
    std::shared_ptr<expressions::BinaryRelationExpression> getAssumption() const;
    bool isStateAssumption() const;
    bool isActionAssumption() const;

   private:
    std::shared_ptr<expressions::BinaryRelationExpression> assumption;
    bool stateAssumption;
};
std::ostream &operator<<(std::ostream &output, const Assumption &c);
bool operator<(const Assumption &c1, const Assumption &c2);
}  // namespace analysis
}  // namespace storm
