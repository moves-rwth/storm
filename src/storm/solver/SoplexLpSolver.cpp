#include "storm/solver/SoplexLpSolver.h"

#include <numeric>

#include "storm/storage/expressions/LinearCoefficientVisitor.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/DebugSettings.h"

#include "storm/utility/constants.h"
#include "storm/utility/macros.h"
#include "storm/utility/vector.h"

#include "storm/storage/expressions/Expression.h"
#include "storm/storage/expressions/ExpressionManager.h"

#include "storm/exceptions/InvalidAccessException.h"
#include "storm/exceptions/InvalidArgumentException.h"
#include "storm/exceptions/InvalidStateException.h"
#include "storm/exceptions/NotImplementedException.h"
#include "storm/exceptions/NotSupportedException.h"



namespace storm {
namespace solver {

#ifdef STORM_HAVE_SOPLEX

using namespace soplex;

soplex::Rational to_soplex_rational(storm::RationalNumber const& in) {
    return soplex::Rational(storm::utility::convertNumber<GmpRationalNumber>(in).get_mpq_t());
}

storm::RationalNumber from_soplex_rational(soplex::Rational const& r) {
    return storm::utility::convertNumber<storm::RationalNumber>(GmpRationalNumber(r.backend().data()));
}

template<typename ValueType>
SoplexLpSolver<ValueType>::SoplexLpSolver(std::string const& name, OptimizationDirection const& optDir)
    : LpSolver<ValueType>(optDir) {
    if (std::is_same_v<ValueType, storm::RationalNumber>) {
        solver.setIntParam(SoPlex::READMODE, SoPlex::READMODE_RATIONAL);
        solver.setIntParam(SoPlex::SOLVEMODE, SoPlex::SOLVEMODE_RATIONAL);
        solver.setIntParam(SoPlex::CHECKMODE, SoPlex::CHECKMODE_RATIONAL);
        solver.setIntParam(SoPlex::SYNCMODE, SoPlex::SYNCMODE_AUTO);
        solver.setRealParam(SoPlex::FEASTOL, 0.0);
        solver.setRealParam(SoPlex::OPTTOL, 0.0);
    }
    solver.setIntParam(SoPlex::VERBOSITY, 0);
}

template<typename ValueType>
SoplexLpSolver<ValueType>::SoplexLpSolver(std::string const& name)
    : SoplexLpSolver(name, OptimizationDirection::Minimize) {
    // Intentionally left empty.
}

template<typename ValueType>
SoplexLpSolver<ValueType>::SoplexLpSolver(OptimizationDirection const& optDir)
    : SoplexLpSolver("", optDir) {
    // Intentionally left empty.
}


template<typename ValueType>
SoplexLpSolver<ValueType>::~SoplexLpSolver() {

}

template<typename ValueType>
void SoplexLpSolver<ValueType>::update() const {
    // Nothing to be done.
}

template<typename ValueType>
storm::expressions::Variable SoplexLpSolver<ValueType>::addBoundedContinuousVariable(std::string const& name, ValueType lowerBound, ValueType upperBound,
                                                                                     ValueType objectiveFunctionCoefficient) {
    storm::expressions::Variable newVariable = this->manager->declareOrGetVariable(name, this->manager->getRationalType());
    this->addVariable(newVariable, lowerBound, upperBound, objectiveFunctionCoefficient);
    return newVariable;

}

template<typename ValueType>
storm::expressions::Variable SoplexLpSolver<ValueType>::addLowerBoundedContinuousVariable(std::string const& name, ValueType lowerBound,
                                                                                          ValueType objectiveFunctionCoefficient) {
    storm::expressions::Variable newVariable = this->manager->declareOrGetVariable(name, this->manager->getRationalType());
    this->addVariable(newVariable, storm::utility::convertNumber<double>(lowerBound), soplex::infinity, objectiveFunctionCoefficient);
    return newVariable;
}

template<typename ValueType>
storm::expressions::Variable SoplexLpSolver<ValueType>::addUpperBoundedContinuousVariable(std::string const& name, ValueType upperBound,
                                                                                          ValueType objectiveFunctionCoefficient) {
    storm::expressions::Variable newVariable = this->manager->declareOrGetVariable(name, this->manager->getRationalType());
    this->addVariable(newVariable, -soplex::infinity, storm::utility::convertNumber<double>(upperBound), objectiveFunctionCoefficient);
    return newVariable;
}

template<typename ValueType>
storm::expressions::Variable SoplexLpSolver<ValueType>::addUnboundedContinuousVariable(std::string const& name, ValueType objectiveFunctionCoefficient) {
    storm::expressions::Variable newVariable = this->manager->declareOrGetVariable(name, this->manager->getRationalType());
    this->addVariable(newVariable, -soplex::infinity, soplex::infinity, objectiveFunctionCoefficient);
    return newVariable;
}

template<>
void SoplexLpSolver<double>::addVariable(storm::expressions::Variable const& variable, double const& lowerBound, double const& upperBound, double const& objectiveFunctionCoefficient) {
    // Assert whether the variable does not exist yet.
    // Due to potential incremental usage (push(), pop()), a variable might be declared in the manager but not in the lp model.
    STORM_LOG_ASSERT(variableToIndexMap.count(variable) == 0, "Variable " << variable.getName() << " exists already in the model.");
    this->variableToIndexMap.emplace(variable, nextVariableIndex);
    ++nextVariableIndex;
    solver.addColReal(soplex::LPColReal(objectiveFunctionCoefficient, variables, upperBound, lowerBound));
}


template<typename ValueType>
void SoplexLpSolver<ValueType>::addVariable(storm::expressions::Variable const& variable, ValueType const& lowerBound, ValueType const& upperBound, ValueType const& objectiveFunctionCoefficient) {
    // Assert whether the variable does not exist yet.
    // Due to potential incremental usage (push(), pop()), a variable might be declared in the manager but not in the lp model.
    STORM_LOG_ASSERT(variableToIndexMap.count(variable) == 0, "Variable " << variable.getName() << " exists already in the model.");
    this->variableToIndexMap.emplace(variable, nextVariableIndex);
    ++nextVariableIndex;
    solver.addColRational(soplex::LPColRational(to_soplex_rational(objectiveFunctionCoefficient), variables, to_soplex_rational(upperBound), to_soplex_rational(lowerBound)));
}


template<typename VT, typename VectType>
void addRowToSolver(soplex::SoPlex& solver, VT const& lconst,   VectType const& row, VT const& rconst);

template<>
void addRowToSolver<double, soplex::DSVector>(soplex::SoPlex& solver, double const& lconst, soplex::DSVector const& row, double const& rconst) {
    solver.addRowReal(LPRow(lconst, row, rconst));
}

template<typename VT,typename VectType>
void addRowToSolver(soplex::SoPlex& solver, storm::RationalNumber const& lconst, VectType const& row, storm::RationalNumber const& rconst) {
    solver.addRowRational(LPRowRational(to_soplex_rational(lconst), row, to_soplex_rational(rconst)));
}

template<typename ValueType>
void SoplexLpSolver<ValueType>::addConstraint(std::string const& name, storm::expressions::Expression const& constraint) {
    STORM_LOG_TRACE("Adding constraint " << (name == "" ? std::to_string(nextConstraintIndex) : name) << " to SoplexLpSolver:\n"
                                         << "\t" << constraint);
    STORM_LOG_THROW(constraint.isRelationalExpression(), storm::exceptions::InvalidArgumentException, "Illegal constraint is not a relational expression.");
    STORM_LOG_THROW(constraint.getOperator() != storm::expressions::OperatorType::NotEqual, storm::exceptions::InvalidArgumentException,
                    "Illegal constraint uses inequality operator.");

    storm::expressions::LinearCoefficientVisitor::VariableCoefficients leftCoefficients =
        storm::expressions::LinearCoefficientVisitor().getLinearCoefficients(constraint.getOperand(0));
    storm::expressions::LinearCoefficientVisitor::VariableCoefficients rightCoefficients =
        storm::expressions::LinearCoefficientVisitor().getLinearCoefficients(constraint.getOperand(1));
    leftCoefficients.separateVariablesFromConstantPart(rightCoefficients);

    // Now we need to transform the coefficients to the vector representation.
    TypedDSVector row(leftCoefficients.size());
    for (auto const& variableCoefficientPair : leftCoefficients) {
        auto variableIndexPair = this->variableToIndexMap.find(variableCoefficientPair.first);
        row.add(variableIndexPair->second, leftCoefficients.getCoefficient(variableIndexPair->first));
    }

    // Determine the type of the constraint and add it properly.
    switch (constraint.getOperator()) {
        case storm::expressions::OperatorType::Less:
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "SoPlex only supports nonstrict inequalities");
            break;
        case storm::expressions::OperatorType::LessOrEqual:
            addRowToSolver<ValueType>(solver, -soplex::infinity, row, rightCoefficients.getConstantPart());
            break;
        case storm::expressions::OperatorType::Greater:
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "SoPlex only supports nonstrict inequalities");
            break;
        case storm::expressions::OperatorType::GreaterOrEqual:
            addRowToSolver<ValueType>(solver, rightCoefficients.getConstantPart(), row, soplex::infinity);
            break;
        case storm::expressions::OperatorType::Equal:
            addRowToSolver<ValueType>(solver, rightCoefficients.getConstantPart(), row, rightCoefficients.getConstantPart());
            break;
        default:
            STORM_LOG_ASSERT(false, "Illegal operator in LP solver constraint.");
    }
}

template<typename ValueType>
void SoplexLpSolver<ValueType>::optimize() const {
    // First incorporate all recent changes.
    this->update();
    //
    if(this->getOptimizationDirection() == storm::solver::OptimizationDirection::Minimize) {
        solver.setIntParam(SoPlex::OBJSENSE, SoPlex::OBJSENSE_MINIMIZE);
    } else {
        solver.setIntParam(SoPlex::OBJSENSE, SoPlex::OBJSENSE_MAXIMIZE);
    }

    status = solver.optimize();
    this->currentModelHasBeenOptimized = true;
}

template<typename ValueType>
bool SoplexLpSolver<ValueType>::isInfeasible() const {
    if (!this->currentModelHasBeenOptimized) {
        throw storm::exceptions::InvalidStateException() << "Illegal call to SoplexLpSolver<ValueType>::isInfeasible: model has not been optimized.";
    }

    return (status == soplex::SPxSolver::INFEASIBLE);
}

template<typename ValueType>
bool SoplexLpSolver<ValueType>::isUnbounded() const {
    if (!this->currentModelHasBeenOptimized) {
        throw storm::exceptions::InvalidStateException() << "Illegal call to SoplexLpSolver<ValueType>::isUnbounded: model has not been optimized.";
    }


    return (status == soplex::SPxSolver::UNBOUNDED);
}

template<typename ValueType>
bool SoplexLpSolver<ValueType>::isOptimal() const {
    if (!this->currentModelHasBeenOptimized) {
        return false;
    }
    return (status == soplex::SPxSolver::OPTIMAL);
}

template<>
double SoplexLpSolver<double>::getContinuousValue(storm::expressions::Variable const& variable) const {
    ensureSolved();

    auto variableIndexPair = this->variableToIndexMap.find(variable);
    STORM_LOG_THROW(variableIndexPair != this->variableToIndexMap.end(), storm::exceptions::InvalidAccessException,
                    "Accessing value of unknown variable '" << variable.getName() << "'.");
    STORM_LOG_ASSERT(variableIndexPair->second < nextVariableIndex, "Variable Index exceeds highest value.");
    if (primalSolution.dim() == 0) {
        primalSolution = TypedDVector(nextVariableIndex);
        solver.getPrimal(primalSolution);
    }
    return primalSolution[variableIndexPair->second];
}

template<typename ValueType>
ValueType SoplexLpSolver<ValueType>::getContinuousValue(storm::expressions::Variable const& variable) const {
    ensureSolved();

    auto variableIndexPair = this->variableToIndexMap.find(variable);
    STORM_LOG_THROW(variableIndexPair != this->variableToIndexMap.end(), storm::exceptions::InvalidAccessException,
                    "Accessing value of unknown variable '" << variable.getName() << "'.");
    STORM_LOG_ASSERT(variableIndexPair->second < nextVariableIndex, "Variable Index exceeds highest value.");
    if (primalSolution.dim() == 0) {
        primalSolution = TypedDVector(nextVariableIndex);
        solver.getPrimalRational(primalSolution);
    }
    return storm::utility::convertNumber<ValueType>(from_soplex_rational(primalSolution[variableIndexPair->second]));
}

template<>
double SoplexLpSolver<double>::getObjectiveValue() const {
    ensureSolved();
    return solver.objValueReal();
}

template<typename ValueType>
ValueType SoplexLpSolver<ValueType>::getObjectiveValue() const {
    ensureSolved();
    return storm::utility::convertNumber<ValueType>(from_soplex_rational(solver.objValueRational()));
}

template<typename ValueType>
void SoplexLpSolver<ValueType>::ensureSolved() const {
    if (!this->isOptimal()) {
        STORM_LOG_THROW(!this->isInfeasible(), storm::exceptions::InvalidAccessException, "Unable to get Soplex solution from infeasible model.");
        STORM_LOG_THROW(!this->isUnbounded(), storm::exceptions::InvalidAccessException, "Unable to get Soplex solution from unbounded model.");
        STORM_LOG_THROW(false, storm::exceptions::InvalidAccessException, "Unable to get Soplex solution from unoptimized model.");
    }
}


template<typename ValueType>
void SoplexLpSolver<ValueType>::writeModelToFile(std::string const& filename) const {
    solver.writeFileReal(filename.c_str(), NULL, NULL, NULL);
}

template<typename ValueType>
void SoplexLpSolver<ValueType>::push() {
    STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Push/Pop not supported on SoPlex");
}

template<typename ValueType>
void SoplexLpSolver<ValueType>::pop() {
    STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Push/Pop not supported on SoPlex");
}

#else
template<typename ValueType>
SoplexLpSolver<ValueType>::SoplexLpSolver(std::string const&, OptimizationDirection const&) {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for Soplex. Yet, a method was called that "
                                                          "requires this support. Please choose a version of support with Soplex support.";
}

template<typename ValueType>
SoplexLpSolver<ValueType>::SoplexLpSolver(std::string const&) {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for Soplex. Yet, a method was called that "
                                                          "requires this support. Please choose a version of support with Soplex support.";
}

template<typename ValueType>
SoplexLpSolver<ValueType>::SoplexLpSolver(OptimizationDirection const&) {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for Soplex. Yet, a method was called that "
                                                          "requires this support. Please choose a version of support with Soplex support.";
}

template<typename ValueType>
SoplexLpSolver<ValueType>::SoplexLpSolver() {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for Soplex. Yet, a method was called that "
                                                          "requires this support. Please choose a version of support with Soplex support.";
}

template<typename ValueType>
SoplexLpSolver<ValueType>::~SoplexLpSolver() {}

template<typename ValueType>
storm::expressions::Variable SoplexLpSolver<ValueType>::addBoundedContinuousVariable(std::string const&, ValueType, ValueType, ValueType) {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for Soplex. Yet, a method was called that "
                                                          "requires this support. Please choose a version of support with Soplex support.";
}

template<typename ValueType>
storm::expressions::Variable SoplexLpSolver<ValueType>::addLowerBoundedContinuousVariable(std::string const&, ValueType, ValueType) {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for Soplex. Yet, a method was called that "
                                                          "requires this support. Please choose a version of support with Soplex support.";
}

template<typename ValueType>
storm::expressions::Variable SoplexLpSolver<ValueType>::addUpperBoundedContinuousVariable(std::string const&, ValueType, ValueType) {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for Soplex. Yet, a method was called that "
                                                          "requires this support. Please choose a version of support with Soplex support.";
}

template<typename ValueType>
storm::expressions::Variable SoplexLpSolver<ValueType>::addUnboundedContinuousVariable(std::string const&, ValueType) {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for Soplex. Yet, a method was called that "
                                                          "requires this support. Please choose a version of support with Soplex support.";
}


template<typename ValueType>
void SoplexLpSolver<ValueType>::update() const {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for Soplex. Yet, a method was called that "
                                                          "requires this support. Please choose a version of support with Soplex support.";
}

template<typename ValueType>
void SoplexLpSolver<ValueType>::addConstraint(std::string const&, storm::expressions::Expression const&) {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for Soplex. Yet, a method was called that "
                                                          "requires this support. Please choose a version of support with Soplex support.";
}

template<typename ValueType>
void SoplexLpSolver<ValueType>::optimize() const {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for Soplex. Yet, a method was called that "
                                                          "requires this support. Please choose a version of support with Soplex support.";
}

template<typename ValueType>
bool SoplexLpSolver<ValueType>::isInfeasible() const {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for Soplex. Yet, a method was called that "
                                                          "requires this support. Please choose a version of support with Soplex support.";
}

template<typename ValueType>
bool SoplexLpSolver<ValueType>::isUnbounded() const {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for Soplex. Yet, a method was called that "
                                                          "requires this support. Please choose a version of support with Soplex support.";
}

template<typename ValueType>
bool SoplexLpSolver<ValueType>::isOptimal() const {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for Soplex. Yet, a method was called that "
                                                          "requires this support. Please choose a version of support with Soplex support.";
}

template<typename ValueType>
ValueType SoplexLpSolver<ValueType>::getContinuousValue(storm::expressions::Variable const&) const {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for Soplex. Yet, a method was called that "
                                                          "requires this support. Please choose a version of support with Soplex support.";
}

template<typename ValueType>
ValueType SoplexLpSolver<ValueType>::getObjectiveValue() const {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for Soplex. Yet, a method was called that "
                                                          "requires this support. Please choose a version of support with Soplex support.";
}

template<typename ValueType>
void SoplexLpSolver<ValueType>::writeModelToFile(std::string const&) const {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for Soplex. Yet, a method was called that "
                                                          "requires this support. Please choose a version of support with Soplex support.";
}

template<typename ValueType>
void SoplexLpSolver<ValueType>::push() {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for Soplex. Yet, a method was called that "
                                                          "requires this support. Please choose a version of support with Soplex support.";
}

template<typename ValueType>
void SoplexLpSolver<ValueType>::pop() {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for Soplex. Yet, a method was called that "
                                                          "requires this support. Please choose a version of support with Soplex support.";
}
#endif



template<typename ValueType>
storm::expressions::Variable SoplexLpSolver<ValueType>::addBoundedIntegerVariable(std::string const& name, ValueType lowerBound, ValueType upperBound,
                                                                                  ValueType objectiveFunctionCoefficient) {
    STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "SoPlex does not support integers");
}

template<typename ValueType>
storm::expressions::Variable SoplexLpSolver<ValueType>::addLowerBoundedIntegerVariable(std::string const& name, ValueType lowerBound,
                                                                                       ValueType objectiveFunctionCoefficient) {
    STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "SoPlex does not support integers");
}

template<typename ValueType>
storm::expressions::Variable SoplexLpSolver<ValueType>::addUpperBoundedIntegerVariable(std::string const& name, ValueType upperBound,
                                                                                       ValueType objectiveFunctionCoefficient) {
    STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "SoPlex does not support integers");
}

template<typename ValueType>
storm::expressions::Variable SoplexLpSolver<ValueType>::addUnboundedIntegerVariable(std::string const& name, ValueType objectiveFunctionCoefficient) {
    STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "SoPlex does not support integers");
}

template<typename ValueType>
storm::expressions::Variable SoplexLpSolver<ValueType>::addBinaryVariable(std::string const& name, ValueType objectiveFunctionCoefficient) {
    STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "SoPlex does not support binary variables");
}

template<typename ValueType>
int_fast64_t SoplexLpSolver<ValueType>::getIntegerValue(storm::expressions::Variable const& variable) const {
    STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "SoPlex does not support integer variables");
}

template<typename ValueType>
bool SoplexLpSolver<ValueType>::getBinaryValue(storm::expressions::Variable const& variable) const {
    STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "SoPlex does not support binary variables");
}


template<typename ValueType>
void SoplexLpSolver<ValueType>::setMaximalMILPGap(ValueType const& gap, bool relative) {
    STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "SoPlex does not support integer variables.");
}

template<typename ValueType>
ValueType SoplexLpSolver<ValueType>::getMILPGap(bool relative) const {
    STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "SoPlex does not support integer variables.");
}



template class SoplexLpSolver<double>;
template class SoplexLpSolver<storm::RationalNumber>;
}  // namespace solver
}  // namespace storm
