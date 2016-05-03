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
    
    py::class_<std::shared_ptr<storm::logic::Formula>>(m, "Formula", "Generic Storm Formula")
        .def("__str__", &storm::logic::Formula::toString)
    ;

    // Path Formulae
    py::class_<std::shared_ptr<storm::logic::PathFormula>>(m, "PathFormula", "Formula about the probability of a set of paths in an automaton", py::base<std::shared_ptr<storm::logic::Formula>>());
    //py::class_<storm::logic::UnaryPathFormula, storm::logic::PathFormula>(m, "UnaryPathFormula", "Path formula with one operand");
    //py::class_<storm::logic::EventuallyFormula, storm::logic::UnaryPathFormula>(m, "EventuallyFormula", "Formula for eventually");
    //py::class_<storm::logic::GloballyFormula, storm::logic::UnaryPathFormula>(m, "GloballyFormula", "Formula for globally");
    //py::class_<storm::logic::BinaryPathFormula, storm::logic::PathFormula>(m, "BinaryPathFormula", "Path formula with two operands");
    //py::class_<storm::logic::BoundedUntilFormula, storm::logic::BinaryPathFormula>(m, "BoundedUntilFormula", "Until Formula with either a step or a time bound.");
    //py::class_<storm::logic::ConditionalPathFormula, storm::logic::BinaryPathFormula>(m, "ConditionalPathFormula", "Path Formula with the right hand side being a condition.");
    //py::class_<storm::logic::UntilFormula, storm::logic::BinaryPathFormula>(m, "UntilFormula", "Path Formula for unbounded until");

/*
    //
    // Reward Path Formulae
    //
    defineClass<storm::logic::RewardPathFormula, storm::logic::Formula, boost::noncopyable>("RewardPathFormula",
    "Formula about the rewards of a set of paths in an automaton");
    defineClass<storm::logic::CumulativeRewardFormula, storm::logic::RewardPathFormula>("CumulativeRewardFormula",
    "Summed rewards over a the paths");
    defineClass<storm::logic::InstantaneousRewardFormula, storm::logic::RewardPathFormula>("InstanteneousRewardFormula",
    "");
    defineClass<storm::logic::LongRunAverageRewardFormula, storm::logic::RewardPathFormula>("LongRunAverageRewardFormula",
    "");
    defineClass<storm::logic::ReachabilityRewardFormula, storm::logic::RewardPathFormula>("ReachabilityRewardFormula",
    "");


    //
    // State Formulae
    //
    defineClass<storm::logic::StateFormula, storm::logic::Formula, boost::noncopyable>("StateFormula",
    "Formula about a state of an automaton");
    defineClass<storm::logic::AtomicExpressionFormula, storm::logic::StateFormula>("AtomicExpressionFormula",
    "");
    defineClass<storm::logic::AtomicLabelFormula, storm::logic::StateFormula>("AtomicLabelFormula",
    "");
    defineClass<storm::logic::BooleanLiteralFormula, storm::logic::StateFormula>("BooleanLiteralFormula",
    "");
    defineClass<storm::logic::UnaryStateFormula, storm::logic::StateFormula, boost::noncopyable>("UnaryStateFormula",
    "State formula with one operand");
    defineClass<storm::logic::UnaryBooleanStateFormula, storm::logic::UnaryStateFormula>("UnaryBooleanStateFormula",
    "");
    defineClass<storm::logic::OperatorFormula, storm::logic::UnaryStateFormula, boost::noncopyable>("OperatorFormula",
    "")
        .add_property("has_bound", &storm::logic::OperatorFormula::hasBound)
        .add_property("bound", &storm::logic::OperatorFormula::getBound, &storm::logic::OperatorFormula::setBound)
        .add_property("comparison_type", &storm::logic::OperatorFormula::getComparisonType, &storm::logic::OperatorFormula::setComparisonType)
    ;
    defineClass<storm::logic::ExpectedTimeOperatorFormula, storm::logic::OperatorFormula>("ExpectedTimeOperator",
    "The expected time between two events");
    defineClass<storm::logic::LongRunAverageOperatorFormula, storm::logic::OperatorFormula>("LongRunAvarageOperator",
    "");
    defineClass<storm::logic::ProbabilityOperatorFormula, storm::logic::OperatorFormula>("ProbabilityOperator",
    "");
    defineClass<storm::logic::RewardOperatorFormula, storm::logic::OperatorFormula>("RewardOperatorFormula",
    "");
    defineClass<storm::logic::BinaryStateFormula, storm::logic::StateFormula, boost::noncopyable>("BinaryStateFormula",
    "State formula with two operands");
    defineClass<storm::logic::BinaryBooleanStateFormula, storm::logic::BinaryStateFormula>("BooleanBinaryStateFormula",
    "");
    */
}
