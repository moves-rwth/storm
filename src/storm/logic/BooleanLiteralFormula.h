#ifndef STORM_LOGIC_BOOLEANLITERALFORMULA_H_
#define STORM_LOGIC_BOOLEANLITERALFORMULA_H_

#include "storm/logic/StateFormula.h"

namespace storm {
namespace logic {
class BooleanLiteralFormula : public StateFormula {
   public:
    BooleanLiteralFormula(bool value);

    virtual ~BooleanLiteralFormula() {
        // Intentionally left empty.
    }

    virtual bool isBooleanLiteralFormula() const override;
    virtual bool isTrueFormula() const override;
    virtual bool isFalseFormula() const override;

    virtual boost::any accept(FormulaVisitor const& visitor, boost::any const& data) const override;

    virtual std::ostream& writeToStream(std::ostream& out, bool allowParentheses = false) const override;

   private:
    bool value;
};
}  // namespace logic
}  // namespace storm

#endif /* STORM_LOGIC_BOOLEANLITERALFORMULA_H_ */
