#ifndef STORM_LOGIC_MULTIOBJECTIVEFORMULA_H_
#define STORM_LOGIC_MULTIOBJECTIVEFORMULA_H_

#include "storm/logic/StateFormula.h"

namespace storm {
namespace logic {
class MultiObjectiveFormula : public StateFormula {
   public:
    MultiObjectiveFormula(std::vector<std::shared_ptr<Formula const>> const& subformulas);

    virtual ~MultiObjectiveFormula();

    virtual bool isMultiObjectiveFormula() const override;

    virtual bool hasQualitativeResult() const override;   // Result is true or false
    virtual bool hasQuantitativeResult() const override;  // Result is numerical or a pareto curve
    virtual bool hasNumericalResult() const;              // Result is numerical
    virtual bool hasParetoCurveResult() const;            // Result is a pareto curve

    Formula const& getSubformula(uint_fast64_t index) const;
    uint_fast64_t getNumberOfSubformulas() const;
    std::vector<std::shared_ptr<Formula const>> const& getSubformulas() const;

    virtual boost::any accept(FormulaVisitor const& visitor, boost::any const& data) const override;
    virtual void gatherAtomicExpressionFormulas(std::vector<std::shared_ptr<AtomicExpressionFormula const>>& atomicExpressionFormulas) const override;
    virtual void gatherAtomicLabelFormulas(std::vector<std::shared_ptr<AtomicLabelFormula const>>& atomicLabelFormulas) const override;
    virtual void gatherReferencedRewardModels(std::set<std::string>& referencedRewardModels) const override;

    virtual std::ostream& writeToStream(std::ostream& out, bool allowParentheses = false) const override;

   private:
    std::vector<std::shared_ptr<Formula const>> subformulas;
};
}  // namespace logic
}  // namespace storm

#endif /* STORM_LOGIC_MULTIOBJECTIVEFORMULA_H_ */
