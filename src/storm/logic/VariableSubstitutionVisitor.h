#ifndef STORM_LOGIC_VARIABLESUBSTITUTIONVISITOR_H_
#define STORM_LOGIC_VARIABLESUBSTITUTIONVISITOR_H_

#include <map>

#include "storm/logic/CloneVisitor.h"

#include "storm/storage/expressions/Expression.h"

namespace storm {
    namespace logic {
        
        class VariableSubstitutionVisitor : public CloneVisitor {
        public:
            VariableSubstitutionVisitor(std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution);
            
            std::shared_ptr<Formula> substitute(Formula const& f) const;
            

            virtual boost::any visit(TimeOperatorFormula const& f, boost::any const& data) const override;
            virtual boost::any visit(LongRunAverageOperatorFormula const& f, boost::any const& data) const override;
            virtual boost::any visit(ProbabilityOperatorFormula const& f, boost::any const& data) const override;
            virtual boost::any visit(RewardOperatorFormula const& f, boost::any const& data) const override;
            virtual boost::any visit(BoundedUntilFormula const& f, boost::any const& data) const override;
            virtual boost::any visit(CumulativeRewardFormula const& f, boost::any const& data) const override;
            virtual boost::any visit(InstantaneousRewardFormula const& f, boost::any const& data) const override;
            virtual boost::any visit(AtomicExpressionFormula const& f, boost::any const& data) const override;
            
        private:
            
            OperatorInformation substituteOperatorInformation(OperatorInformation const& operatorInformation) const;
            
            std::map<storm::expressions::Variable, storm::expressions::Expression> const& substitution;
        };
        
    }
}


#endif /* STORM_LOGIC_VARIABLESUBSTITUTIONVISITOR_H_ */
