#include "storm/logic/FormulaInformationVisitor.h"

#include "storm/logic/Formulas.h"

namespace storm {
    namespace logic {
        FormulaInformation FormulaInformationVisitor::getInformation(Formula const& f) const {
            boost::any result = f.accept(*this, boost::any());
            return boost::any_cast<FormulaInformation>(result);
        }
        
        boost::any FormulaInformationVisitor::visit(AtomicExpressionFormula const&, boost::any const&) const {
            return FormulaInformation();
        }
        
        boost::any FormulaInformationVisitor::visit(AtomicLabelFormula const&, boost::any const&) const {
            return FormulaInformation();
        }
        
        boost::any FormulaInformationVisitor::visit(BinaryBooleanStateFormula const&, boost::any const&) const {
            return FormulaInformation();
        }
        
        boost::any FormulaInformationVisitor::visit(BooleanLiteralFormula const&, boost::any const&) const {
            return FormulaInformation();
        }
        
        boost::any FormulaInformationVisitor::visit(BoundedUntilFormula const& f, boost::any const& data) const {
            FormulaInformation result;
            result.setContainsBoundedUntilFormula(true);
            for (unsigned i = 0; i < f.getDimension(); ++i) {
                if (f.getTimeBoundReference(i).isRewardBound()) {
                    result.setContainsRewardBoundedFormula(true);
                }
            }
            
            if (f.hasMultiDimensionalSubformulas()) {
                for (unsigned i = 0; i < f.getDimension(); ++i) {
                    result.join(boost::any_cast<FormulaInformation>(f.getLeftSubformula(i).accept(*this, data)));
                    result.join(boost::any_cast<FormulaInformation>(f.getRightSubformula(i).accept(*this, data)));
                }
            } else {
                result.join(boost::any_cast<FormulaInformation>(f.getLeftSubformula().accept(*this, data)));
                result.join(boost::any_cast<FormulaInformation>(f.getRightSubformula().accept(*this, data)));
            }
            return result;
        }
        
        boost::any FormulaInformationVisitor::visit(ConditionalFormula const& f, boost::any const& data) const {
            return boost::any_cast<FormulaInformation>(f.getSubformula().accept(*this, data)).join(boost::any_cast<FormulaInformation>(f.getConditionFormula().accept(*this)));
        }
        
        boost::any FormulaInformationVisitor::visit(CumulativeRewardFormula const& f, boost::any const&) const {
            FormulaInformation result;
            result.setContainsCumulativeRewardFormula(true);
            for (unsigned i = 0; i < f.getDimension(); ++i) {
                if (f.getTimeBoundReference(i).isRewardBound()) {
                    result.setContainsRewardBoundedFormula(true);
                }
            }
            return result;
        }
        
        boost::any FormulaInformationVisitor::visit(EventuallyFormula const& f, boost::any const& data) const {
            return f.getSubformula().accept(*this, data);
        }
        
        boost::any FormulaInformationVisitor::visit(TimeOperatorFormula const& f, boost::any const& data) const {
            return f.getSubformula().accept(*this, data);
        }
        
        boost::any FormulaInformationVisitor::visit(GloballyFormula const& f, boost::any const& data) const {
            return f.getSubformula().accept(*this, data);
        }
        
        boost::any FormulaInformationVisitor::visit(InstantaneousRewardFormula const&, boost::any const&) const {
            return FormulaInformation();
        }
        
        boost::any FormulaInformationVisitor::visit(LongRunAverageOperatorFormula const& f, boost::any const& data) const {
            FormulaInformation result;
            result.setContainsLongRunFormula(true);
            result.join(boost::any_cast<FormulaInformation>(f.getSubformula().accept(*this, data)));
            return result;
        }
        
        boost::any FormulaInformationVisitor::visit(LongRunAverageRewardFormula const&, boost::any const&) const {
            FormulaInformation result;
            result.setContainsLongRunFormula(true);
            return result;
        }
        
        boost::any FormulaInformationVisitor::visit(MultiObjectiveFormula const& f, boost::any const& data) const {
            FormulaInformation result;
            for(auto const& subF : f.getSubformulas()){
                result.join(boost::any_cast<FormulaInformation>(subF->accept(*this, data)));
            }
            return result;
        }
        
        boost::any FormulaInformationVisitor::visit(QuantileFormula const& f, boost::any const& data) const {
            return f.getSubformula().accept(*this, data);
        }
        
        boost::any FormulaInformationVisitor::visit(NextFormula const& f, boost::any const& data) const {
            return boost::any_cast<FormulaInformation>(f.getSubformula().accept(*this, data)).setContainsNextFormula();
        }
        
        boost::any FormulaInformationVisitor::visit(ProbabilityOperatorFormula const& f, boost::any const& data) const {
            return f.getSubformula().accept(*this, data);
        }
        
        boost::any FormulaInformationVisitor::visit(RewardOperatorFormula const& f, boost::any const& data) const {
            return boost::any_cast<FormulaInformation>(f.getSubformula().accept(*this, data)).setContainsRewardOperator();
        }
        
        boost::any FormulaInformationVisitor::visit(TotalRewardFormula const&, boost::any const&) const {
            return FormulaInformation();
        }
        
        boost::any FormulaInformationVisitor::visit(UnaryBooleanStateFormula const& f, boost::any const& data) const {
            return f.getSubformula().accept(*this, data);
        }
        
        boost::any FormulaInformationVisitor::visit(UntilFormula const& f, boost::any const& data) const {
            return boost::any_cast<FormulaInformation>(f.getLeftSubformula().accept(*this, data)).join(boost::any_cast<FormulaInformation>(f.getRightSubformula().accept(*this)));
        }

    }
}
