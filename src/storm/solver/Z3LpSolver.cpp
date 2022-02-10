#include "storm/solver/Z3LpSolver.h"

#include <numeric>

#include "storm/storage/expressions/LinearCoefficientVisitor.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/DebugSettings.h"

#include "storm/io/file.h"
#include "storm/storage/expressions/Expression.h"
#include "storm/storage/expressions/ExpressionManager.h"
#include "storm/utility/constants.h"
#include "storm/utility/macros.h"

#include "storm/exceptions/ExpressionEvaluationException.h"
#include "storm/exceptions/InvalidAccessException.h"
#include "storm/exceptions/InvalidArgumentException.h"
#include "storm/exceptions/InvalidStateException.h"
#include "storm/exceptions/NotImplementedException.h"

namespace storm {
namespace solver {

#ifdef STORM_HAVE_Z3_OPTIMIZE

template<typename ValueType>
Z3LpSolver<ValueType>::Z3LpSolver(std::string const& name, OptimizationDirection const& optDir) : LpSolver<ValueType>(optDir), isIncremental(false) {
    z3::config config;
    config.set("model", true);
    context = std::unique_ptr<z3::context>(new z3::context(config));
    solver = std::unique_ptr<z3::optimize>(new z3::optimize(*context));
    expressionAdapter = std::unique_ptr<storm::adapters::Z3ExpressionAdapter>(new storm::adapters::Z3ExpressionAdapter(*this->manager, *context));
}

template<typename ValueType>
Z3LpSolver<ValueType>::Z3LpSolver(std::string const& name) : Z3LpSolver(name, OptimizationDirection::Minimize) {
    // Intentionally left empty.
}

template<typename ValueType>
Z3LpSolver<ValueType>::Z3LpSolver(OptimizationDirection const& optDir) : Z3LpSolver("", optDir) {
    // Intentionally left empty.
}

template<typename ValueType>
Z3LpSolver<ValueType>::Z3LpSolver() : Z3LpSolver("", OptimizationDirection::Minimize) {
    // Intentionally left empty.
}

template<typename ValueType>
Z3LpSolver<ValueType>::~Z3LpSolver() {
    // Intentionally left empty.
}

template<typename ValueType>
void Z3LpSolver<ValueType>::update() const {
    // Since the model changed, we erase the optimality flag.
    lastCheckObjectiveValue.reset(nullptr);
    lastCheckModel.reset(nullptr);
    this->currentModelHasBeenOptimized = false;
}

template<typename ValueType>
storm::expressions::Variable Z3LpSolver<ValueType>::addBoundedContinuousVariable(std::string const& name, ValueType lowerBound, ValueType upperBound,
                                                                                 ValueType objectiveFunctionCoefficient) {
    storm::expressions::Variable newVariable;
    if (isIncremental) {
        newVariable = this->manager->declareOrGetVariable(name, this->manager->getRationalType());
    } else {
        newVariable = this->manager->declareVariable(name, this->manager->getRationalType());
    }
    solver->add(expressionAdapter->translateExpression((newVariable.getExpression() >= this->manager->rational(lowerBound)) &&
                                                       (newVariable.getExpression() <= this->manager->rational(upperBound))));
    if (!storm::utility::isZero(objectiveFunctionCoefficient)) {
        optimizationSummands.push_back(this->manager->rational(objectiveFunctionCoefficient) * newVariable);
    }
    return newVariable;
}

template<typename ValueType>
storm::expressions::Variable Z3LpSolver<ValueType>::addLowerBoundedContinuousVariable(std::string const& name, ValueType lowerBound,
                                                                                      ValueType objectiveFunctionCoefficient) {
    storm::expressions::Variable newVariable;
    if (isIncremental) {
        newVariable = this->manager->declareOrGetVariable(name, this->manager->getRationalType());
    } else {
        newVariable = this->manager->declareVariable(name, this->manager->getRationalType());
    }
    solver->add(expressionAdapter->translateExpression(newVariable.getExpression() >= this->manager->rational(lowerBound)));
    if (!storm::utility::isZero(objectiveFunctionCoefficient)) {
        optimizationSummands.push_back(this->manager->rational(objectiveFunctionCoefficient) * newVariable);
    }
    return newVariable;
}

template<typename ValueType>
storm::expressions::Variable Z3LpSolver<ValueType>::addUpperBoundedContinuousVariable(std::string const& name, ValueType upperBound,
                                                                                      ValueType objectiveFunctionCoefficient) {
    storm::expressions::Variable newVariable;
    if (isIncremental) {
        newVariable = this->manager->declareOrGetVariable(name, this->manager->getRationalType());
    } else {
        newVariable = this->manager->declareVariable(name, this->manager->getRationalType());
    }
    solver->add(expressionAdapter->translateExpression(newVariable.getExpression() <= this->manager->rational(upperBound)));
    if (!storm::utility::isZero(objectiveFunctionCoefficient)) {
        optimizationSummands.push_back(this->manager->rational(objectiveFunctionCoefficient) * newVariable);
    }
    return newVariable;
}

template<typename ValueType>
storm::expressions::Variable Z3LpSolver<ValueType>::addUnboundedContinuousVariable(std::string const& name, ValueType objectiveFunctionCoefficient) {
    storm::expressions::Variable newVariable;
    if (isIncremental) {
        newVariable = this->manager->declareOrGetVariable(name, this->manager->getRationalType());
    } else {
        newVariable = this->manager->declareVariable(name, this->manager->getRationalType());
    }
    if (!storm::utility::isZero(objectiveFunctionCoefficient)) {
        optimizationSummands.push_back(this->manager->rational(objectiveFunctionCoefficient) * newVariable);
    }
    return newVariable;
}

template<typename ValueType>
storm::expressions::Variable Z3LpSolver<ValueType>::addBoundedIntegerVariable(std::string const& name, ValueType lowerBound, ValueType upperBound,
                                                                              ValueType objectiveFunctionCoefficient) {
    storm::expressions::Variable newVariable;
    if (isIncremental) {
        newVariable = this->manager->declareOrGetVariable(name, this->manager->getIntegerType());
    } else {
        newVariable = this->manager->declareVariable(name, this->manager->getIntegerType());
    }
    solver->add(expressionAdapter->translateExpression((newVariable.getExpression() >= this->manager->rational(lowerBound)) &&
                                                       (newVariable.getExpression() <= this->manager->rational(upperBound))));
    if (!storm::utility::isZero(objectiveFunctionCoefficient)) {
        optimizationSummands.push_back(this->manager->rational(objectiveFunctionCoefficient) * newVariable);
    }
    return newVariable;
}

template<typename ValueType>
storm::expressions::Variable Z3LpSolver<ValueType>::addLowerBoundedIntegerVariable(std::string const& name, ValueType lowerBound,
                                                                                   ValueType objectiveFunctionCoefficient) {
    storm::expressions::Variable newVariable;
    if (isIncremental) {
        newVariable = this->manager->declareOrGetVariable(name, this->manager->getIntegerType());
    } else {
        newVariable = this->manager->declareVariable(name, this->manager->getIntegerType());
    }
    solver->add(expressionAdapter->translateExpression(newVariable.getExpression() >= this->manager->rational(lowerBound)));
    if (!storm::utility::isZero(objectiveFunctionCoefficient)) {
        optimizationSummands.push_back(this->manager->rational(objectiveFunctionCoefficient) * newVariable);
    }
    return newVariable;
}

template<typename ValueType>
storm::expressions::Variable Z3LpSolver<ValueType>::addUpperBoundedIntegerVariable(std::string const& name, ValueType upperBound,
                                                                                   ValueType objectiveFunctionCoefficient) {
    storm::expressions::Variable newVariable;
    if (isIncremental) {
        newVariable = this->manager->declareOrGetVariable(name, this->manager->getIntegerType());
    } else {
        newVariable = this->manager->declareVariable(name, this->manager->getIntegerType());
    }
    solver->add(expressionAdapter->translateExpression(newVariable.getExpression() <= this->manager->rational(upperBound)));
    if (!storm::utility::isZero(objectiveFunctionCoefficient)) {
        optimizationSummands.push_back(this->manager->rational(objectiveFunctionCoefficient) * newVariable);
    }
    return newVariable;
}

template<typename ValueType>
storm::expressions::Variable Z3LpSolver<ValueType>::addUnboundedIntegerVariable(std::string const& name, ValueType objectiveFunctionCoefficient) {
    storm::expressions::Variable newVariable;
    if (isIncremental) {
        newVariable = this->manager->declareOrGetVariable(name, this->manager->getIntegerType());
    } else {
        newVariable = this->manager->declareVariable(name, this->manager->getIntegerType());
    }
    if (!storm::utility::isZero(objectiveFunctionCoefficient)) {
        optimizationSummands.push_back(this->manager->rational(objectiveFunctionCoefficient) * newVariable);
    }
    return newVariable;
}

template<typename ValueType>
storm::expressions::Variable Z3LpSolver<ValueType>::addBinaryVariable(std::string const& name, ValueType objectiveFunctionCoefficient) {
    storm::expressions::Variable newVariable;
    if (isIncremental) {
        newVariable = this->manager->declareOrGetVariable(name, this->manager->getIntegerType());
    } else {
        newVariable = this->manager->declareVariable(name, this->manager->getIntegerType());
    }
    solver->add(
        expressionAdapter->translateExpression((newVariable.getExpression() >= this->manager->rational(storm::utility::zero<storm::RationalNumber>())) &&
                                               (newVariable.getExpression() <= this->manager->rational(storm::utility::one<storm::RationalNumber>()))));
    if (!storm::utility::isZero(objectiveFunctionCoefficient)) {
        optimizationSummands.push_back(this->manager->rational(objectiveFunctionCoefficient) * newVariable);
    }
    return newVariable;
}

template<typename ValueType>
void Z3LpSolver<ValueType>::addConstraint(std::string const& name, storm::expressions::Expression const& constraint) {
    STORM_LOG_THROW(constraint.isRelationalExpression(), storm::exceptions::InvalidArgumentException, "Illegal constraint is not a relational expression.");
    STORM_LOG_THROW(constraint.getOperator() != storm::expressions::OperatorType::NotEqual, storm::exceptions::InvalidArgumentException,
                    "Illegal constraint uses inequality operator.");
    solver->add(expressionAdapter->translateExpression(constraint));
}

template<typename ValueType>
void Z3LpSolver<ValueType>::optimize() const {
    // First incorporate all recent changes.
    this->update();

    // Invoke push() as we want to be able to erase the current optimization function after checking
    solver->push();

    // Solve the optimization problem depending on the optimization direction
    storm::expressions::Expression optimizationFunction = storm::expressions::sum(optimizationSummands);
    z3::optimize::handle optFuncHandle = this->getOptimizationDirection() == OptimizationDirection::Minimize
                                             ? solver->minimize(expressionAdapter->translateExpression(optimizationFunction))
                                             : solver->maximize(expressionAdapter->translateExpression(optimizationFunction));
    z3::check_result chkRes = solver->check();
    STORM_LOG_THROW(chkRes != z3::unknown, storm::exceptions::InvalidStateException, "Unable to solve LP problem with Z3: Check result is unknown.");

    // We need to store the resulting information at this point. Otherwise, the information would be lost after calling pop() ...

    // Check feasibility
    lastCheckInfeasible = (chkRes == z3::unsat);
    if (lastCheckInfeasible) {
        lastCheckUnbounded = false;
    } else {
        // Get objective result
        lastCheckObjectiveValue = std::make_unique<z3::expr>(solver->upper(optFuncHandle));
        // Check boundedness
        STORM_LOG_ASSERT(lastCheckObjectiveValue->is_app(), "Failed to convert Z3 expression. Encountered unknown expression type.");
        lastCheckUnbounded = (lastCheckObjectiveValue->decl().decl_kind() != Z3_OP_ANUM);
        if (lastCheckUnbounded) {
            lastCheckObjectiveValue.reset(nullptr);
        } else {
            // Assert that the upper approximation equals the lower one
            STORM_LOG_ASSERT(std::string(Z3_get_numeral_string(*context, *lastCheckObjectiveValue)) ==
                                 std::string(Z3_get_numeral_string(*context, solver->lower(optFuncHandle))),
                             "Lower and Upper Approximation of z3LPSolver result do not match.");
            lastCheckModel = std::make_unique<z3::model>(solver->get_model());
        }
    }

    solver->pop();  // removes current optimization function
    this->currentModelHasBeenOptimized = true;
}

template<typename ValueType>
bool Z3LpSolver<ValueType>::isInfeasible() const {
    STORM_LOG_THROW(this->currentModelHasBeenOptimized, storm::exceptions::InvalidStateException,
                    "Illegal call to Z3LpSolver<ValueType>::isInfeasible: model has not been optimized.");
    return lastCheckInfeasible;
}

template<typename ValueType>
bool Z3LpSolver<ValueType>::isUnbounded() const {
    STORM_LOG_THROW(this->currentModelHasBeenOptimized, storm::exceptions::InvalidStateException,
                    "Illegal call to Z3LpSolver<ValueType>::isUnbounded: model has not been optimized.");
    return lastCheckUnbounded;
}

template<typename ValueType>
bool Z3LpSolver<ValueType>::isOptimal() const {
    STORM_LOG_THROW(this->currentModelHasBeenOptimized, storm::exceptions::InvalidStateException,
                    "Illegal call to Z3LpSolver<ValueType>::isOptimal: model has not been optimized.");
    return !lastCheckInfeasible && !lastCheckUnbounded;
}

template<typename ValueType>
storm::expressions::Expression Z3LpSolver<ValueType>::getValue(storm::expressions::Variable const& variable) const {
    STORM_LOG_ASSERT(variable.getManager() == this->getManager(), "Requested variable is managed by a different manager.");
    if (!this->isOptimal()) {
        STORM_LOG_THROW(!this->isInfeasible(), storm::exceptions::InvalidAccessException, "Unable to get Z3 solution from infeasible model.");
        STORM_LOG_THROW(!this->isUnbounded(), storm::exceptions::InvalidAccessException, "Unable to get Z3 solution from unbounded model.");
        STORM_LOG_THROW(false, storm::exceptions::InvalidAccessException, "Unable to get Z3 solution from unoptimized model.");
    }
    STORM_LOG_ASSERT(lastCheckModel, "Model has not been stored.");

    z3::expr z3Var = this->expressionAdapter->translateExpression(variable);
    return this->expressionAdapter->translateExpression(lastCheckModel->eval(z3Var, true));
}

template<typename ValueType>
ValueType Z3LpSolver<ValueType>::getContinuousValue(storm::expressions::Variable const& variable) const {
    storm::expressions::Expression value = getValue(variable);
    if (value.getBaseExpression().isIntegerLiteralExpression()) {
        return storm::utility::convertNumber<ValueType>(value.getBaseExpression().asIntegerLiteralExpression().getValue());
    }
    STORM_LOG_THROW(value.getBaseExpression().isRationalLiteralExpression(), storm::exceptions::ExpressionEvaluationException,
                    "Expected a rational literal while obtaining the value of a continuous variable. Got " << value << "instead.");
    return storm::utility::convertNumber<ValueType>(value.getBaseExpression().asRationalLiteralExpression().getValue());
}

template<typename ValueType>
int_fast64_t Z3LpSolver<ValueType>::getIntegerValue(storm::expressions::Variable const& variable) const {
    storm::expressions::Expression value = getValue(variable);
    STORM_LOG_THROW(value.getBaseExpression().isIntegerLiteralExpression(), storm::exceptions::ExpressionEvaluationException,
                    "Expected an integer literal while obtaining the value of an integer variable. Got " << value << "instead.");
    return value.getBaseExpression().asIntegerLiteralExpression().getValue();
}

template<typename ValueType>
bool Z3LpSolver<ValueType>::getBinaryValue(storm::expressions::Variable const& variable) const {
    storm::expressions::Expression value = getValue(variable);
    // Binary variables are in fact represented as integer variables!
    STORM_LOG_THROW(value.getBaseExpression().isIntegerLiteralExpression(), storm::exceptions::ExpressionEvaluationException,
                    "Expected an integer literal while obtaining the value of a binary variable. Got " << value << "instead.");
    int_fast64_t val = value.getBaseExpression().asIntegerLiteralExpression().getValue();
    STORM_LOG_THROW((val == 0 || val == 1), storm::exceptions::ExpressionEvaluationException,
                    "Tried to get a binary value for a variable that is neither 0 nor 1.");
    return val == 1;
}

template<typename ValueType>
ValueType Z3LpSolver<ValueType>::getObjectiveValue() const {
    if (!this->isOptimal()) {
        STORM_LOG_THROW(!this->isInfeasible(), storm::exceptions::InvalidAccessException, "Unable to get Z3 solution from infeasible model.");
        STORM_LOG_THROW(!this->isUnbounded(), storm::exceptions::InvalidAccessException, "Unable to get Z3 solution from unbounded model.");
        STORM_LOG_THROW(false, storm::exceptions::InvalidAccessException, "Unable to get Z3 solution from unoptimized model.");
    }
    STORM_LOG_ASSERT(lastCheckObjectiveValue, "Objective value has not been stored.");

    storm::expressions::Expression result = this->expressionAdapter->translateExpression(*lastCheckObjectiveValue);
    if (result.getBaseExpression().isIntegerLiteralExpression()) {
        return storm::utility::convertNumber<ValueType>(result.getBaseExpression().asIntegerLiteralExpression().getValue());
    }
    STORM_LOG_THROW(result.getBaseExpression().isRationalLiteralExpression(), storm::exceptions::ExpressionEvaluationException,
                    "Expected a rational literal while obtaining the objective result. Got " << result << "instead.");
    return storm::utility::convertNumber<ValueType>(result.getBaseExpression().asRationalLiteralExpression().getValue());
}

template<typename ValueType>
void Z3LpSolver<ValueType>::writeModelToFile(std::string const& filename) const {
    std::ofstream stream;
    storm::utility::openFile(filename, stream);
    stream << Z3_optimize_to_string(*context, *solver);
    storm::utility::closeFile(stream);
}

template<typename ValueType>
void Z3LpSolver<ValueType>::push() {
    incrementaOptimizationSummandIndicators.push_back(optimizationSummands.size());
    solver->push();
}

template<typename ValueType>
void Z3LpSolver<ValueType>::pop() {
    STORM_LOG_ASSERT(!incrementaOptimizationSummandIndicators.empty(), "Tried to pop() without push()ing first.");
    solver->pop();
    // Delete summands of the optimization function that have been added since the last call to push()
    optimizationSummands.resize(incrementaOptimizationSummandIndicators.back());
    incrementaOptimizationSummandIndicators.pop_back();
    isIncremental = true;
}

template<typename ValueType>
void Z3LpSolver<ValueType>::setMaximalMILPGap(ValueType const&, bool) {
    // Since the solver is always exact, setting a gap has no effect.
    // Intentionally left empty.
}

template<typename ValueType>
ValueType Z3LpSolver<ValueType>::getMILPGap(bool relative) const {
    // Since the solver is precise, the milp gap is always zero.
    return storm::utility::zero<ValueType>();
}
#else
template<typename ValueType>
Z3LpSolver<ValueType>::Z3LpSolver(std::string const&, OptimizationDirection const&) {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without Z3 or the version of Z3 does not support optimization. "
                                                          "Yet, a method was called that requires this support.";
}

template<typename ValueType>
Z3LpSolver<ValueType>::Z3LpSolver(std::string const&) {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without Z3 or the version of Z3 does not support optimization. "
                                                          "Yet, a method was called that requires this support.";
}

template<typename ValueType>
Z3LpSolver<ValueType>::Z3LpSolver(OptimizationDirection const&) {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without Z3 or the version of Z3 does not support optimization. "
                                                          "Yet, a method was called that requires this support.";
}

template<typename ValueType>
Z3LpSolver<ValueType>::Z3LpSolver() {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without Z3 or the version of Z3 does not support optimization. "
                                                          "Yet, a method was called that requires this support.";
}

template<typename ValueType>
Z3LpSolver<ValueType>::~Z3LpSolver() {}

template<typename ValueType>
storm::expressions::Variable Z3LpSolver<ValueType>::addBoundedContinuousVariable(std::string const&, ValueType, ValueType, ValueType) {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without Z3 or the version of Z3 does not support optimization. "
                                                          "Yet, a method was called that requires this support.";
}

template<typename ValueType>
storm::expressions::Variable Z3LpSolver<ValueType>::addLowerBoundedContinuousVariable(std::string const&, ValueType, ValueType) {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without Z3 or the version of Z3 does not support optimization. "
                                                          "Yet, a method was called that requires this support.";
}

template<typename ValueType>
storm::expressions::Variable Z3LpSolver<ValueType>::addUpperBoundedContinuousVariable(std::string const&, ValueType, ValueType) {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without Z3 or the version of Z3 does not support optimization. "
                                                          "Yet, a method was called that requires this support.";
}

template<typename ValueType>
storm::expressions::Variable Z3LpSolver<ValueType>::addUnboundedContinuousVariable(std::string const&, ValueType) {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without Z3 or the version of Z3 does not support optimization. "
                                                          "Yet, a method was called that requires this support.";
}

template<typename ValueType>
storm::expressions::Variable Z3LpSolver<ValueType>::addBoundedIntegerVariable(std::string const&, ValueType, ValueType, ValueType) {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without Z3 or the version of Z3 does not support optimization. "
                                                          "Yet, a method was called that requires this support.";
}

template<typename ValueType>
storm::expressions::Variable Z3LpSolver<ValueType>::addLowerBoundedIntegerVariable(std::string const&, ValueType, ValueType) {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without Z3 or the version of Z3 does not support optimization. "
                                                          "Yet, a method was called that requires this support.";
}

template<typename ValueType>
storm::expressions::Variable Z3LpSolver<ValueType>::addUpperBoundedIntegerVariable(std::string const&, ValueType, ValueType) {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without Z3 or the version of Z3 does not support optimization. "
                                                          "Yet, a method was called that requires this support.";
}

template<typename ValueType>
storm::expressions::Variable Z3LpSolver<ValueType>::addUnboundedIntegerVariable(std::string const&, ValueType) {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without Z3 or the version of Z3 does not support optimization. "
                                                          "Yet, a method was called that requires this support.";
}

template<typename ValueType>
storm::expressions::Variable Z3LpSolver<ValueType>::addBinaryVariable(std::string const&, ValueType) {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without Z3 or the version of Z3 does not support optimization. "
                                                          "Yet, a method was called that requires this support.";
}

template<typename ValueType>
void Z3LpSolver<ValueType>::update() const {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without Z3 or the version of Z3 does not support optimization. "
                                                          "Yet, a method was called that requires this support.";
}

template<typename ValueType>
void Z3LpSolver<ValueType>::addConstraint(std::string const&, storm::expressions::Expression const&) {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without Z3 or the version of Z3 does not support optimization. "
                                                          "Yet, a method was called that requires this support.";
}

template<typename ValueType>
void Z3LpSolver<ValueType>::optimize() const {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without Z3 or the version of Z3 does not support optimization. "
                                                          "Yet, a method was called that requires this support.";
}

template<typename ValueType>
bool Z3LpSolver<ValueType>::isInfeasible() const {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without Z3 or the version of Z3 does not support optimization. "
                                                          "Yet, a method was called that requires this support.";
}

template<typename ValueType>
bool Z3LpSolver<ValueType>::isUnbounded() const {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without Z3 or the version of Z3 does not support optimization. "
                                                          "Yet, a method was called that requires this support.";
}

template<typename ValueType>
bool Z3LpSolver<ValueType>::isOptimal() const {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without Z3 or the version of Z3 does not support optimization. "
                                                          "Yet, a method was called that requires this support.";
}

template<typename ValueType>
storm::expressions::Expression Z3LpSolver<ValueType>::getValue(storm::expressions::Variable const& variable) const {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without Z3 or the version of Z3 does not support optimization. "
                                                          "Yet, a method was called that requires this support.";
}

template<typename ValueType>
ValueType Z3LpSolver<ValueType>::getContinuousValue(storm::expressions::Variable const&) const {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without Z3 or the version of Z3 does not support optimization. "
                                                          "Yet, a method was called that requires this support.";
}

template<typename ValueType>
int_fast64_t Z3LpSolver<ValueType>::getIntegerValue(storm::expressions::Variable const&) const {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without Z3 or the version of Z3 does not support optimization. "
                                                          "Yet, a method was called that requires this support.";
}

template<typename ValueType>
bool Z3LpSolver<ValueType>::getBinaryValue(storm::expressions::Variable const&) const {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without Z3 or the version of Z3 does not support optimization. "
                                                          "Yet, a method was called that requires this support.";
}

template<typename ValueType>
ValueType Z3LpSolver<ValueType>::getObjectiveValue() const {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without Z3 or the version of Z3 does not support optimization. "
                                                          "Yet, a method was called that requires this support.";
}

template<typename ValueType>
void Z3LpSolver<ValueType>::writeModelToFile(std::string const& filename) const {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without Z3 or the version of Z3 does not support optimization. "
                                                          "Yet, a method was called that requires this support.";
}

template<typename ValueType>
void Z3LpSolver<ValueType>::push() {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without Z3 or the version of Z3 does not support optimization. "
                                                          "Yet, a method was called that requires this support.";
}

template<typename ValueType>
void Z3LpSolver<ValueType>::pop() {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without Z3 or the version of Z3 does not support optimization. "
                                                          "Yet, a method was called that requires this support.";
}

template<typename ValueType>
void Z3LpSolver<ValueType>::setMaximalMILPGap(ValueType const& gap, bool relative) {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without Z3 or the version of Z3 does not support optimization. "
                                                          "Yet, a method was called that requires this support.";
}

template<typename ValueType>
ValueType Z3LpSolver<ValueType>::getMILPGap(bool relative) const {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without Z3 or the version of Z3 does not support optimization. "
                                                          "Yet, a method was called that requires this support.";
}
#endif

template class Z3LpSolver<double>;
template class Z3LpSolver<storm::RationalNumber>;
}  // namespace solver
}  // namespace storm
