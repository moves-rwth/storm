#include "storm/solver/GurobiLpSolver.h"

#include <numeric>

#include "storm/storage/expressions/LinearCoefficientVisitor.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/DebugSettings.h"
#include "storm/settings/modules/GurobiSettings.h"

#include "storm/utility/constants.h"
#include "storm/utility/macros.h"
#include "storm/utility/vector.h"

#include "storm/storage/expressions/Expression.h"
#include "storm/storage/expressions/ExpressionManager.h"

#include "storm/exceptions/InvalidAccessException.h"
#include "storm/exceptions/InvalidArgumentException.h"
#include "storm/exceptions/InvalidStateException.h"
#include "storm/exceptions/NotImplementedException.h"

namespace storm {
namespace solver {

GurobiEnvironment::~GurobiEnvironment() {
#ifdef STORM_HAVE_GUROBI
    if (initialized) {
        GRBfreeenv(env);
    }
#endif
}

#ifdef STORM_HAVE_GUROBI
GRBenv* GurobiEnvironment::operator*() {
    STORM_LOG_ASSERT(initialized, "Gurobi Environment has not been initialized");
    return env;
}
#endif

void GurobiEnvironment::initialize() {
#ifdef STORM_HAVE_GUROBI
    // Create the environment.
    int error = GRBloadenv(&env, "");
    if (error || env == nullptr) {
        STORM_LOG_ERROR("Could not initialize Gurobi (" << GRBgeterrormsg(env) << ", error code " << error << ").");
        throw storm::exceptions::InvalidStateException()
            << "Could not initialize Gurobi environment (" << GRBgeterrormsg(env) << ", error code " << error << ").";
    }
    setOutput(storm::settings::getModule<storm::settings::modules::DebugSettings>().isDebugSet() ||
              storm::settings::getModule<storm::settings::modules::GurobiSettings>().isOutputSet());

    error = GRBsetintparam(env, "Method", static_cast<int>(storm::settings::getModule<storm::settings::modules::GurobiSettings>().getMethod()));
    STORM_LOG_THROW(error == 0, storm::exceptions::InvalidStateException,
                    "Unable to set Gurobi Parameter Method (" << GRBgeterrormsg(env) << ", error code " << error << ").");

    // Enable the following line to restrict Gurobi to one thread only.
    error = GRBsetintparam(env, "Threads", storm::settings::getModule<storm::settings::modules::GurobiSettings>().getNumberOfThreads());
    STORM_LOG_THROW(error == 0, storm::exceptions::InvalidStateException,
                    "Unable to set Gurobi Parameter Threads (" << GRBgeterrormsg(env) << ", error code " << error << ").");

    error = GRBsetintparam(env, "MIPFocus", storm::settings::getModule<storm::settings::modules::GurobiSettings>().getMIPFocus());
    STORM_LOG_THROW(error == 0, storm::exceptions::InvalidStateException,
                    "Unable to set Gurobi Parameter MIPFocus (" << GRBgeterrormsg(env) << ", error code " << error << ").");

    error = GRBsetintparam(env, "ConcurrentMIP", storm::settings::getModule<storm::settings::modules::GurobiSettings>().getNumberOfConcurrentMipThreads());
    STORM_LOG_THROW(error == 0, storm::exceptions::InvalidStateException,
                    "Unable to set Gurobi Parameter ConcurrentMIP (" << GRBgeterrormsg(env) << ", error code " << error << ").");

    // Enable the following line to force Gurobi to be as precise about the binary variables as required by the given precision option.
    error = GRBsetdblparam(env, "IntFeasTol", storm::settings::getModule<storm::settings::modules::GurobiSettings>().getIntegerTolerance());
    STORM_LOG_THROW(error == 0, storm::exceptions::InvalidStateException,
                    "Unable to set Gurobi Parameter IntFeasTol (" << GRBgeterrormsg(env) << ", error code " << error << ").");

    initialized = true;
#endif
}

void GurobiEnvironment::setOutput(bool set) {
#ifdef STORM_HAVE_GUROBI
    int error = GRBsetintparam(env, "OutputFlag", set);
    STORM_LOG_THROW(error == 0, storm::exceptions::InvalidStateException,
                    "Unable to set Gurobi Parameter OutputFlag (" << GRBgeterrormsg(env) << ", error code " << error << ").");
#else
    (void)set;
#endif
}

#ifdef STORM_HAVE_GUROBI

template<typename ValueType>
GurobiLpSolver<ValueType>::GurobiLpSolver(std::shared_ptr<GurobiEnvironment> const& environment, std::string const& name, OptimizationDirection const& optDir)
    : LpSolver<ValueType>(optDir), environment(environment), model(nullptr), nextVariableIndex(0), nextConstraintIndex(0) {
    // Create the model.
    int error = 0;
    error = GRBnewmodel(**environment, &model, name.c_str(), 0, nullptr, nullptr, nullptr, nullptr, nullptr);
    if (error) {
        STORM_LOG_ERROR("Could not initialize Gurobi model (" << GRBgeterrormsg(**environment) << ", error code " << error << ").");
        throw storm::exceptions::InvalidStateException()
            << "Could not initialize Gurobi model (" << GRBgeterrormsg(**environment) << ", error code " << error << ").";
    }
}

template<typename ValueType>
GurobiLpSolver<ValueType>::GurobiLpSolver(std::shared_ptr<GurobiEnvironment> const& environment, std::string const& name)
    : GurobiLpSolver(environment, name, OptimizationDirection::Minimize) {
    // Intentionally left empty.
}

template<typename ValueType>
GurobiLpSolver<ValueType>::GurobiLpSolver(std::shared_ptr<GurobiEnvironment> const& environment, OptimizationDirection const& optDir)
    : GurobiLpSolver(environment, "", optDir) {
    // Intentionally left empty.
}

template<typename ValueType>
GurobiLpSolver<ValueType>::GurobiLpSolver(std::shared_ptr<GurobiEnvironment> const& environment)
    : GurobiLpSolver(environment, "", OptimizationDirection::Minimize) {
    // Intentionally left empty.
}

template<typename ValueType>
GurobiLpSolver<ValueType>::~GurobiLpSolver() {
    // Dispose of the objects allocated inside Gurobi.
    GRBfreemodel(model);
}

template<typename ValueType>
void GurobiLpSolver<ValueType>::update() const {
    int error = GRBupdatemodel(model);
    STORM_LOG_THROW(error == 0, storm::exceptions::InvalidStateException,
                    "Unable to update Gurobi model (" << GRBgeterrormsg(**environment) << ", error code " << error << ").");

    // Since the model changed, we erase the optimality flag.
    this->currentModelHasBeenOptimized = false;
}

template<typename ValueType>
storm::expressions::Variable GurobiLpSolver<ValueType>::addBoundedContinuousVariable(std::string const& name, ValueType lowerBound, ValueType upperBound,
                                                                                     ValueType objectiveFunctionCoefficient) {
    storm::expressions::Variable newVariable = this->manager->declareOrGetVariable(name, this->manager->getRationalType());
    this->addVariable(newVariable, GRB_CONTINUOUS, storm::utility::convertNumber<double>(lowerBound), storm::utility::convertNumber<double>(upperBound),
                      objectiveFunctionCoefficient);
    return newVariable;
}

template<typename ValueType>
storm::expressions::Variable GurobiLpSolver<ValueType>::addLowerBoundedContinuousVariable(std::string const& name, ValueType lowerBound,
                                                                                          ValueType objectiveFunctionCoefficient) {
    storm::expressions::Variable newVariable = this->manager->declareOrGetVariable(name, this->manager->getRationalType());
    this->addVariable(newVariable, GRB_CONTINUOUS, storm::utility::convertNumber<double>(lowerBound), GRB_INFINITY, objectiveFunctionCoefficient);
    return newVariable;
}

template<typename ValueType>
storm::expressions::Variable GurobiLpSolver<ValueType>::addUpperBoundedContinuousVariable(std::string const& name, ValueType upperBound,
                                                                                          ValueType objectiveFunctionCoefficient) {
    storm::expressions::Variable newVariable = this->manager->declareOrGetVariable(name, this->manager->getRationalType());
    this->addVariable(newVariable, GRB_CONTINUOUS, -GRB_INFINITY, storm::utility::convertNumber<double>(upperBound), objectiveFunctionCoefficient);
    return newVariable;
}

template<typename ValueType>
storm::expressions::Variable GurobiLpSolver<ValueType>::addUnboundedContinuousVariable(std::string const& name, ValueType objectiveFunctionCoefficient) {
    storm::expressions::Variable newVariable = this->manager->declareOrGetVariable(name, this->manager->getRationalType());
    this->addVariable(newVariable, GRB_CONTINUOUS, -GRB_INFINITY, GRB_INFINITY, objectiveFunctionCoefficient);
    return newVariable;
}

template<typename ValueType>
storm::expressions::Variable GurobiLpSolver<ValueType>::addBoundedIntegerVariable(std::string const& name, ValueType lowerBound, ValueType upperBound,
                                                                                  ValueType objectiveFunctionCoefficient) {
    storm::expressions::Variable newVariable = this->manager->declareOrGetVariable(name, this->manager->getIntegerType());
    this->addVariable(newVariable, GRB_INTEGER, storm::utility::convertNumber<double>(lowerBound), storm::utility::convertNumber<double>(upperBound),
                      objectiveFunctionCoefficient);
    return newVariable;
}

template<typename ValueType>
storm::expressions::Variable GurobiLpSolver<ValueType>::addLowerBoundedIntegerVariable(std::string const& name, ValueType lowerBound,
                                                                                       ValueType objectiveFunctionCoefficient) {
    storm::expressions::Variable newVariable = this->manager->declareOrGetVariable(name, this->manager->getIntegerType());
    this->addVariable(newVariable, GRB_INTEGER, storm::utility::convertNumber<double>(lowerBound), GRB_INFINITY, objectiveFunctionCoefficient);
    return newVariable;
}

template<typename ValueType>
storm::expressions::Variable GurobiLpSolver<ValueType>::addUpperBoundedIntegerVariable(std::string const& name, ValueType upperBound,
                                                                                       ValueType objectiveFunctionCoefficient) {
    storm::expressions::Variable newVariable = this->manager->declareOrGetVariable(name, this->manager->getIntegerType());
    this->addVariable(newVariable, GRB_INTEGER, -GRB_INFINITY, storm::utility::convertNumber<double>(upperBound), objectiveFunctionCoefficient);
    return newVariable;
}

template<typename ValueType>
storm::expressions::Variable GurobiLpSolver<ValueType>::addUnboundedIntegerVariable(std::string const& name, ValueType objectiveFunctionCoefficient) {
    storm::expressions::Variable newVariable = this->manager->declareOrGetVariable(name, this->manager->getIntegerType());
    this->addVariable(newVariable, GRB_INTEGER, -GRB_INFINITY, GRB_INFINITY, objectiveFunctionCoefficient);
    return newVariable;
}

template<typename ValueType>
storm::expressions::Variable GurobiLpSolver<ValueType>::addBinaryVariable(std::string const& name, ValueType objectiveFunctionCoefficient) {
    storm::expressions::Variable newVariable = this->manager->declareOrGetVariable(name, this->manager->getIntegerType());
    this->addVariable(newVariable, GRB_BINARY, 0, 1, objectiveFunctionCoefficient);
    return newVariable;
}

template<typename ValueType>
void GurobiLpSolver<ValueType>::addVariable(storm::expressions::Variable const& variable, char variableType, double lowerBound, double upperBound,
                                            ValueType objectiveFunctionCoefficient) {
    // Assert whether the variable does not exist yet.
    // Due to incremental usage (push(), pop()), a variable might be declared in the manager but not in the lp model.
    STORM_LOG_ASSERT(variableToIndexMap.count(variable) == 0, "Variable " << variable.getName() << " exists already in the model.");
    // Check for valid variable type.
    STORM_LOG_ASSERT(variableType == GRB_CONTINUOUS || variableType == GRB_INTEGER || variableType == GRB_BINARY,
                     "Illegal type '" << variableType << "' for Gurobi variable.");

    // Finally, create the actual variable.
    int error = 0;
    error = GRBaddvar(model, 0, nullptr, nullptr, storm::utility::convertNumber<double>(objectiveFunctionCoefficient),
                      storm::utility::convertNumber<double>(lowerBound), storm::utility::convertNumber<double>(upperBound), variableType,
                      variable.getName().c_str());
    STORM_LOG_THROW(error == 0, storm::exceptions::InvalidStateException,
                    "Could not create binary Gurobi variable (" << GRBgeterrormsg(**environment) << ", error code " << error << ").");
    this->variableToIndexMap.emplace(variable, nextVariableIndex);
    ++nextVariableIndex;
    if (!incrementalData.empty()) {
        incrementalData.back().variables.push_back(variable);
    }
}

template<typename ValueType>
void GurobiLpSolver<ValueType>::addConstraint(std::string const& name, storm::expressions::Expression const& constraint) {
    STORM_LOG_TRACE("Adding constraint " << (name == "" ? std::to_string(nextConstraintIndex) : name) << " to GurobiLpSolver:\n"
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
    std::vector<int> variables;
    std::vector<double> coefficients;
    for (auto const& variableCoefficientPair : leftCoefficients) {
        auto variableIndexPair = this->variableToIndexMap.find(variableCoefficientPair.first);
        variables.push_back(variableIndexPair->second);
        coefficients.push_back(leftCoefficients.getCoefficient(variableIndexPair->first));
    }

    // Determine the type of the constraint and add it properly.
    int error = 0;
    switch (constraint.getOperator()) {
        case storm::expressions::OperatorType::Less:
            error =
                GRBaddconstr(model, variables.size(), variables.data(), coefficients.data(), GRB_LESS_EQUAL,
                             rightCoefficients.getConstantPart() - storm::settings::getModule<storm::settings::modules::GurobiSettings>().getIntegerTolerance(),
                             name == "" ? nullptr : name.c_str());
            break;
        case storm::expressions::OperatorType::LessOrEqual:
            error = GRBaddconstr(model, variables.size(), variables.data(), coefficients.data(), GRB_LESS_EQUAL, rightCoefficients.getConstantPart(),
                                 name == "" ? nullptr : name.c_str());
            break;
        case storm::expressions::OperatorType::Greater:
            error =
                GRBaddconstr(model, variables.size(), variables.data(), coefficients.data(), GRB_GREATER_EQUAL,
                             rightCoefficients.getConstantPart() + storm::settings::getModule<storm::settings::modules::GurobiSettings>().getIntegerTolerance(),
                             name == "" ? nullptr : name.c_str());
            break;
        case storm::expressions::OperatorType::GreaterOrEqual:
            error = GRBaddconstr(model, variables.size(), variables.data(), coefficients.data(), GRB_GREATER_EQUAL, rightCoefficients.getConstantPart(),
                                 name == "" ? nullptr : name.c_str());
            break;
        case storm::expressions::OperatorType::Equal:
            error = GRBaddconstr(model, variables.size(), variables.data(), coefficients.data(), GRB_EQUAL, rightCoefficients.getConstantPart(),
                                 name == "" ? nullptr : name.c_str());
            break;
        default:
            STORM_LOG_ASSERT(false, "Illegal operator in LP solver constraint.");
    }
    ++nextConstraintIndex;
    STORM_LOG_THROW(error == 0, storm::exceptions::InvalidStateException,
                    "Could not assert constraint (" << GRBgeterrormsg(**environment) << ", error code " << error << ").");
}

template<typename ValueType>
void GurobiLpSolver<ValueType>::optimize() const {
    // First incorporate all recent changes.
    this->update();

    // Set the most recently set model sense.
    int error = GRBsetintattr(model, "ModelSense", this->getOptimizationDirection() == OptimizationDirection::Minimize ? 1 : -1);
    STORM_LOG_THROW(error == 0, storm::exceptions::InvalidStateException,
                    "Unable to set Gurobi model sense (" << GRBgeterrormsg(**environment) << ", error code " << error << ").");

    // Then we actually optimize the model.
    error = GRBoptimize(model);
    STORM_LOG_THROW(error == 0, storm::exceptions::InvalidStateException,
                    "Unable to optimize Gurobi model (" << GRBgeterrormsg(**environment) << ", error code " << error << ").");

    this->currentModelHasBeenOptimized = true;
}

template<typename ValueType>
bool GurobiLpSolver<ValueType>::isInfeasible() const {
    if (!this->currentModelHasBeenOptimized) {
        throw storm::exceptions::InvalidStateException() << "Illegal call to GurobiLpSolver<ValueType>::isInfeasible: model has not been optimized.";
    }

    int optimalityStatus = 0;

    int error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimalityStatus);
    STORM_LOG_THROW(error == 0, storm::exceptions::InvalidStateException,
                    "Unable to retrieve optimization status of Gurobi model (" << GRBgeterrormsg(**environment) << ", error code " << error << ").");

    // By default, Gurobi may tell us only that the model is either infeasible or unbounded. To decide which one
    // it is, we need to perform an extra step.
    if (optimalityStatus == GRB_INF_OR_UNBD) {
        error = GRBsetintparam(GRBgetenv(model), GRB_INT_PAR_DUALREDUCTIONS, 0);
        STORM_LOG_THROW(error == 0, storm::exceptions::InvalidStateException,
                        "Unable to set Gurobi parameter (" << GRBgeterrormsg(**environment) << ", error code " << error << ").");

        this->optimize();

        error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimalityStatus);
        STORM_LOG_THROW(error == 0, storm::exceptions::InvalidStateException,
                        "Unable to retrieve optimization status of Gurobi model (" << GRBgeterrormsg(**environment) << ", error code " << error << ").");

        error = GRBsetintparam(GRBgetenv(model), GRB_INT_PAR_DUALREDUCTIONS, 1);
        STORM_LOG_THROW(error == 0, storm::exceptions::InvalidStateException,
                        "Unable to set Gurobi parameter (" << GRBgeterrormsg(**environment) << ", error code " << error << ").");
    }

    return optimalityStatus == GRB_INFEASIBLE;
}

template<typename ValueType>
bool GurobiLpSolver<ValueType>::isUnbounded() const {
    if (!this->currentModelHasBeenOptimized) {
        throw storm::exceptions::InvalidStateException() << "Illegal call to GurobiLpSolver<ValueType>::isUnbounded: model has not been optimized.";
    }

    int optimalityStatus = 0;

    int error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimalityStatus);
    STORM_LOG_THROW(error == 0, storm::exceptions::InvalidStateException,
                    "Unable to retrieve optimization status of Gurobi model (" << GRBgeterrormsg(**environment) << ", error code " << error << ").");

    // By default, Gurobi may tell us only that the model is either infeasible or unbounded. To decide which one
    // it is, we need to perform an extra step.
    if (optimalityStatus == GRB_INF_OR_UNBD) {
        error = GRBsetintparam(GRBgetenv(model), GRB_INT_PAR_DUALREDUCTIONS, 0);
        STORM_LOG_THROW(error == 0, storm::exceptions::InvalidStateException,
                        "Unable to set Gurobi parameter (" << GRBgeterrormsg(**environment) << ", error code " << error << ").");

        this->optimize();

        error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimalityStatus);
        STORM_LOG_THROW(error == 0, storm::exceptions::InvalidStateException,
                        "Unable to retrieve optimization status of Gurobi model (" << GRBgeterrormsg(**environment) << ", error code " << error << ").");

        error = GRBsetintparam(GRBgetenv(model), GRB_INT_PAR_DUALREDUCTIONS, 1);
        STORM_LOG_THROW(error == 0, storm::exceptions::InvalidStateException,
                        "Unable to set Gurobi parameter (" << GRBgeterrormsg(**environment) << ", error code " << error << ").");
    }

    return optimalityStatus == GRB_UNBOUNDED;
}

template<typename ValueType>
bool GurobiLpSolver<ValueType>::isOptimal() const {
    if (!this->currentModelHasBeenOptimized) {
        return false;
    }
    int optimalityStatus = 0;

    int error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimalityStatus);
    STORM_LOG_THROW(error == 0, storm::exceptions::InvalidStateException,
                    "Unable to retrieve optimization status of Gurobi model (" << GRBgeterrormsg(**environment) << ", error code " << error << ").");

    return optimalityStatus == GRB_OPTIMAL;
}

template<typename ValueType>
ValueType GurobiLpSolver<ValueType>::getContinuousValue(storm::expressions::Variable const& variable) const {
    if (!this->isOptimal()) {
        STORM_LOG_THROW(!this->isInfeasible(), storm::exceptions::InvalidAccessException,
                        "Unable to get Gurobi solution from infeasible model (" << GRBgeterrormsg(**environment) << ").");
        STORM_LOG_THROW(!this->isUnbounded(), storm::exceptions::InvalidAccessException,
                        "Unable to get Gurobi solution from unbounded model (" << GRBgeterrormsg(**environment) << ").");
        STORM_LOG_THROW(false, storm::exceptions::InvalidAccessException,
                        "Unable to get Gurobi solution from unoptimized model (" << GRBgeterrormsg(**environment) << ").");
    }

    auto variableIndexPair = this->variableToIndexMap.find(variable);
    STORM_LOG_THROW(variableIndexPair != this->variableToIndexMap.end(), storm::exceptions::InvalidAccessException,
                    "Accessing value of unknown variable '" << variable.getName() << "'.");

    double value = 0;
    int error = GRBgetdblattrelement(model, GRB_DBL_ATTR_X, variableIndexPair->second, &value);
    STORM_LOG_THROW(error == 0, storm::exceptions::InvalidStateException,
                    "Unable to get Gurobi solution (" << GRBgeterrormsg(**environment) << ", error code " << error << ").");

    return storm::utility::convertNumber<ValueType>(value);
}

template<typename ValueType>
int_fast64_t GurobiLpSolver<ValueType>::getIntegerValue(storm::expressions::Variable const& variable) const {
    if (!this->isOptimal()) {
        STORM_LOG_THROW(!this->isInfeasible(), storm::exceptions::InvalidAccessException,
                        "Unable to get Gurobi solution from infeasible model (" << GRBgeterrormsg(**environment) << ").");
        STORM_LOG_THROW(!this->isUnbounded(), storm::exceptions::InvalidAccessException,
                        "Unable to get Gurobi solution from unbounded model (" << GRBgeterrormsg(**environment) << ").");
        STORM_LOG_THROW(false, storm::exceptions::InvalidAccessException,
                        "Unable to get Gurobi solution from unoptimized model (" << GRBgeterrormsg(**environment) << ").");
    }

    auto variableIndexPair = this->variableToIndexMap.find(variable);
    STORM_LOG_THROW(variableIndexPair != this->variableToIndexMap.end(), storm::exceptions::InvalidAccessException,
                    "Accessing value of unknown variable '" << variable.getName() << "'.");

    double value = 0;
    int error = GRBgetdblattrelement(model, GRB_DBL_ATTR_X, variableIndexPair->second, &value);
    STORM_LOG_THROW(error == 0, storm::exceptions::InvalidStateException,
                    "Unable to get Gurobi solution (" << GRBgeterrormsg(**environment) << ", error code " << error << ").");
    double roundedValue = std::round(value);
    double diff = std::abs(roundedValue - value);
    STORM_LOG_ERROR_COND(diff <= storm::settings::getModule<storm::settings::modules::GurobiSettings>().getIntegerTolerance(),
                         "Illegal value for integer variable in Gurobi solution (" << value << "). Difference to nearest int is " << diff);
    return static_cast<int_fast64_t>(roundedValue);
}

template<typename ValueType>
bool GurobiLpSolver<ValueType>::getBinaryValue(storm::expressions::Variable const& variable) const {
    if (!this->isOptimal()) {
        STORM_LOG_THROW(!this->isInfeasible(), storm::exceptions::InvalidAccessException,
                        "Unable to get Gurobi solution from infeasible model (" << GRBgeterrormsg(**environment) << ").");
        STORM_LOG_THROW(!this->isUnbounded(), storm::exceptions::InvalidAccessException,
                        "Unable to get Gurobi solution from unbounded model (" << GRBgeterrormsg(**environment) << ").");
        STORM_LOG_THROW(false, storm::exceptions::InvalidAccessException,
                        "Unable to get Gurobi solution from unoptimized model (" << GRBgeterrormsg(**environment) << ").");
    }

    auto variableIndexPair = this->variableToIndexMap.find(variable);
    STORM_LOG_THROW(variableIndexPair != this->variableToIndexMap.end(), storm::exceptions::InvalidAccessException,
                    "Accessing value of unknown variable '" << variable.getName() << "'.");

    double value = 0;
    int error = GRBgetdblattrelement(model, GRB_DBL_ATTR_X, variableIndexPair->second, &value);
    STORM_LOG_THROW(error == 0, storm::exceptions::InvalidStateException,
                    "Unable to get Gurobi solution (" << GRBgeterrormsg(**environment) << ", error code " << error << ").");

    if (value > 0.5) {
        STORM_LOG_ERROR_COND(std::abs(value - 1.0) <= storm::settings::getModule<storm::settings::modules::GurobiSettings>().getIntegerTolerance(),
                             "Illegal value for binary variable in Gurobi solution (" << value << ").");
        return true;
    } else {
        STORM_LOG_ERROR_COND(std::abs(value) <= storm::settings::getModule<storm::settings::modules::GurobiSettings>().getIntegerTolerance(),
                             "Illegal value for binary variable in Gurobi solution (" << value << ").");
        return false;
    }
}

template<typename ValueType>
ValueType GurobiLpSolver<ValueType>::getObjectiveValue() const {
    if (!this->isOptimal()) {
        STORM_LOG_THROW(!this->isInfeasible(), storm::exceptions::InvalidAccessException,
                        "Unable to get Gurobi solution from infeasible model (" << GRBgeterrormsg(**environment) << ").");
        STORM_LOG_THROW(!this->isUnbounded(), storm::exceptions::InvalidAccessException,
                        "Unable to get Gurobi solution from unbounded model (" << GRBgeterrormsg(**environment) << ").");
        STORM_LOG_THROW(false, storm::exceptions::InvalidAccessException,
                        "Unable to get Gurobi solution from unoptimized model (" << GRBgeterrormsg(**environment) << ").");
    }

    double value = 0;
    int error = GRBgetdblattr(model, GRB_DBL_ATTR_OBJVAL, &value);
    STORM_LOG_THROW(error == 0, storm::exceptions::InvalidStateException,
                    "Unable to get Gurobi solution (" << GRBgeterrormsg(**environment) << ", error code " << error << ").");

    return storm::utility::convertNumber<ValueType>(value);
}

template<typename ValueType>
void GurobiLpSolver<ValueType>::writeModelToFile(std::string const& filename) const {
    int error = GRBwrite(model, filename.c_str());
    if (error) {
        STORM_LOG_ERROR("Unable to write Gurobi model (" << GRBgeterrormsg(**environment) << ", error code " << error << ") to file.");
        throw storm::exceptions::InvalidStateException()
            << "Unable to write Gurobi model (" << GRBgeterrormsg(**environment) << ", error code " << error << ") to file.";
    }
}

template<typename ValueType>
void GurobiLpSolver<ValueType>::push() {
    IncrementalLevel lvl;
    lvl.firstConstraintIndex = nextConstraintIndex;
    incrementalData.push_back(lvl);
}

template<typename ValueType>
void GurobiLpSolver<ValueType>::pop() {
    if (incrementalData.empty()) {
        STORM_LOG_ERROR("Tried to pop from a solver without pushing before.");
    } else {
        IncrementalLevel const& lvl = incrementalData.back();

        std::vector<int> indicesToBeRemoved = storm::utility::vector::buildVectorForRange(lvl.firstConstraintIndex, nextConstraintIndex);
        GRBdelconstrs(model, indicesToBeRemoved.size(), indicesToBeRemoved.data());
        nextConstraintIndex = lvl.firstConstraintIndex;
        indicesToBeRemoved.clear();

        if (!lvl.variables.empty()) {
            int firstIndex = -1;
            bool first = true;
            for (auto const& var : lvl.variables) {
                if (first) {
                    auto it = variableToIndexMap.find(var);
                    firstIndex = it->second;
                    variableToIndexMap.erase(it);
                    first = false;
                } else {
                    variableToIndexMap.erase(var);
                }
            }
            std::vector<int> indicesToBeRemoved = storm::utility::vector::buildVectorForRange(firstIndex, nextVariableIndex);
            GRBdelvars(model, indicesToBeRemoved.size(), indicesToBeRemoved.data());
            nextVariableIndex = firstIndex;
        }
        incrementalData.pop_back();
        update();
    }
}

template<typename ValueType>
void GurobiLpSolver<ValueType>::setMaximalSolutionCount(uint64_t value) {
    int error = GRBsetintparam(GRBgetenv(model), "PoolSolutions", value);
    STORM_LOG_THROW(error == 0, storm::exceptions::InvalidStateException,
                    "Unable to set Gurobi Parameter PoolSolutions (" << GRBgeterrormsg(**environment) << ", error code " << error << ").");
}

template<typename ValueType>
uint64_t GurobiLpSolver<ValueType>::getSolutionCount() const {
    STORM_LOG_THROW(this->currentModelHasBeenOptimized, storm::exceptions::InvalidStateException,
                    "Illegal call to GurobiLpSolver<ValueType>::getSolutionCount: model has not been optimized.");
    int result = -1;
    int error = GRBgetintattr(model, "SolCount", &result);
    STORM_LOG_THROW(error == 0 && result >= 0, storm::exceptions::InvalidStateException, "Unable to get solution count or invalid number of solutions.");
    return result;
}

template<typename ValueType>
ValueType GurobiLpSolver<ValueType>::getContinuousValue(storm::expressions::Variable const& variable, uint64_t const& solutionIndex) const {
    if (!this->isOptimal()) {
        STORM_LOG_THROW(!this->isInfeasible(), storm::exceptions::InvalidAccessException,
                        "Unable to get Gurobi solution from infeasible model (" << GRBgeterrormsg(**environment) << ").");
        STORM_LOG_THROW(!this->isUnbounded(), storm::exceptions::InvalidAccessException,
                        "Unable to get Gurobi solution from unbounded model (" << GRBgeterrormsg(**environment) << ").");
        STORM_LOG_THROW(false, storm::exceptions::InvalidAccessException,
                        "Unable to get Gurobi solution from unoptimized model (" << GRBgeterrormsg(**environment) << ").");
    }
    STORM_LOG_ASSERT(solutionIndex < getSolutionCount(), "Invalid solution index.");

    auto variableIndexPair = this->variableToIndexMap.find(variable);
    STORM_LOG_THROW(variableIndexPair != this->variableToIndexMap.end(), storm::exceptions::InvalidAccessException,
                    "Accessing value of unknown variable '" << variable.getName() << "'.");

    double value = 0;
    int error = GRBsetintparam(GRBgetenv(model), GRB_INT_PAR_SOLUTIONNUMBER, solutionIndex);
    STORM_LOG_THROW(error == 0, storm::exceptions::InvalidStateException,
                    "Unable to set Gurobi solution index (" << GRBgeterrormsg(**environment) << ", error code " << error << ").");
    error = GRBgetdblattrelement(model, GRB_DBL_ATTR_Xn, variableIndexPair->second, &value);
    STORM_LOG_THROW(error == 0, storm::exceptions::InvalidStateException,
                    "Unable to get Gurobi solution (" << GRBgeterrormsg(**environment) << ", error code " << error << ").");

    return storm::utility::convertNumber<ValueType>(value);
}

template<typename ValueType>
int_fast64_t GurobiLpSolver<ValueType>::getIntegerValue(storm::expressions::Variable const& variable, uint64_t const& solutionIndex) const {
    if (!this->isOptimal()) {
        STORM_LOG_THROW(!this->isInfeasible(), storm::exceptions::InvalidAccessException,
                        "Unable to get Gurobi solution from infeasible model (" << GRBgeterrormsg(**environment) << ").");
        STORM_LOG_THROW(!this->isUnbounded(), storm::exceptions::InvalidAccessException,
                        "Unable to get Gurobi solution from unbounded model (" << GRBgeterrormsg(**environment) << ").");
        STORM_LOG_THROW(false, storm::exceptions::InvalidAccessException,
                        "Unable to get Gurobi solution from unoptimized model (" << GRBgeterrormsg(**environment) << ").");
    }
    STORM_LOG_ASSERT(solutionIndex < getSolutionCount(), "Invalid solution index.");

    auto variableIndexPair = this->variableToIndexMap.find(variable);
    STORM_LOG_THROW(variableIndexPair != this->variableToIndexMap.end(), storm::exceptions::InvalidAccessException,
                    "Accessing value of unknown variable '" << variable.getName() << "'.");

    double value = 0;
    int error = GRBsetintparam(GRBgetenv(model), GRB_INT_PAR_SOLUTIONNUMBER, solutionIndex);
    STORM_LOG_THROW(error == 0, storm::exceptions::InvalidStateException,
                    "Unable to set Gurobi solution index (" << GRBgeterrormsg(**environment) << ", error code " << error << ").");
    error = GRBgetdblattrelement(model, GRB_DBL_ATTR_Xn, variableIndexPair->second, &value);
    STORM_LOG_THROW(error == 0, storm::exceptions::InvalidStateException,
                    "Unable to get Gurobi solution (" << GRBgeterrormsg(**environment) << ", error code " << error << ").");
    double roundedValue = std::round(value);
    double diff = std::abs(roundedValue - value);
    STORM_LOG_ERROR_COND(diff <= storm::settings::getModule<storm::settings::modules::GurobiSettings>().getIntegerTolerance(),
                         "Illegal value for integer variable in Gurobi solution (" << value << "). Difference to nearest int is " << diff);
    return static_cast<int_fast64_t>(roundedValue);
}

template<typename ValueType>
bool GurobiLpSolver<ValueType>::getBinaryValue(storm::expressions::Variable const& variable, uint64_t const& solutionIndex) const {
    if (!this->isOptimal()) {
        STORM_LOG_THROW(!this->isInfeasible(), storm::exceptions::InvalidAccessException,
                        "Unable to get Gurobi solution from infeasible model (" << GRBgeterrormsg(**environment) << ").");
        STORM_LOG_THROW(!this->isUnbounded(), storm::exceptions::InvalidAccessException,
                        "Unable to get Gurobi solution from unbounded model (" << GRBgeterrormsg(**environment) << ").");
        STORM_LOG_THROW(false, storm::exceptions::InvalidAccessException,
                        "Unable to get Gurobi solution from unoptimized model (" << GRBgeterrormsg(**environment) << ").");
    }
    STORM_LOG_ASSERT(solutionIndex < getSolutionCount(), "Invalid solution index.");

    auto variableIndexPair = this->variableToIndexMap.find(variable);
    STORM_LOG_THROW(variableIndexPair != this->variableToIndexMap.end(), storm::exceptions::InvalidAccessException,
                    "Accessing value of unknown variable '" << variable.getName() << "'.");

    double value = 0;
    int error = GRBsetintparam(GRBgetenv(model), GRB_INT_PAR_SOLUTIONNUMBER, solutionIndex);
    STORM_LOG_THROW(error == 0, storm::exceptions::InvalidStateException,
                    "Unable to set Gurobi solution index (" << GRBgeterrormsg(**environment) << ", error code " << error << ").");
    error = GRBgetdblattrelement(model, GRB_DBL_ATTR_Xn, variableIndexPair->second, &value);
    STORM_LOG_THROW(error == 0, storm::exceptions::InvalidStateException,
                    "Unable to get Gurobi solution (" << GRBgeterrormsg(**environment) << ", error code " << error << ").");

    if (value > 0.5) {
        STORM_LOG_ERROR_COND(std::abs(value - 1) <= storm::settings::getModule<storm::settings::modules::GurobiSettings>().getIntegerTolerance(),
                             "Illegal value for integer variable in Gurobi solution (" << value << ").");
        return true;
    } else {
        STORM_LOG_ERROR_COND(std::abs(value) <= storm::settings::getModule<storm::settings::modules::GurobiSettings>().getIntegerTolerance(),
                             "Illegal value for integer variable in Gurobi solution (" << value << ").");
        return false;
    }
}

template<typename ValueType>
ValueType GurobiLpSolver<ValueType>::getObjectiveValue(uint64_t const& solutionIndex) const {
    if (!this->isOptimal()) {
        STORM_LOG_THROW(!this->isInfeasible(), storm::exceptions::InvalidAccessException,
                        "Unable to get Gurobi solution from infeasible model (" << GRBgeterrormsg(**environment) << ").");
        STORM_LOG_THROW(!this->isUnbounded(), storm::exceptions::InvalidAccessException,
                        "Unable to get Gurobi solution from unbounded model (" << GRBgeterrormsg(**environment) << ").");
        STORM_LOG_THROW(false, storm::exceptions::InvalidAccessException,
                        "Unable to get Gurobi solution from unoptimized model (" << GRBgeterrormsg(**environment) << ").");
    }
    STORM_LOG_ASSERT(solutionIndex < getSolutionCount(), "Invalid solution index.");

    double value = 0;
    int error = GRBsetintparam(GRBgetenv(model), GRB_INT_PAR_SOLUTIONNUMBER, solutionIndex);
    STORM_LOG_THROW(error == 0, storm::exceptions::InvalidStateException,
                    "Unable to set Gurobi solution index (" << GRBgeterrormsg(**environment) << ", error code " << error << ").");
    error = GRBgetdblattr(model, GRB_DBL_ATTR_POOLOBJVAL, &value);
    STORM_LOG_THROW(error == 0, storm::exceptions::InvalidStateException,
                    "Unable to get Gurobi solution (" << GRBgeterrormsg(**environment) << ", error code " << error << ").");

    return storm::utility::convertNumber<ValueType>(value);
}

template<typename ValueType>
void GurobiLpSolver<ValueType>::setMaximalMILPGap(ValueType const& gap, bool relative) {
    int error = -1;
    if (relative) {
        error = GRBsetdblparam(GRBgetenv(model), GRB_DBL_PAR_MIPGAP, storm::utility::convertNumber<double>(gap));
    } else {
        error = GRBsetdblparam(GRBgetenv(model), GRB_DBL_PAR_MIPGAPABS, storm::utility::convertNumber<double>(gap));
    }
    STORM_LOG_THROW(error == 0, storm::exceptions::InvalidStateException,
                    "Unable to set Gurobi MILP GAP (" << GRBgeterrormsg(**environment) << ", error code " << error << ").");
}

template<typename ValueType>
ValueType GurobiLpSolver<ValueType>::getMILPGap(bool relative) const {
    double relativeGap;
    int error = GRBgetdblattr(model, GRB_DBL_ATTR_MIPGAP, &relativeGap);
    STORM_LOG_THROW(error == 0, storm::exceptions::InvalidStateException,
                    "Unable to get Gurobi MILP GAP (" << GRBgeterrormsg(**environment) << ", error code " << error << ").");
    auto result = storm::utility::convertNumber<ValueType>(relativeGap);
    if (relative) {
        return result;
    } else {
        return storm::utility::abs<ValueType>(result * getObjectiveValue());
    }
}

#else
template<typename ValueType>
GurobiLpSolver<ValueType>::GurobiLpSolver(std::shared_ptr<GurobiEnvironment> const&, std::string const&, OptimizationDirection const&) {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for Gurobi. Yet, a method was called that "
                                                          "requires this support. Please choose a version of support with Gurobi support.";
}

template<typename ValueType>
GurobiLpSolver<ValueType>::GurobiLpSolver(std::shared_ptr<GurobiEnvironment> const&, std::string const&) {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for Gurobi. Yet, a method was called that "
                                                          "requires this support. Please choose a version of support with Gurobi support.";
}

template<typename ValueType>
GurobiLpSolver<ValueType>::GurobiLpSolver(std::shared_ptr<GurobiEnvironment> const&, OptimizationDirection const&) {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for Gurobi. Yet, a method was called that "
                                                          "requires this support. Please choose a version of support with Gurobi support.";
}

template<typename ValueType>
GurobiLpSolver<ValueType>::GurobiLpSolver(std::shared_ptr<GurobiEnvironment> const&) {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for Gurobi. Yet, a method was called that "
                                                          "requires this support. Please choose a version of support with Gurobi support.";
}

template<typename ValueType>
GurobiLpSolver<ValueType>::~GurobiLpSolver() {}

template<typename ValueType>
storm::expressions::Variable GurobiLpSolver<ValueType>::addBoundedContinuousVariable(std::string const&, ValueType, ValueType, ValueType) {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for Gurobi. Yet, a method was called that "
                                                          "requires this support. Please choose a version of support with Gurobi support.";
}

template<typename ValueType>
storm::expressions::Variable GurobiLpSolver<ValueType>::addLowerBoundedContinuousVariable(std::string const&, ValueType, ValueType) {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for Gurobi. Yet, a method was called that "
                                                          "requires this support. Please choose a version of support with Gurobi support.";
}

template<typename ValueType>
storm::expressions::Variable GurobiLpSolver<ValueType>::addUpperBoundedContinuousVariable(std::string const&, ValueType, ValueType) {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for Gurobi. Yet, a method was called that "
                                                          "requires this support. Please choose a version of support with Gurobi support.";
}

template<typename ValueType>
storm::expressions::Variable GurobiLpSolver<ValueType>::addUnboundedContinuousVariable(std::string const&, ValueType) {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for Gurobi. Yet, a method was called that "
                                                          "requires this support. Please choose a version of support with Gurobi support.";
}

template<typename ValueType>
storm::expressions::Variable GurobiLpSolver<ValueType>::addBoundedIntegerVariable(std::string const&, ValueType, ValueType, ValueType) {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for Gurobi. Yet, a method was called that "
                                                          "requires this support. Please choose a version of support with Gurobi support.";
}

template<typename ValueType>
storm::expressions::Variable GurobiLpSolver<ValueType>::addLowerBoundedIntegerVariable(std::string const&, ValueType, ValueType) {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for Gurobi. Yet, a method was called that "
                                                          "requires this support. Please choose a version of support with Gurobi support.";
}

template<typename ValueType>
storm::expressions::Variable GurobiLpSolver<ValueType>::addUpperBoundedIntegerVariable(std::string const&, ValueType, ValueType) {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for Gurobi. Yet, a method was called that "
                                                          "requires this support. Please choose a version of support with Gurobi support.";
}

template<typename ValueType>
storm::expressions::Variable GurobiLpSolver<ValueType>::addUnboundedIntegerVariable(std::string const&, ValueType) {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for Gurobi. Yet, a method was called that "
                                                          "requires this support. Please choose a version of support with Gurobi support.";
}

template<typename ValueType>
storm::expressions::Variable GurobiLpSolver<ValueType>::addBinaryVariable(std::string const&, ValueType) {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for Gurobi. Yet, a method was called that "
                                                          "requires this support. Please choose a version of support with Gurobi support.";
}

template<typename ValueType>
void GurobiLpSolver<ValueType>::update() const {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for Gurobi. Yet, a method was called that "
                                                          "requires this support. Please choose a version of support with Gurobi support.";
}

template<typename ValueType>
void GurobiLpSolver<ValueType>::addConstraint(std::string const&, storm::expressions::Expression const&) {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for Gurobi. Yet, a method was called that "
                                                          "requires this support. Please choose a version of support with Gurobi support.";
}

template<typename ValueType>
void GurobiLpSolver<ValueType>::optimize() const {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for Gurobi. Yet, a method was called that "
                                                          "requires this support. Please choose a version of support with Gurobi support.";
}

template<typename ValueType>
bool GurobiLpSolver<ValueType>::isInfeasible() const {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for Gurobi. Yet, a method was called that "
                                                          "requires this support. Please choose a version of support with Gurobi support.";
}

template<typename ValueType>
bool GurobiLpSolver<ValueType>::isUnbounded() const {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for Gurobi. Yet, a method was called that "
                                                          "requires this support. Please choose a version of support with Gurobi support.";
}

template<typename ValueType>
bool GurobiLpSolver<ValueType>::isOptimal() const {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for Gurobi. Yet, a method was called that "
                                                          "requires this support. Please choose a version of support with Gurobi support.";
}

template<typename ValueType>
ValueType GurobiLpSolver<ValueType>::getContinuousValue(storm::expressions::Variable const&) const {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for Gurobi. Yet, a method was called that "
                                                          "requires this support. Please choose a version of support with Gurobi support.";
}

template<typename ValueType>
int_fast64_t GurobiLpSolver<ValueType>::getIntegerValue(storm::expressions::Variable const&) const {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for Gurobi. Yet, a method was called that "
                                                          "requires this support. Please choose a version of support with Gurobi support.";
}

template<typename ValueType>
bool GurobiLpSolver<ValueType>::getBinaryValue(storm::expressions::Variable const&) const {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for Gurobi. Yet, a method was called that "
                                                          "requires this support. Please choose a version of support with Gurobi support.";
}

template<typename ValueType>
ValueType GurobiLpSolver<ValueType>::getObjectiveValue() const {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for Gurobi. Yet, a method was called that "
                                                          "requires this support. Please choose a version of support with Gurobi support.";
}

template<typename ValueType>
void GurobiLpSolver<ValueType>::writeModelToFile(std::string const&) const {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for Gurobi. Yet, a method was called that "
                                                          "requires this support. Please choose a version of support with Gurobi support.";
}

template<typename ValueType>
void GurobiLpSolver<ValueType>::push() {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for Gurobi. Yet, a method was called that "
                                                          "requires this support. Please choose a version of support with Gurobi support.";
}

template<typename ValueType>
void GurobiLpSolver<ValueType>::pop() {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for Gurobi. Yet, a method was called that "
                                                          "requires this support. Please choose a version of support with Gurobi support.";
}

template<typename ValueType>
void GurobiLpSolver<ValueType>::setMaximalSolutionCount(uint64_t) {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for Gurobi. Yet, a method was called that "
                                                          "requires this support. Please choose a version of storm with Gurobi support.";
}

template<typename ValueType>
uint64_t GurobiLpSolver<ValueType>::getSolutionCount() const {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for Gurobi. Yet, a method was called that "
                                                          "requires this support. Please choose a version of storm with Gurobi support.";
}

template<typename ValueType>
ValueType GurobiLpSolver<ValueType>::getContinuousValue(storm::expressions::Variable const&, uint64_t const&) const {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for Gurobi. Yet, a method was called that "
                                                          "requires this support. Please choose a version of storm with Gurobi support.";
}

template<typename ValueType>
int_fast64_t GurobiLpSolver<ValueType>::getIntegerValue(storm::expressions::Variable const&, uint64_t const&) const {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for Gurobi. Yet, a method was called that "
                                                          "requires this support. Please choose a version of storm with Gurobi support.";
}

template<typename ValueType>
bool GurobiLpSolver<ValueType>::getBinaryValue(storm::expressions::Variable const&, uint64_t const&) const {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for Gurobi. Yet, a method was called that "
                                                          "requires this support. Please choose a version of storm with Gurobi support.";
}

template<typename ValueType>
ValueType GurobiLpSolver<ValueType>::getObjectiveValue(uint64_t const&) const {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for Gurobi. Yet, a method was called that "
                                                          "requires this support. Please choose a version of storm with Gurobi support.";
}

template<typename ValueType>
void GurobiLpSolver<ValueType>::setMaximalMILPGap(ValueType const&, bool) {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for Gurobi. Yet, a method was called that "
                                                          "requires this support. Please choose a version of storm with Gurobi support.";
}

template<typename ValueType>
ValueType GurobiLpSolver<ValueType>::getMILPGap(bool) const {
    throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for Gurobi. Yet, a method was called that "
                                                          "requires this support. Please choose a version of storm with Gurobi support.";
}

#endif

std::string toString(GurobiSolverMethod const& method) {
    switch (method) {
        case GurobiSolverMethod::AUTOMATIC:
            return "auto";
        case GurobiSolverMethod::PRIMALSIMPLEX:
            return "primal-simplex";
        case GurobiSolverMethod::DUALSIMPLEX:
            return "dual-simplex";
        case GurobiSolverMethod::BARRIER:
            return "barrier";
        case GurobiSolverMethod::CONCURRENT:
            return "concurrent";
        case GurobiSolverMethod::DETCONCURRENT:
            return "deterministic-concurrent";
        case GurobiSolverMethod::DETCONCURRENTSIMPLEX:
            return "deterministic-concurrent-simplex";
    }
    STORM_LOG_ASSERT(false, "Unknown solver method");
}

std::optional<GurobiSolverMethod> gurobiSolverMethodFromString(std::string const& method) {
    for (auto const& mt : getGurobiSolverMethods()) {
        if (toString(mt) == method) {
            return mt;
        }
    }
    return {};
}

std::vector<GurobiSolverMethod> getGurobiSolverMethods() {
    return {GurobiSolverMethod::AUTOMATIC,  GurobiSolverMethod::PRIMALSIMPLEX, GurobiSolverMethod::DUALSIMPLEX,         GurobiSolverMethod::BARRIER,
            GurobiSolverMethod::CONCURRENT, GurobiSolverMethod::DETCONCURRENT, GurobiSolverMethod::DETCONCURRENTSIMPLEX};
}

template class GurobiLpSolver<double>;
template class GurobiLpSolver<storm::RationalNumber>;
}  // namespace solver
}  // namespace storm
