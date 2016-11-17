#include "src/logic/FormulaInformationVisitor.h"

#include "src/logic/Formulas.h"

namespace storm {
    namespace logic {
        FormulaInformation FormulaInformationVisitor::getInformation(Formula const& f) const {
            boost::any result = f.accept(*this, boost::any());
            return boost::any_cast<FormulaInformation>(result);
        }
        
        boost::any FormulaInformationVisitor::visit(AtomicExpressionFormula const& f, boost::any const& data) const {
            return FormulaInformation();
        }
        
        boost::any FormulaInformationVisitor::visit(AtomicLabelFormula const& f, boost::any const& data) const {
            return FormulaInformation();
        }
        
        boost::any FormulaInformationVisitor::visit(BinaryBooleanStateFormula const& f, boost::any const& data) const {
            return FormulaInformation();
        }
        
        boost::any FormulaInformationVisitor::visit(BooleanLiteralFormula const& f, boost::any const& data) const {
            return FormulaInformation();
        }
        
        boost::any FormulaInformationVisitor::visit(BoundedUntilFormula const& f, boost::any const& data) const {
            return boost::any_cast<FormulaInformation>(f.getLeftSubformula().accept(*this)).join(boost::any_cast<FormulaInformation>(f.getRightSubformula().accept(*this))).setContainsBoundedUntilFormula();
        }
        
        boost::any FormulaInformationVisitor::visit(ConditionalFormula const& f, boost::any const& data) const {
            return boost::any_cast<FormulaInformation>(f.getSubformula().accept(*this)).join(boost::any_cast<FormulaInformation>(f.getConditionFormula().accept(*this)));
        }
        
        boost::any FormulaInformationVisitor::visit(CumulativeRewardFormula const& f, boost::any const& data) const {
            return FormulaInformation();
        }
        
        boost::any FormulaInformationVisitor::visit(EventuallyFormula const& f, boost::any const& data) const {
            return f.getSubformula().accept(*this);
        }
        
        boost::any FormulaInformationVisitor::visit(TimeOperatorFormula const& f, boost::any const& data) const {
            return f.getSubformula().accept(*this);
        }
        
        boost::any FormulaInformationVisitor::visit(GloballyFormula const& f, boost::any const& data) const {
            return f.getSubformula().accept(*this);
        }
        
        boost::any FormulaInformationVisitor::visit(InstantaneousRewardFormula const& f, boost::any const& data) const {
            return FormulaInformation();
        }
        
        boost::any FormulaInformationVisitor::visit(LongRunAverageOperatorFormula const& f, boost::any const& data) const {
            return f.getSubformula().accept(*this);
        }
        
        boost::any FormulaInformationVisitor::visit(LongRunAverageRewardFormula const& f, boost::any const& data) const {
            return FormulaInformation();
        }
        
        boost::any FormulaInformationVisitor::visit(MultiObjectiveFormula const& f, boost::any const& data) const {
            FormulaInformation result;
            for(auto const& subF : f.getSubformulas()){
                result.join(boost::any_cast<FormulaInformation>(subF->accept(*this)));
            }
            return result;
        }
        
        boost::any FormulaInformationVisitor::visit(NextFormula const& f, boost::any const& data) const {
            return boost::any_cast<FormulaInformation>(f.getSubformula().accept(*this)).setContainsNextFormula();
        }
        
        boost::any FormulaInformationVisitor::visit(ProbabilityOperatorFormula const& f, boost::any const& data) const {
            return f.getSubformula().accept(*this);
        }
        
        boost::any FormulaInformationVisitor::visit(RewardOperatorFormula const& f, boost::any const& data) const {
            return boost::any_cast<FormulaInformation>(f.getSubformula().accept(*this)).setContainsRewardOperator();
        }
        
        boost::any FormulaInformationVisitor::visit(TotalRewardFormula const& f, boost::any const& data) const {
            return FormulaInformation();
        }
        
        boost::any FormulaInformationVisitor::visit(UnaryBooleanStateFormula const& f, boost::any const& data) const {
            return f.getSubformula().accept(*this);
        }
        
        boost::any FormulaInformationVisitor::visit(UntilFormula const& f, boost::any const& data) const {
            return boost::any_cast<FormulaInformation>(f.getLeftSubformula().accept(*this)).join(boost::any_cast<FormulaInformation>(f.getRightSubformula().accept(*this)));
        }

    }
}
