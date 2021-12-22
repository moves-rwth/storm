#include "storm/solver/GlpkLpSolver.h"

#include <cmath>
#include <iostream>

#include "storm/storage/expressions/LinearCoefficientVisitor.h"

#include "storm/settings/SettingsManager.h"
#include "storm/storage/expressions/Expression.h"
#include "storm/storage/expressions/ExpressionManager.h"
#include "storm/utility/constants.h"
#include "storm/utility/macros.h"
#include "storm/utility/vector.h"

#include "storm/exceptions/InvalidAccessException.h"
#include "storm/exceptions/InvalidArgumentException.h"
#include "storm/exceptions/InvalidStateException.h"

#include "storm/settings/modules/DebugSettings.h"
#include "storm/settings/modules/GlpkSettings.h"

namespace storm {
namespace solver {

#ifdef STORM_HAVE_GLPK
template<typename ValueType>
GlpkLpSolver<ValueType>::GlpkLpSolver(std::string const& name, OptimizationDirection const& optDir)
    : LpSolver<ValueType>(optDir), lp(nullptr), variableToIndexMap(), modelContainsIntegerVariables(false), isInfeasibleFlag(false), isUnboundedFlag(false) {
    // Create the LP problem for glpk.
    lp = glp_create_prob();

    // Set its name and model sense.
    glp_set_prob_name(lp, name.c_str());

    // Set whether the glpk output shall be printed to the command line.
    glp_term_out(storm::settings::getModule<storm::settings::modules::DebugSettings>().isDebugSet() ||
                         storm::settings::getModule<storm::settings::modules::GlpkSettings>().isOutputSet()
                     ? GLP_ON
                     : GLP_OFF);

    // Set the maximal allowed MILP gap to its default value
    glp_iocp* defaultParameters = new glp_iocp();
    glp_init_iocp(defaultParameters);
    this->maxMILPGap = defaultParameters->mip_gap;
    this->maxMILPGapRelative = true;
}

template<typename ValueType>
GlpkLpSolver<ValueType>::GlpkLpSolver(std::string const& name) : GlpkLpSolver(name, OptimizationDirection::Minimize) {
    // Intentionally left empty.
}

template<typename ValueType>
GlpkLpSolver<ValueType>::GlpkLpSolver() : GlpkLpSolver("", OptimizationDirection::Minimize) {
    // Intentionally left empty.
}

template<typename ValueType>
GlpkLpSolver<ValueType>::GlpkLpSolver(OptimizationDirection const& optDir) : GlpkLpSolver("", optDir) {
    // Intentionally left empty.
}

template<typename ValueType>
GlpkLpSolver<ValueType>::~GlpkLpSolver() {
    // Dispose of all objects allocated dynamically by glpk.
    glp_delete_prob(this->lp);
    glp_free_env();
}

template<typename ValueType>
storm::expressions::Variable GlpkLpSolver<ValueType>::addBoundedContinuousVariable(std::string const& name, ValueType lowerBound, ValueType upperBound,
                                                                                   ValueType objectiveFunctionCoefficient) {
    STORM_LOG_ASSERT(lowerBound != upperBound, "GLPK does not support variable bounds of the form [x,x].");
    storm::expressions::Variable newVariable = this->manager->declareOrGetVariable(name, this->manager->getRationalType());
    this->addVariable(newVariable, GLP_CV, GLP_DB, lowerBound, upperBound, objectiveFunctionCoefficient);
    return newVariable;
}

template<typename ValueType>
storm::expressions::Variable GlpkLpSolver<ValueType>::addLowerBoundedContinuousVariable(std::string const& name, ValueType lowerBound,
                                                                                        ValueType objectiveFunctionCoefficient) {
    storm::expressions::Variable newVariable = this->manager->declareOrGetVariable(name, this->manager->getRationalType());
    this->addVariable(newVariable, GLP_CV, GLP_LO, lowerBound, 0, objectiveFunctionCoefficient);
    return newVariable;
}

template<typename ValueType>
storm::expressions::Variable GlpkLpSolver<ValueType>::addUpperBoundedContinuousVariable(std::string const& name, ValueType upperBound,
                                                                                        ValueType objectiveFunctionCoefficient) {
    storm::expressions::Variable newVariable = this->manager->declareOrGetVariable(name, this->manager->getRationalType());
    this->addVariable(newVariable, GLP_CV, GLP_UP, 0, upperBound, objectiveFunctionCoefficient);
    return newVariable;
}

template<typename ValueType>
storm::expressions::Variable GlpkLpSolver<ValueType>::addUnboundedContinuousVariable(std::string const& name, ValueType objectiveFunctionCoefficient) {
    storm::expressions::Variable newVariable = this->manager->declareOrGetVariable(name, this->manager->getRationalType());
    this->addVariable(newVariable, GLP_CV, GLP_FR, 0, 0, objectiveFunctionCoefficient);
    return newVariable;
}

template<typename ValueType>
storm::expressions::Variable GlpkLpSolver<ValueType>::addBoundedIntegerVariable(std::string const& name, ValueType lowerBound, ValueType upperBound,
                                                                                ValueType objectiveFunctionCoefficient) {
    STORM_LOG_ASSERT(lowerBound != upperBound, "GLPK does not support variable bounds of the form [x,x].");
    storm::expressions::Variable newVariable = this->manager->declareOrGetVariable(name, this->manager->getIntegerType());
    this->addVariable(newVariable, GLP_IV, GLP_DB, lowerBound, upperBound, objectiveFunctionCoefficient);
    this->modelContainsIntegerVariables = true;
    return newVariable;
}

template<typename ValueType>
storm::expressions::Variable GlpkLpSolver<ValueType>::addLowerBoundedIntegerVariable(std::string const& name, ValueType lowerBound,
                                                                                     ValueType objectiveFunctionCoefficient) {
    storm::expressions::Variable newVariable = this->manager->declareOrGetVariable(name, this->manager->getIntegerType());
    this->addVariable(newVariable, GLP_IV, GLP_LO, lowerBound, 0, objectiveFunctionCoefficient);
    this->modelContainsIntegerVariables = true;
    return newVariable;
}

template<typename ValueType>
storm::expressions::Variable GlpkLpSolver<ValueType>::addUpperBoundedIntegerVariable(std::string const& name, ValueType upperBound,
                                                                                     ValueType objectiveFunctionCoefficient) {
    storm::expressions::Variable newVariable = this->manager->declareOrGetVariable(name, this->manager->getIntegerType());
    this->addVariable(newVariable, GLP_IV, GLP_UP, 0, upperBound, objectiveFunctionCoefficient);
    this->modelContainsIntegerVariables = true;
    return newVariable;
}

template<typename ValueType>
storm::expressions::Variable GlpkLpSolver<ValueType>::addUnboundedIntegerVariable(std::string const& name, ValueType objectiveFunctionCoefficient) {
    storm::expressions::Variable newVariable = this->manager->declareOrGetVariable(name, this->manager->getIntegerType());
    this->addVariable(newVariable, GLP_IV, GLP_FR, 0, 0, objectiveFunctionCoefficient);
    this->modelContainsIntegerVariables = true;
    return newVariable;
}

template<typename ValueType>
storm::expressions::Variable GlpkLpSolver<ValueType>::addBinaryVariable(std::string const& name, ValueType objectiveFunctionCoefficient) {
    storm::expressions::Variable newVariable = this->manager->declareOrGetVariable(name, this->manager->getIntegerType());
    this->addVariable(newVariable, GLP_BV, GLP_FR, 0, 0, objectiveFunctionCoefficient);
    this->modelContainsIntegerVariables = true;
    return newVariable;
}

template<typename ValueType>
void GlpkLpSolver<ValueType>::addVariable(storm::expressions::Variable const& variable, int variableType, int boundType, ValueType lowerBound,
                                          ValueType upperBound, ValueType objectiveFunctionCoefficient) {
    // Assert whether the variable does not exist yet.
    // Due to incremental usage (push(), pop()), a variable might be declared in the manager but not in the lp model.
    STORM_LOG_ASSERT(variableToIndexMap.count(variable) == 0, "Variable " << variable.getName() << " exists already in the model.");

    // Check for valid variable type.
    STORM_LOG_ASSERT(variableType == GLP_CV || variableType == GLP_IV || variableType == GLP_BV, "Illegal type '" << variableType << "' for glpk variable.");

    // Check for valid bound type.
    STORM_LOG_ASSERT(boundType == GLP_FR || boundType == GLP_UP || boundType == GLP_LO || boundType == GLP_DB,
                     "Illegal bound type for variable '" << variable.getName() << "'.");

    // Finally, create the actual variable.
    int variableIndex = glp_add_cols(this->lp, 1);
    glp_set_col_name(this->lp, variableIndex, variable.getName().c_str());
    glp_set_col_bnds(lp, variableIndex, boundType, storm::utility::convertNumber<double>(lowerBound), storm::utility::convertNumber<double>(upperBound));
    glp_set_col_kind(this->lp, variableIndex, variableType);
    glp_set_obj_coef(this->lp, variableIndex, storm::utility::convertNumber<double>(objectiveFunctionCoefficient));
    this->variableToIndexMap.emplace(variable, variableIndex);
    if (!incrementalData.empty()) {
        incrementalData.back().variables.push_back(variable);
    }
}

template<typename ValueType>
void GlpkLpSolver<ValueType>::update() const {
    // Intentionally left empty.
}

template<typename ValueType>
void GlpkLpSolver<ValueType>::addConstraint(std::string const& name, storm::expressions::Expression const& constraint) {
    // Add the row that will represent this constraint.
    int constraintIndex = glp_add_rows(this->lp, 1);
    glp_set_row_name(this->lp, constraintIndex, name.c_str());

    STORM_LOG_THROW(constraint.getManager() == this->getManager(), storm::exceptions::InvalidArgumentException,
                    "Constraint was not built over the proper variables.");
    STORM_LOG_THROW(constraint.isRelationalExpression(), storm::exceptions::InvalidArgumentException, "Illegal constraint is not a relational expression.");
    STORM_LOG_THROW(constraint.getOperator() != storm::expressions::OperatorType::NotEqual, storm::exceptions::InvalidArgumentException,
                    "Illegal constraint uses inequality operator.");

    storm::expressions::LinearCoefficientVisitor::VariableCoefficients leftCoefficients =
        storm::expressions::LinearCoefficientVisitor().getLinearCoefficients(constraint.getOperand(0));
    storm::expressions::LinearCoefficientVisitor::VariableCoefficients rightCoefficients =
        storm::expressions::LinearCoefficientVisitor().getLinearCoefficients(constraint.getOperand(1));
    leftCoefficients.separateVariablesFromConstantPart(rightCoefficients);

    // Determine the type of the constraint and add it properly.
    switch (constraint.getOperator()) {
        case storm::expressions::OperatorType::Less:
            glp_set_row_bnds(this->lp, constraintIndex, GLP_UP, 0,
                             rightCoefficients.getConstantPart() - storm::settings::getModule<storm::settings::modules::GlpkSettings>().getIntegerTolerance());
            break;
        case storm::expressions::OperatorType::LessOrEqual:
            glp_set_row_bnds(this->lp, constraintIndex, GLP_UP, 0, rightCoefficients.getConstantPart());
            break;
        case storm::expressions::OperatorType::Greater:
            glp_set_row_bnds(this->lp, constraintIndex, GLP_LO,
                             rightCoefficients.getConstantPart() + storm::settings::getModule<storm::settings::modules::GlpkSettings>().getIntegerTolerance(),
                             0);
            break;
        case storm::expressions::OperatorType::GreaterOrEqual:
            glp_set_row_bnds(this->lp, constraintIndex, GLP_LO, rightCoefficients.getConstantPart(), 0);
            break;
        case storm::expressions::OperatorType::Equal:
            glp_set_row_bnds(this->lp, constraintIndex, GLP_FX, rightCoefficients.getConstantPart(), rightCoefficients.getConstantPart());
            break;
        default:
            STORM_LOG_ASSERT(false, "Illegal operator in LP solver constraint.");
    }

    // Now we need to transform the coefficients to the vector representation.
    int len = std::distance(leftCoefficients.begin(), leftCoefficients.end());
    // glpk uses 1-based indexing (wtf!?)...
    std::vector<int> variableIndices(1, -1);
    std::vector<double> coefficients(1, 0.0);
    variableIndices.reserve(len + 1);
    coefficients.reserve(len + 1);
    for (auto const& variableCoefficientPair : leftCoefficients) {
        auto variableIndexPair = this->variableToIndexMap.find(variableCoefficientPair.first);
        variableIndices.push_back(variableIndexPair->second);
        coefficients.push_back(variableCoefficientPair.second);
    }

    glp_set_mat_row(this->lp, constraintIndex, len, variableIndices.data(), coefficients.data());

    this->currentModelHasBeenOptimized = false;
}

// Method used within the MIP solver to terminate early
void callback(glp_tree* t, void* info) {
    auto& mipgap = *static_cast<std::pair<double, bool>*>(info);
    double actualRelativeGap = glp_ios_mip_gap(t);
    double factor = storm::utility::one<double>();
    if (!mipgap.second) {
        // Compute absolute gap
        factor = storm::utility::abs(glp_mip_obj_val(glp_ios_get_prob(t))) + DBL_EPSILON;
        assert(factor >= 0.0);
    }
    if (actualRelativeGap * factor <= mipgap.first) {
        // Terminate early
        mipgap.first = actualRelativeGap;
        mipgap.second = true;  // The gap is relative.
        glp_ios_terminate(t);
    }
}

template<typename ValueType>
void GlpkLpSolver<ValueType>::optimize() const {
    // First, reset the flags.
    this->isInfeasibleFlag = false;
    this->isUnboundedFlag = false;

    // Start by setting the model sense.
    glp_set_obj_dir(this->lp, this->getOptimizationDirection() == OptimizationDirection::Minimize ? GLP_MIN : GLP_MAX);

    int error = 0;
    if (this->modelContainsIntegerVariables) {
        glp_iocp* parameters = new glp_iocp();
        glp_init_iocp(parameters);
        parameters->tol_int = storm::settings::getModule<storm::settings::modules::GlpkSettings>().getIntegerTolerance();
        this->isInfeasibleFlag = false;
        if (storm::settings::getModule<storm::settings::modules::GlpkSettings>().isMILPPresolverEnabled()) {
            parameters->presolve = GLP_ON;
        } else {
            // Without presolving, we solve the relaxed model first. This is required because
            // glp_intopt requires that either presolving is enabled or an optimal initial basis is provided.
            error = glp_simplex(this->lp, nullptr);
            STORM_LOG_THROW(error == 0, storm::exceptions::InvalidStateException, "Unable to optimize relaxed glpk model (" << error << ").");
            // If the relaxed model is already not feasible, we don't have to solve the actual model.
            if (glp_get_status(this->lp) == GLP_INFEAS || glp_get_status(this->lp) == GLP_NOFEAS) {
                this->isInfeasibleFlag = true;
            }
            // If the relaxed model is unbounded, there could still be no feasible integer solution.
            // However, since we can not provide an optimal initial basis, we will need to enable presolving
            if (glp_get_status(this->lp) == GLP_UNBND) {
                parameters->presolve = GLP_ON;
            } else {
                parameters->presolve = GLP_OFF;
            }
        }
        if (!this->isInfeasibleFlag) {
            // Check whether we allow sub-optimal solutions via a non-zero MIP gap.
            // parameters->mip_gap = this->maxMILPGap; (only works for relative values. Also, we need to obtain the actual gap anyway.
            std::pair<double, bool> mipgap(this->maxMILPGap, this->maxMILPGapRelative);
            if (!storm::utility::isZero(this->maxMILPGap)) {
                parameters->cb_func = &callback;
                parameters->cb_info = &mipgap;
            }

            // Invoke mip solving
            error = glp_intopt(this->lp, parameters);
            int status = glp_mip_status(this->lp);
            delete parameters;

            // mipgap.first has been set to the achieved mipgap (either within the callback function or because it has been set to this->maxMILPGap)
            this->actualRelativeMILPGap = mipgap.first;

            // In case the error is caused by an infeasible problem, we do not want to view this as an error and
            // reset the error code.
            if (error == GLP_ENOPFS || status == GLP_NOFEAS) {
                this->isInfeasibleFlag = true;
                error = 0;
            } else if (error == GLP_ENODFS) {
                this->isUnboundedFlag = true;
                error = 0;
            } else if (error == GLP_ESTOP) {
                // Early termination due to achieved MIP Gap. That's fine.
                error = 0;
            } else if (error == GLP_EBOUND) {
                throw storm::exceptions::InvalidStateException()
                    << "The bounds of some variables are illegal. Note that glpk only accepts integer bounds for integer variables.";
            }
        }
    } else {
        error = glp_simplex(this->lp, nullptr);
    }

    STORM_LOG_THROW(error == 0, storm::exceptions::InvalidStateException, "Unable to optimize glpk model (" << error << ").");
    this->currentModelHasBeenOptimized = true;
}

template<typename ValueType>
bool GlpkLpSolver<ValueType>::isInfeasible() const {
    if (!this->currentModelHasBeenOptimized) {
        throw storm::exceptions::InvalidStateException() << "Illegal call to GlpkLpSolver::isInfeasible: model has not been optimized.";
    }

    if (this->modelContainsIntegerVariables) {
        return isInfeasibleFlag;
    } else {
        return glp_get_status(this->lp) == GLP_INFEAS || glp_get_status(this->lp) == GLP_NOFEAS;
    }
}

template<typename ValueType>
bool GlpkLpSolver<ValueType>::isUnbounded() const {
    if (!this->currentModelHasBeenOptimized) {
        throw storm::exceptions::InvalidStateException() << "Illegal call to GlpkLpSolver::isUnbounded: model has not been optimized.";
    }

    if (this->modelContainsIntegerVariables) {
        return isUnboundedFlag;
    } else {
        return glp_get_status(this->lp) == GLP_UNBND;
    }
}

template<typename ValueType>
bool GlpkLpSolver<ValueType>::isOptimal() const {
    if (!this->currentModelHasBeenOptimized) {
        return false;
    }

    return !isInfeasible() && !isUnbounded();
}

template<typename ValueType>
ValueType GlpkLpSolver<ValueType>::getContinuousValue(storm::expressions::Variable const& variable) const {
    if (!this->isOptimal()) {
        STORM_LOG_THROW(!this->isInfeasible(), storm::exceptions::InvalidAccessException, "Unable to get glpk solution from infeasible model.");
        STORM_LOG_THROW(!this->isUnbounded(), storm::exceptions::InvalidAccessException, "Unable to get glpk solution from unbounded model.");
        STORM_LOG_THROW(false, storm::exceptions::InvalidAccessException, "Unable to get glpk solution from unoptimized model.");
    }

    auto variableIndexPair = this->variableToIndexMap.find(variable);
    STORM_LOG_THROW(variableIndexPair != this->variableToIndexMap.end(), storm::exceptions::InvalidAccessException,
                    "Accessing value of unknown variable '" << variable.getName() << "'.");

    double value = 0;
    if (this->modelContainsIntegerVariables) {
        value = glp_mip_col_val(this->lp, static_cast<int>(variableIndexPair->second));
    } else {
        value = glp_get_col_prim(this->lp, static_cast<int>(variableIndexPair->second));
    }
    return storm::utility::convertNumber<ValueType>(value);
}

template<typename ValueType>
int_fast64_t GlpkLpSolver<ValueType>::getIntegerValue(storm::expressions::Variable const& variable) const {
    if (!this->isOptimal()) {
        STORM_LOG_THROW(!this->isInfeasible(), storm::exceptions::InvalidAccessException, "Unable to get glpk solution from infeasible model.");
        STORM_LOG_THROW(!this->isUnbounded(), storm::exceptions::InvalidAccessException, "Unable to get glpk solution from unbounded model.");
        STORM_LOG_THROW(false, storm::exceptions::InvalidAccessException, "Unable to get glpk solution from unoptimized model.");
    }

    auto variableIndexPair = this->variableToIndexMap.find(variable);
    STORM_LOG_THROW(variableIndexPair != this->variableToIndexMap.end(), storm::exceptions::InvalidAccessException,
                    "Accessing value of unknown variable '" << variable.getName() << "'.");

    double value = 0;
    if (this->modelContainsIntegerVariables) {
        value = glp_mip_col_val(this->lp, static_cast<int>(variableIndexPair->second));
    } else {
        value = glp_get_col_prim(this->lp, static_cast<int>(variableIndexPair->second));
    }

    double roundedValue = std::round(value);
    double diff = std::abs(roundedValue - value);
    STORM_LOG_ERROR_COND(diff <= storm::settings::getModule<storm::settings::modules::GlpkSettings>().getIntegerTolerance(),
                         "Illegal value for integer variable in GLPK solution (" << value << "). Difference to nearest int is " << diff);
    return static_cast<int_fast64_t>(roundedValue);
}

template<typename ValueType>
bool GlpkLpSolver<ValueType>::getBinaryValue(storm::expressions::Variable const& variable) const {
    if (!this->isOptimal()) {
        STORM_LOG_THROW(!this->isInfeasible(), storm::exceptions::InvalidAccessException, "Unable to get glpk solution from infeasible model.");
        STORM_LOG_THROW(!this->isUnbounded(), storm::exceptions::InvalidAccessException, "Unable to get glpk solution from unbounded model.");
        STORM_LOG_THROW(false, storm::exceptions::InvalidAccessException, "Unable to get glpk solution from unoptimized model.");
    }

    auto variableIndexPair = this->variableToIndexMap.find(variable);
    STORM_LOG_THROW(variableIndexPair != this->variableToIndexMap.end(), storm::exceptions::InvalidAccessException,
                    "Accessing value of unknown variable '" << variable.getName() << "'.");

    double value = 0;
    if (this->modelContainsIntegerVariables) {
        value = glp_mip_col_val(this->lp, static_cast<int>(variableIndexPair->second));
    } else {
        value = glp_get_col_prim(this->lp, static_cast<int>(variableIndexPair->second));
    }

    if (value > 0.5) {
        STORM_LOG_ERROR_COND(std::abs(value - 1.0) <= storm::settings::getModule<storm::settings::modules::GlpkSettings>().getIntegerTolerance(),
                             "Illegal value for binary variable in GLPK solution (" << value << ").");
        return true;
    } else {
        STORM_LOG_ERROR_COND(std::abs(value) <= storm::settings::getModule<storm::settings::modules::GlpkSettings>().getIntegerTolerance(),
                             "Illegal value for binary variable in GLPK solution (" << value << ").");
        return false;
    }

    return static_cast<bool>(value);
}

template<typename ValueType>
ValueType GlpkLpSolver<ValueType>::getObjectiveValue() const {
    if (!this->isOptimal()) {
        STORM_LOG_THROW(!this->isInfeasible(), storm::exceptions::InvalidAccessException, "Unable to get glpk solution from infeasible model.");
        STORM_LOG_THROW(!this->isUnbounded(), storm::exceptions::InvalidAccessException, "Unable to get glpk solution from unbounded model.");
        STORM_LOG_THROW(false, storm::exceptions::InvalidAccessException, "Unable to get glpk solution from unoptimized model.");
    }

    double value = 0;
    if (this->modelContainsIntegerVariables) {
        value = glp_mip_obj_val(this->lp);
    } else {
        value = glp_get_obj_val(this->lp);
    }

    return storm::utility::convertNumber<ValueType>(value);
}

template<typename ValueType>
void GlpkLpSolver<ValueType>::writeModelToFile(std::string const& filename) const {
    glp_write_lp(this->lp, 0, filename.c_str());
}

template<typename ValueType>
void GlpkLpSolver<ValueType>::push() {
    IncrementalLevel lvl;
    lvl.firstConstraintIndex = glp_get_num_rows(this->lp) + 1;
    incrementalData.push_back(lvl);
}

template<typename ValueType>
void GlpkLpSolver<ValueType>::pop() {
    if (incrementalData.empty()) {
        STORM_LOG_ERROR("Tried to pop from a solver without pushing before.");
    } else {
        IncrementalLevel const& lvl = incrementalData.back();
        // Since glpk uses 1-based indexing, we need to prepend an additional index
        std::vector<int> indicesToBeRemoved = storm::utility::vector::buildVectorForRange(lvl.firstConstraintIndex - 1, glp_get_num_rows(this->lp) + 1);
        if (indicesToBeRemoved.size() > 1) {
            glp_del_rows(this->lp, indicesToBeRemoved.size() - 1, indicesToBeRemoved.data());
        }
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
            // Since glpk uses 1-based indexing, we need to prepend an additional index
            std::vector<int> indicesToBeRemoved = storm::utility::vector::buildVectorForRange(firstIndex - 1, glp_get_num_cols(this->lp) + 1);
            glp_del_cols(this->lp, indicesToBeRemoved.size() - 1, indicesToBeRemoved.data());
        }
        incrementalData.pop_back();
        update();
        // Check whether we need to adapt the current basis (i.e. the number of basic variables does not equal the number of constraints)
        int n = glp_get_num_rows(lp);
        int m = glp_get_num_cols(lp);
        int nb(0), mb(0);
        for (int i = 1; i <= n; ++i) {
            if (glp_get_row_stat(lp, i) == GLP_BS) {
                ++nb;
            }
        }
        for (int j = 1; j <= m; ++j) {
            if (glp_get_col_stat(lp, j) == GLP_BS) {
                ++mb;
            }
        }
        if (n != (nb + mb)) {
            glp_std_basis(this->lp);
        }
    }
}

template<typename ValueType>
void GlpkLpSolver<ValueType>::setMaximalMILPGap(ValueType const& gap, bool relative) {
    this->maxMILPGap = storm::utility::convertNumber<double>(gap);
    this->maxMILPGapRelative = relative;
}

template<typename ValueType>
ValueType GlpkLpSolver<ValueType>::getMILPGap(bool relative) const {
    STORM_LOG_ASSERT(this->isOptimal(), "Asked for the MILP gap although there is no (bounded) solution.");
    if (relative) {
        return storm::utility::convertNumber<ValueType>(this->actualRelativeMILPGap);
    } else {
        return storm::utility::abs<ValueType>(storm::utility::convertNumber<ValueType>(this->actualRelativeMILPGap) * getObjectiveValue());
    }
}

template class GlpkLpSolver<double>;
template class GlpkLpSolver<storm::RationalNumber>;
#endif
}  // namespace solver
}  // namespace storm
