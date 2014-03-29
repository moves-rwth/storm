#include "src/solver/GlpkLpSolver.h"

#ifdef STORM_HAVE_GLPK

#include <iostream>

#include "src/exceptions/InvalidStateException.h"
#include "src/settings/Settings.h"

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"

extern log4cplus::Logger logger;

bool GlpkLpSolverOptionsRegistered = storm::settings::Settings::registerNewModule([] (storm::settings::Settings* instance) -> bool {
	instance->addOption(storm::settings::OptionBuilder("GlpkLpSolver", "glpkoutput", "", "If set, the glpk output will be printed to the command line.").build());
    
    instance->addOption(storm::settings::OptionBuilder("GurobiLpSolver", "glpkinttol", "", "Sets glpk's precision for integer variables.").addArgument(storm::settings::ArgumentBuilder::createDoubleArgument("value", "The precision to achieve.").setDefaultValueDouble(1e-06).addValidationFunctionDouble(storm::settings::ArgumentValidators::doubleRangeValidatorExcluding(0.0, 1.0)).build()).build());

    return true;
});

namespace storm {
    namespace solver {
        GlpkLpSolver::GlpkLpSolver(std::string const& name, ModelSense const& modelSense) : LpSolver(modelSense), lp(nullptr), nextVariableIndex(1), nextConstraintIndex(1), modelContainsIntegerVariables(false), isInfeasibleFlag(false), isUnboundedFlag(false), rowIndices(), columnIndices(), coefficientValues() {
            // Create the LP problem for glpk.
            lp = glp_create_prob();
            
            // Set its name and model sense.
            glp_set_prob_name(lp, name.c_str());
            
            // Set whether the glpk output shall be printed to the command line.
            glp_term_out(storm::settings::Settings::getInstance()->isSet("debug") || storm::settings::Settings::getInstance()->isSet("glpkoutput") ? GLP_ON : GLP_OFF);
            
            // Because glpk uses 1-based indexing (wtf!?), we need to put dummy elements into the matrix vectors.
            rowIndices.push_back(0);
            columnIndices.push_back(0);
            coefficientValues.push_back(0);
        }
        
        GlpkLpSolver::GlpkLpSolver(std::string const& name) : GlpkLpSolver(name, MINIMIZE) {
            // Intentionally left empty.
        }
        
        GlpkLpSolver::GlpkLpSolver() : GlpkLpSolver("", MINIMIZE) {
            // Intentionally left empty.
        }
        
        GlpkLpSolver::GlpkLpSolver(ModelSense const& modelSense) : GlpkLpSolver("", modelSense) {
            // Intentionally left empty.
        }
        
        GlpkLpSolver::~GlpkLpSolver() {
            // Dispose of all objects allocated dynamically by glpk.
            glp_delete_prob(this->lp);
            glp_free_env();
        }
        
        uint_fast64_t GlpkLpSolver::createContinuousVariable(std::string const& name, VariableType const& variableType, double lowerBound, double upperBound, double objectiveFunctionCoefficient) {
            glp_add_cols(this->lp, 1);
            glp_set_col_name(this->lp, nextVariableIndex, name.c_str());
            switch (variableType) {
                case LpSolver::BOUNDED:
                    glp_set_col_bnds(lp, nextVariableIndex, GLP_DB, lowerBound, upperBound);
                    break;
                case LpSolver::UNBOUNDED:
                    glp_set_col_bnds(lp, nextVariableIndex, GLP_FR, 0, 0);
                    break;
                case LpSolver::UPPER_BOUND:
                    glp_set_col_bnds(lp, nextVariableIndex, GLP_UP, 0, upperBound);
                    break;
                case LpSolver::LOWER_BOUND:
                    glp_set_col_bnds(lp, nextVariableIndex, GLP_LO, lowerBound, 0);
                    break;
            }
            glp_set_col_kind(this->lp, nextVariableIndex, GLP_CV);
            glp_set_obj_coef(this->lp, nextVariableIndex, objectiveFunctionCoefficient);
            ++nextVariableIndex;
            
            this->currentModelHasBeenOptimized = false;
            return nextVariableIndex - 1;
        }
        
        uint_fast64_t GlpkLpSolver::createIntegerVariable(std::string const& name, VariableType const& variableType, double lowerBound, double upperBound, double objectiveFunctionCoefficient) {
            uint_fast64_t index = this->createContinuousVariable(name, variableType, lowerBound, upperBound, objectiveFunctionCoefficient);
            glp_set_col_kind(this->lp, index, GLP_IV);
            this->modelContainsIntegerVariables = true;
            return index;
        }
        
        uint_fast64_t GlpkLpSolver::createBinaryVariable(std::string const& name, double objectiveFunctionCoefficient) {
            uint_fast64_t index = this->createContinuousVariable(name, UNBOUNDED, 0, 1, objectiveFunctionCoefficient);
            glp_set_col_kind(this->lp, index, GLP_BV);
            this->modelContainsIntegerVariables = true;
            return index;
        }
        
        void GlpkLpSolver::update() const {
            // Intentionally left empty.
        }
        
        void GlpkLpSolver::addConstraint(std::string const& name, std::vector<uint_fast64_t> const& variables, std::vector<double> const& coefficients, BoundType const& boundType, double rightHandSideValue) {
            if (variables.size() != coefficients.size()) {
                LOG4CPLUS_ERROR(logger, "Sizes of variable indices vector and coefficients vector do not match.");
                throw storm::exceptions::InvalidStateException() << "Sizes of variable indices vector and coefficients vector do not match.";
            }

            // Add the row that will represent this constraint.
            glp_add_rows(this->lp, 1);
            glp_set_row_name(this->lp, nextConstraintIndex, name.c_str());
            
            // Determine the type of the constraint and add it properly.
            switch (boundType) {
                case LESS:
                    glp_set_row_bnds(this->lp, nextConstraintIndex, GLP_UP, 0, rightHandSideValue - storm::settings::Settings::getInstance()->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
                    break;
                case LESS_EQUAL:
                    glp_set_row_bnds(this->lp, nextConstraintIndex, GLP_UP, 0, rightHandSideValue);
                    break;
                case GREATER:
                    glp_set_row_bnds(this->lp, nextConstraintIndex, GLP_LO, rightHandSideValue + storm::settings::Settings::getInstance()->getOptionByLongName("precision").getArgument(0).getValueAsDouble(), 0);
                    break;
                case GREATER_EQUAL:
                    glp_set_row_bnds(this->lp, nextConstraintIndex, GLP_LO, rightHandSideValue, 0);
                    break;
                case EQUAL:
                    glp_set_row_bnds(this->lp, nextConstraintIndex, GLP_FX, rightHandSideValue, rightHandSideValue);
                    break;
            }
            
            // Record the variables and coefficients in the coefficient matrix.
            rowIndices.insert(rowIndices.end(), variables.size(), nextConstraintIndex);
            columnIndices.insert(columnIndices.end(), variables.begin(), variables.end());
            coefficientValues.insert(coefficientValues.end(), coefficients.begin(), coefficients.end());
            
            ++nextConstraintIndex;
            this->currentModelHasBeenOptimized = false;
        }
        
        void GlpkLpSolver::optimize() const {
            // First, reset the flags.
            this->isInfeasibleFlag = false;
            this->isUnboundedFlag = false;
            
            // Start by setting the model sense.
            glp_set_obj_dir(this->lp, this->getModelSense() == MINIMIZE ? GLP_MIN : GLP_MAX);
            
            glp_load_matrix(this->lp, rowIndices.size() - 1, rowIndices.data(), columnIndices.data(), coefficientValues.data());
            
            int error = 0;
            if (this->modelContainsIntegerVariables) {
                glp_iocp* parameters = new glp_iocp;
                glp_init_iocp(parameters);
                parameters->presolve = GLP_ON;
                parameters->tol_int = storm::settings::Settings::getInstance()->getOptionByLongName("glpkinttol").getArgument(0).getValueAsDouble();
                error = glp_intopt(this->lp, parameters);
                delete parameters;
                
                // In case the error is caused by an infeasible problem, we do not want to view this as an error and
                // reset the error code.
                if (error == GLP_ENOPFS) {
                    this->isInfeasibleFlag = true;
                    error = 0;
                } else if (error == GLP_ENODFS) {
                    this->isUnboundedFlag = true;
                    error = 0;
                } else if (error == GLP_EBOUND) {
                    throw storm::exceptions::InvalidStateException() << "The bounds of some variables are illegal. Note that glpk only accepts integer bounds for integer variables.";
                }
            } else {
                error = glp_simplex(this->lp, nullptr);
            }
            
            if (error != 0) {
                LOG4CPLUS_ERROR(logger, "Unable to optimize glpk model (" << error << ").");
                throw storm::exceptions::InvalidStateException() << "Unable to optimize glpk model (" << error << ").";
            }
            
            this->currentModelHasBeenOptimized = true;
        }
        
        bool GlpkLpSolver::isInfeasible() const {
            if (!this->currentModelHasBeenOptimized) {
                throw storm::exceptions::InvalidStateException() << "Illegal call to GlpkLpSolver::isInfeasible: model has not been optimized.";
            }

            if (this->modelContainsIntegerVariables) {
                return isInfeasibleFlag;
            } else {
                return glp_get_status(this->lp) == GLP_INFEAS || glp_get_status(this->lp) == GLP_NOFEAS;
            }
        }
        
        bool GlpkLpSolver::isUnbounded() const {
            if (!this->currentModelHasBeenOptimized) {
                throw storm::exceptions::InvalidStateException() << "Illegal call to GlpkLpSolver::isUnbounded: model has not been optimized.";
            }

            if (this->modelContainsIntegerVariables) {
                return isUnboundedFlag;
            } else {
                return glp_get_status(this->lp) == GLP_UNBND;
            }
        }
        
        bool GlpkLpSolver::isOptimal() const {
            if (!this->currentModelHasBeenOptimized) {
                return false;
            }
            
            int status = 0;
            if (this->modelContainsIntegerVariables) {
                status = glp_mip_status(this->lp);
            } else {
                status = glp_get_status(this->lp);
            }
            return status == GLP_OPT;
        }
        
        int_fast64_t GlpkLpSolver::getIntegerValue(uint_fast64_t variableIndex) const {
            if (!this->isOptimal()) {
                if (this->isInfeasible()) {
                    LOG4CPLUS_ERROR(logger, "Unable to get glpk solution from infeasible model.");
                    throw storm::exceptions::InvalidStateException() << "Unable to get Gurobi solution from infeasible model.";
                } else if (this->isUnbounded()) {
                    LOG4CPLUS_ERROR(logger, "Unable to get glpk solution from unbounded model.");
                    throw storm::exceptions::InvalidStateException() << "Unable to get Gurobi solution from unbounded model.";
                } else {
                    LOG4CPLUS_ERROR(logger, "Unable to get glpk solution from unoptimized model.");
                    throw storm::exceptions::InvalidStateException() << "Unable to get Gurobi solution from unoptimized model.";
                }
            }
           
            double value = 0;
            if (this->modelContainsIntegerVariables) {
                value = glp_mip_col_val(this->lp, static_cast<int>(variableIndex));
            } else {
                value = glp_get_col_prim(this->lp, static_cast<int>(variableIndex));
            }

            if (std::abs(value - static_cast<int>(value)) <= storm::settings::Settings::getInstance()->getOptionByLongName("precision").getArgument(0).getValueAsDouble()) {
                // Nothing to do in this case.
            } else if (std::abs(value) > storm::settings::Settings::getInstance()->getOptionByLongName("precision").getArgument(0).getValueAsDouble()) {
                LOG4CPLUS_ERROR(logger, "Illegal value for integer variable in glpk solution (" << value << ").");
                throw storm::exceptions::InvalidStateException() << "Illegal value for integer variable in glpk solution (" << value << ").";
            }
            
            return static_cast<int_fast64_t>(value);
        }
        
        bool GlpkLpSolver::getBinaryValue(uint_fast64_t variableIndex) const {
            if (!this->isOptimal()) {
                if (this->isInfeasible()) {
                    LOG4CPLUS_ERROR(logger, "Unable to get glpk solution from infeasible model.");
                    throw storm::exceptions::InvalidStateException() << "Unable to get Gurobi solution from infeasible model.";
                } else if (this->isUnbounded()) {
                    LOG4CPLUS_ERROR(logger, "Unable to get glpk solution from unbounded model.");
                    throw storm::exceptions::InvalidStateException() << "Unable to get Gurobi solution from unbounded model.";
                } else {
                    LOG4CPLUS_ERROR(logger, "Unable to get glpk solution from unoptimized model.");
                    throw storm::exceptions::InvalidStateException() << "Unable to get Gurobi solution from unoptimized model.";
                }
            }

            double value = 0;
            if (this->modelContainsIntegerVariables) {
                value = glp_mip_col_val(this->lp, static_cast<int>(variableIndex));
            } else {
                value = glp_get_col_prim(this->lp, static_cast<int>(variableIndex));
            }

            if (std::abs(value - 1) <= storm::settings::Settings::getInstance()->getOptionByLongName("precision").getArgument(0).getValueAsDouble()) {
                // Nothing to do in this case.
            } else if (std::abs(value) > storm::settings::Settings::getInstance()->getOptionByLongName("precision").getArgument(0).getValueAsDouble()) {
                LOG4CPLUS_ERROR(logger, "Illegal value for binary variable in Gurobi solution (" << value << ").");
                throw storm::exceptions::InvalidStateException() << "Illegal value for binary variable in Gurobi solution (" << value << ").";
            }
            
            return static_cast<bool>(value);
        }
        
        double GlpkLpSolver::getContinuousValue(uint_fast64_t variableIndex) const {
            if (!this->isOptimal()) {
                if (this->isInfeasible()) {
                    LOG4CPLUS_ERROR(logger, "Unable to get glpk solution from infeasible model.");
                    throw storm::exceptions::InvalidStateException() << "Unable to get Gurobi solution from infeasible model.";
                } else if (this->isUnbounded()) {
                    LOG4CPLUS_ERROR(logger, "Unable to get glpk solution from unbounded model.");
                    throw storm::exceptions::InvalidStateException() << "Unable to get Gurobi solution from unbounded model.";
                } else {
                    LOG4CPLUS_ERROR(logger, "Unable to get glpk solution from unoptimized model.");
                    throw storm::exceptions::InvalidStateException() << "Unable to get Gurobi solution from unoptimized model.";
                }
            }

            double value = 0;
            if (this->modelContainsIntegerVariables) {
                value = glp_mip_col_val(this->lp, static_cast<int>(variableIndex));
            } else {
                value = glp_get_col_prim(this->lp, static_cast<int>(variableIndex));
            }
            return value;
        }
        
        double GlpkLpSolver::getObjectiveValue() const {
            if (!this->isOptimal()) {
                if (this->isInfeasible()) {
                    LOG4CPLUS_ERROR(logger, "Unable to get glpk solution from infeasible model.");
                    throw storm::exceptions::InvalidStateException() << "Unable to get Gurobi solution from infeasible model.";
                } else if (this->isUnbounded()) {
                    LOG4CPLUS_ERROR(logger, "Unable to get glpk solution from unbounded model.");
                    throw storm::exceptions::InvalidStateException() << "Unable to get Gurobi solution from unbounded model.";
                } else {
                    LOG4CPLUS_ERROR(logger, "Unable to get glpk solution from unoptimized model.");
                    throw storm::exceptions::InvalidStateException() << "Unable to get Gurobi solution from unoptimized model.";
                }
            }

            double value = 0;
            if (this->modelContainsIntegerVariables) {
                value = glp_mip_obj_val(this->lp);
            } else {
                value = glp_get_obj_val(this->lp);
            }

            return value;
        }
        
        void GlpkLpSolver::writeModelToFile(std::string const& filename) const {
            glp_load_matrix(this->lp, rowIndices.size() - 1, rowIndices.data(), columnIndices.data(), coefficientValues.data());
            glp_write_lp(this->lp, 0, filename.c_str());
        }
    }
}

#endif