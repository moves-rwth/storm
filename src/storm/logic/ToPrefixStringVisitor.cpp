#include "storm/logic/ToPrefixStringVisitor.h"

#include "storm/logic/Formulas.h"

#include "storm/exceptions/InvalidOperationException.h"
#include "storm/utility/macros.h"

namespace storm {
namespace logic {

std::string ToPrefixStringVisitor::toPrefixString(Formula const& f) const {
    boost::any result = f.accept(*this, boost::any());
    return boost::any_cast<std::string>(result);
}

boost::any ToPrefixStringVisitor::visit(AtomicExpressionFormula const&, boost::any const&) const {
    STORM_LOG_THROW(false, storm::exceptions::InvalidOperationException, "Can not convert to prefix string");
}

boost::any ToPrefixStringVisitor::visit(AtomicLabelFormula const& f, boost::any const&) const {
    return std::string("\"" + f.getLabel() + "\"");
}

boost::any ToPrefixStringVisitor::visit(BinaryBooleanStateFormula const& f, boost::any const& data) const {
    std::string left = boost::any_cast<std::string>(f.getLeftSubformula().accept(*this, data));
    std::string right = boost::any_cast<std::string>(f.getRightSubformula().accept(*this, data));
    switch (f.getOperator()) {
        case BinaryBooleanStateFormula::OperatorType::And:
            return std::string("& ") + left + " " + right;
            break;
        case BinaryBooleanStateFormula::OperatorType::Or:
            return std::string("| ") + left + " " + right;
            break;
    }
    return boost::any();
}

boost::any ToPrefixStringVisitor::visit(BinaryBooleanPathFormula const& f, boost::any const& data) const {
    std::string left = boost::any_cast<std::string>(f.getLeftSubformula().accept(*this, data));
    std::string right = boost::any_cast<std::string>(f.getRightSubformula().accept(*this, data));
    switch (f.getOperator()) {
        case BinaryBooleanPathFormula::OperatorType::And:
            return std::string("& ") + left + " " + right;
            break;
        case BinaryBooleanPathFormula::OperatorType::Or:
            return std::string("| ") + left + " " + right;
            break;
    }
    return boost::any();
}

boost::any ToPrefixStringVisitor::visit(BooleanLiteralFormula const& f, boost::any const&) const {
    storm::expressions::Expression result;
    if (f.isTrueFormula()) {
        return std::string("t");
    } else {
        return std::string("f");
    }
    return result;
}

boost::any ToPrefixStringVisitor::visit(BoundedUntilFormula const&, boost::any const&) const {
    STORM_LOG_THROW(false, storm::exceptions::InvalidOperationException, "Can not convert to prefix string");
}

boost::any ToPrefixStringVisitor::visit(ConditionalFormula const&, boost::any const&) const {
    STORM_LOG_THROW(false, storm::exceptions::InvalidOperationException, "Can not convert to prefix string");
}

boost::any ToPrefixStringVisitor::visit(CumulativeRewardFormula const&, boost::any const&) const {
    STORM_LOG_THROW(false, storm::exceptions::InvalidOperationException, "Can not convert to prefix string");
}

boost::any ToPrefixStringVisitor::visit(EventuallyFormula const& f, boost::any const& data) const {
    std::string subexpression = boost::any_cast<std::string>(f.getSubformula().accept(*this, data));
    return std::string("F ") + subexpression;
}

boost::any ToPrefixStringVisitor::visit(TimeOperatorFormula const&, boost::any const&) const {
    STORM_LOG_THROW(false, storm::exceptions::InvalidOperationException, "Can not convert to prefix string");
}

boost::any ToPrefixStringVisitor::visit(GloballyFormula const& f, boost::any const& data) const {
    std::string subexpression = boost::any_cast<std::string>(f.getSubformula().accept(*this, data));
    return std::string("G ") + subexpression;
}

boost::any ToPrefixStringVisitor::visit(GameFormula const&, boost::any const&) const {
    STORM_LOG_THROW(false, storm::exceptions::InvalidOperationException, "Can not convert to prefix string");
}

boost::any ToPrefixStringVisitor::visit(InstantaneousRewardFormula const&, boost::any const&) const {
    STORM_LOG_THROW(false, storm::exceptions::InvalidOperationException, "Can not convert to prefix string");
}

boost::any ToPrefixStringVisitor::visit(LongRunAverageOperatorFormula const&, boost::any const&) const {
    STORM_LOG_THROW(false, storm::exceptions::InvalidOperationException, "Can not convert to prefix string");
}

boost::any ToPrefixStringVisitor::visit(LongRunAverageRewardFormula const&, boost::any const&) const {
    STORM_LOG_THROW(false, storm::exceptions::InvalidOperationException, "Can not convert to prefix string");
}

boost::any ToPrefixStringVisitor::visit(MultiObjectiveFormula const&, boost::any const&) const {
    STORM_LOG_THROW(false, storm::exceptions::InvalidOperationException, "Can not convert to prefix string");
}

boost::any ToPrefixStringVisitor::visit(QuantileFormula const&, boost::any const&) const {
    STORM_LOG_THROW(false, storm::exceptions::InvalidOperationException, "Can not convert to prefix string");
}

boost::any ToPrefixStringVisitor::visit(NextFormula const& f, boost::any const& data) const {
    std::string subexpression = boost::any_cast<std::string>(f.getSubformula().accept(*this, data));
    return std::string("X ") + subexpression;
}

boost::any ToPrefixStringVisitor::visit(ProbabilityOperatorFormula const&, boost::any const&) const {
    STORM_LOG_THROW(false, storm::exceptions::InvalidOperationException, "Can not convert to prefix string");
}

boost::any ToPrefixStringVisitor::visit(RewardOperatorFormula const&, boost::any const&) const {
    STORM_LOG_THROW(false, storm::exceptions::InvalidOperationException, "Can not convert to prefix string");
}

boost::any ToPrefixStringVisitor::visit(TotalRewardFormula const&, boost::any const&) const {
    STORM_LOG_THROW(false, storm::exceptions::InvalidOperationException, "Can not convert to prefix string");
}

boost::any ToPrefixStringVisitor::visit(UnaryBooleanStateFormula const& f, boost::any const& data) const {
    std::string subexpression = boost::any_cast<std::string>(f.getSubformula().accept(*this, data));
    switch (f.getOperator()) {
        case UnaryBooleanStateFormula::OperatorType::Not:
            return std::string("! ") + subexpression;
            break;
    }
    return boost::any();
}

boost::any ToPrefixStringVisitor::visit(UnaryBooleanPathFormula const& f, boost::any const& data) const {
    std::string subexpression = boost::any_cast<std::string>(f.getSubformula().accept(*this, data));
    switch (f.getOperator()) {
        case UnaryBooleanPathFormula::OperatorType::Not:
            return std::string("! ") + subexpression;
            break;
    }
    return boost::any();
}

boost::any ToPrefixStringVisitor::visit(UntilFormula const& f, boost::any const& data) const {
    std::string left = boost::any_cast<std::string>(f.getLeftSubformula().accept(*this, data));
    std::string right = boost::any_cast<std::string>(f.getRightSubformula().accept(*this, data));
    return std::string("U ") + left + " " + right;
}

boost::any ToPrefixStringVisitor::visit(HOAPathFormula const&, boost::any const&) const {
    STORM_LOG_THROW(false, storm::exceptions::InvalidOperationException, "Can not convert to prefix string");
}
}  // namespace logic
}  // namespace storm
