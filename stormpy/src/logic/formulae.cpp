#include "formulae.h"

#include "src/common.h"
#include "src/logic/Formulas.h"

void define_formulae(py::module& m) {
 
    py::enum_<storm::logic::ComparisonType>(m, "ComparisonType")
        .value("LESS", storm::logic::ComparisonType::Less)
        .value("LEQ", storm::logic::ComparisonType::LessEqual)
        .value("GREATER", storm::logic::ComparisonType::Greater)
        .value("GEQ", storm::logic::ComparisonType::GreaterEqual)
    ;

    /*py::class_<std::vector<std::shared_ptr<storm::logic::Formula>>, void, void>("FormulaVec", "Vector of formulas")
        .def(vector_indexing_suite<std::vector<std::shared_ptr<storm::logic::Formula>>, true>())
    ;*/
    
    py::class_<storm::logic::Formula, std::shared_ptr<storm::logic::Formula const>>(m, "Formula", "Generic Storm Formula")
        .def("__str__", &storm::logic::Formula::toString)
    ;

    // Path Formulae
    py::class_<storm::logic::PathFormula, std::shared_ptr<storm::logic::PathFormula const>>(m, "PathFormula", "Formula about the probability of a set of paths in an automaton", py::base<storm::logic::Formula>());
    py::class_<storm::logic::UnaryPathFormula, std::shared_ptr<storm::logic::UnaryPathFormula const>>(m, "UnaryPathFormula", "Path formula with one operand", py::base<storm::logic::PathFormula>());
    py::class_<storm::logic::EventuallyFormula, std::shared_ptr<storm::logic::EventuallyFormula const>>(m, "EventuallyFormula", "Formula for eventually", py::base<storm::logic::UnaryPathFormula>());
    py::class_<storm::logic::GloballyFormula, std::shared_ptr<storm::logic::GloballyFormula const>>(m, "GloballyFormula", "Formula for globally", py::base<storm::logic::UnaryPathFormula>());
    py::class_<storm::logic::BinaryPathFormula, std::shared_ptr<storm::logic::BinaryPathFormula const>>(m, "BinaryPathFormula", "Path formula with two operands", py::base<storm::logic::PathFormula>());
    py::class_<storm::logic::BoundedUntilFormula, std::shared_ptr<storm::logic::BoundedUntilFormula const>>(m, "BoundedUntilFormula", "Until Formula with either a step or a time bound.", py::base<storm::logic::BinaryPathFormula>());
    py::class_<storm::logic::ConditionalFormula, std::shared_ptr<storm::logic::ConditionalFormula const>>(m, "ConditionalFormula", "Formula with the right hand side being a condition.", py::base<storm::logic::Formula>());
    py::class_<storm::logic::UntilFormula, std::shared_ptr<storm::logic::UntilFormula const>>(m, "UntilFormula", "Path Formula for unbounded until", py::base<storm::logic::BinaryPathFormula>());

    // Reward Path Formulae
    //py::class_<storm::logic::RewardPathFormula, std::shared_ptr<storm::logic::RewardPathFormula>(m, "RewardPathFormula", "Formula about the rewards of a set of paths in an automaton", py::base<storm::logic::Formula>());
    py::class_<storm::logic::CumulativeRewardFormula, std::shared_ptr<storm::logic::CumulativeRewardFormula const>>(m, "CumulativeRewardFormula", "Summed rewards over a the paths", py::base<storm::logic::PathFormula>());
    py::class_<storm::logic::InstantaneousRewardFormula, std::shared_ptr<storm::logic::InstantaneousRewardFormula const>>(m ,"InstantaneousRewardFormula", "Instantaneous reward", py::base<storm::logic::PathFormula>());
    py::class_<storm::logic::LongRunAverageRewardFormula, std::shared_ptr<storm::logic::LongRunAverageRewardFormula const>>(m, "LongRunAverageRewardFormula", "Long run average reward", py::base<storm::logic::PathFormula>());
    //py::class_<storm::logic::ReachabilityRewardFormula, std::shared_ptr<storm::logic::ReachabilityRewardFormula>>(m, "ReachabilityRewardFormula", "Reachability reward", py::base<storm::logic::RewardPathFormula>());


    // State Formulae
    py::class_<storm::logic::StateFormula, std::shared_ptr<storm::logic::StateFormula const>>(m, "StateFormula", "Formula about a state of an automaton", py::base<storm::logic::Formula>());
    py::class_<storm::logic::AtomicExpressionFormula, std::shared_ptr<storm::logic::AtomicExpressionFormula const>>(m, "AtomicExpressionFormula", "Formula with an atomic expression", py::base<storm::logic::StateFormula>());
    py::class_<storm::logic::AtomicLabelFormula, std::shared_ptr<storm::logic::AtomicLabelFormula const>>(m, "AtomicLabelFormula", "Formula with an atomic label", py::base<storm::logic::StateFormula>());
    py::class_<storm::logic::BooleanLiteralFormula, std::shared_ptr<storm::logic::BooleanLiteralFormula const>>(m, "BooleanLiteralFormula", "Formula with a boolean literal", py::base<storm::logic::StateFormula>());
    py::class_<storm::logic::UnaryStateFormula, std::shared_ptr<storm::logic::UnaryStateFormula const>>(m, "UnaryStateFormula", "State formula with one operand", py::base<storm::logic::StateFormula>());
    py::class_<storm::logic::UnaryBooleanStateFormula, std::shared_ptr<storm::logic::UnaryBooleanStateFormula const>>(m, "UnaryBooleanStateFormula", "Unary boolean state formula", py::base<storm::logic::UnaryStateFormula>());
    py::class_<storm::logic::OperatorFormula, std::shared_ptr<storm::logic::OperatorFormula const>>(m, "OperatorFormula", "Operator formula", py::base<storm::logic::UnaryStateFormula>())
        .def("has_bound", &storm::logic::OperatorFormula::hasBound, "Check if formula is bounded")
        .def_property("threshold", &storm::logic::OperatorFormula::getThreshold, &storm::logic::OperatorFormula::setThreshold, "Threshold of bound")
        .def_property("comparison_type", &storm::logic::OperatorFormula::getComparisonType, &storm::logic::OperatorFormula::setComparisonType, "Comparison type of bound")
    ;
    py::class_<storm::logic::TimeOperatorFormula, std::shared_ptr<storm::logic::TimeOperatorFormula const>>(m, "TimeOperator", "The time operator", py::base<storm::logic::OperatorFormula>());
    py::class_<storm::logic::LongRunAverageOperatorFormula, std::shared_ptr<storm::logic::LongRunAverageOperatorFormula const>>(m, "LongRunAvarageOperator", "Long run average operator", py::base<storm::logic::OperatorFormula>());
    py::class_<storm::logic::ProbabilityOperatorFormula, std::shared_ptr<storm::logic::ProbabilityOperatorFormula const>>(m, "ProbabilityOperator", "Probability operator", py::base<storm::logic::OperatorFormula>());
    py::class_<storm::logic::RewardOperatorFormula, std::shared_ptr<storm::logic::RewardOperatorFormula const>>(m, "RewardOperator", "Reward operator", py::base<storm::logic::OperatorFormula>());
    py::class_<storm::logic::BinaryStateFormula, std::shared_ptr<storm::logic::BinaryStateFormula const>>(m, "BinaryStateFormula", "State formula with two operands", py::base<storm::logic::StateFormula>());
    py::class_<storm::logic::BinaryBooleanStateFormula, std::shared_ptr<storm::logic::BinaryBooleanStateFormula const>>(m, "BooleanBinaryStateFormula", "Boolean binary state formula", py::base<storm::logic::BinaryStateFormula>());
}
