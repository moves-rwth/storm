#include "src/solver/GurobiLpSolver.h"

#ifdef STORM_HAVE_GUROBI

#include "src/exceptions/InvalidStateException.h"
#include "src/settings/Settings.h"

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"

extern log4cplus::Logger logger;

bool GurobiLpSolverOptionsRegistered = storm::settings::Settings::registerNewModule([] (storm::settings::Settings* instance) -> bool {
    instance->addOption(storm::settings::OptionBuilder("GurobiLpSolver", "gurobithreads", "", "The number of threads that may be used by Gurobi.").addArgument(storm::settings::ArgumentBuilder::createUnsignedIntegerArgument("count", "The number of threads.").setDefaultValueUnsignedInteger(1).build()).build());
    
	instance->addOption(storm::settings::OptionBuilder("GurobiLpSolver", "gurobioutput", "", "If set, the Gurobi output will be printed to the command line.").build());
    
	instance->addOption(storm::settings::OptionBuilder("GurobiLpSolver", "gurobiinttol", "", "Sets Gurobi's precision for integer variables.").addArgument(storm::settings::ArgumentBuilder::createDoubleArgument("value", "The precision to achieve.").setDefaultValueDouble(1e-06).addValidationFunctionDouble(storm::settings::ArgumentValidators::doubleRangeValidatorExcluding(0.0, 1.0)).build()).build());
    
    return true;
});

namespace storm {
    namespace solver {
        
        GurobiLpSolver::GurobiLpSolver(std::string const& name, ModelSense const& modelSense) : LpSolver(modelSense), env(nullptr), model(nullptr), nextVariableIndex(0) {
            // Create the environment.
            int error = GRBloadenv(&env, "");
            if (error || env == nullptr) {
				LOG4CPLUS_ERROR(logger, "Could not initialize Gurobi (" << GRBgeterrormsg(env) << ", error code " << error << ").");
				throw storm::exceptions::InvalidStateException() << "Could not initialize Gurobi environment (" << GRBgeterrormsg(env) << ", error code " << error << ").";
            }
            
            // Set some general properties of the environment.
            setGurobiEnvironmentProperties();
            
            // Create the model.
            error = GRBnewmodel(env, &model, name.c_str(), 0, nullptr, nullptr, nullptr, nullptr, nullptr);
            if (error) {
				LOG4CPLUS_ERROR(logger, "Could not initialize Gurobi model (" << GRBgeterrormsg(env) << ", error code " << error << ").");
				throw storm::exceptions::InvalidStateException() << "Could not initialize Gurobi model (" << GRBgeterrormsg(env) << ", error code " << error << ").";
            }
        }
        
        GurobiLpSolver::GurobiLpSolver(std::string const& name) : GurobiLpSolver(name, MINIMIZE) {
            // Intentionally left empty.
        }
        
        GurobiLpSolver::GurobiLpSolver(ModelSense const& modelSense) : GurobiLpSolver("", modelSense) {
            // Intentionally left empty.
        }
        
        GurobiLpSolver::GurobiLpSolver() : GurobiLpSolver("", MINIMIZE) {
            // Intentionally left empty.
        }
        
        GurobiLpSolver::~GurobiLpSolver() {
            // Dispose of the objects allocated inside Gurobi.
            GRBfreemodel(model);
            GRBfreeenv(env);
        }
        
        void GurobiLpSolver::setGurobiEnvironmentProperties() const {
			int error = 0;

			// Enable the following line to only print the output of Gurobi if the debug flag is set.
            error = GRBsetintparam(env, "OutputFlag", storm::settings::Settings::getInstance()->isSet("debug") || storm::settings::Settings::getInstance()->isSet("gurobioutput") ? 1 : 0);
			if (error) {
				LOG4CPLUS_ERROR(logger, "Unable to set Gurobi Parameter OutputFlag (" << GRBgeterrormsg(env) << ", error code " << error << ").");
				throw storm::exceptions::InvalidStateException() << "Unable to set Gurobi Parameter OutputFlag (" << GRBgeterrormsg(env) << ", error code " << error << ").";
			}
            
            // Enable the following line to restrict Gurobi to one thread only.
            error = GRBsetintparam(env, "Threads", storm::settings::Settings::getInstance()->getOptionByLongName("gurobithreads").getArgument(0).getValueAsUnsignedInteger());
			if (error) {
				LOG4CPLUS_ERROR(logger, "Unable to set Gurobi Parameter Threads (" << GRBgeterrormsg(env) << ", error code " << error << ").");
				throw storm::exceptions::InvalidStateException() << "Unable to set Gurobi Parameter Threads (" << GRBgeterrormsg(env) << ", error code " << error << ").";
			}

            // Enable the following line to force Gurobi to be as precise about the binary variables as required by the given precision option.
            error = GRBsetdblparam(env, "IntFeasTol", storm::settings::Settings::getInstance()->getOptionByLongName("gurobiinttol").getArgument(0).getValueAsDouble());
			if (error) {
				LOG4CPLUS_ERROR(logger, "Unable to set Gurobi Parameter IntFeasTol (" << GRBgeterrormsg(env) << ", error code " << error << ").");
				throw storm::exceptions::InvalidStateException() << "Unable to set Gurobi Parameter IntFeasTol (" << GRBgeterrormsg(env) << ", error code " << error << ").";
			}
        }
        
        void GurobiLpSolver::update() const {
            int error = GRBupdatemodel(model);
            if (error) {
				LOG4CPLUS_ERROR(logger, "Unable to update Gurobi model (" << GRBgeterrormsg(env) << ", error code " << error << ").");
				throw storm::exceptions::InvalidStateException() << "Unable to update Gurobi model (" << GRBgeterrormsg(env) << ", error code " << error << ").";
            }
            
            // Since the model changed, we erase the optimality flag.
            this->currentModelHasBeenOptimized = false;
        }
        
        uint_fast64_t GurobiLpSolver::createContinuousVariable(std::string const& name, VariableType const& variableType, double lowerBound, double upperBound, double objectiveFunctionCoefficient) {
            int error = 0;
            switch (variableType) {
                case LpSolver::BOUNDED:
                    error = GRBaddvar(model, 0, nullptr, nullptr, objectiveFunctionCoefficient, lowerBound, upperBound, GRB_CONTINUOUS, name.c_str());
                    break;
                case LpSolver::UNBOUNDED:
                    error = GRBaddvar(model, 0, nullptr, nullptr, objectiveFunctionCoefficient, -GRB_INFINITY, GRB_INFINITY, GRB_CONTINUOUS, name.c_str());
                    break;
                case LpSolver::UPPER_BOUND:
                    error = GRBaddvar(model, 0, nullptr, nullptr, objectiveFunctionCoefficient, -GRB_INFINITY, upperBound, GRB_CONTINUOUS, name.c_str());
                    break;
                case LpSolver::LOWER_BOUND:
                    error = GRBaddvar(model, 0, nullptr, nullptr, objectiveFunctionCoefficient, lowerBound, GRB_INFINITY, GRB_CONTINUOUS, name.c_str());
                    break;
            }
            
            if (error) {
				LOG4CPLUS_ERROR(logger, "Could not create binary Gurobi variable (" << GRBgeterrormsg(env) << ", error code " << error << ").");
				throw storm::exceptions::InvalidStateException() << "Could not create binary Gurobi variable (" << GRBgeterrormsg(env) << ", error code " << error << ").";
            }
            ++nextVariableIndex;
            return nextVariableIndex - 1;
        }
        
        uint_fast64_t GurobiLpSolver::createIntegerVariable(std::string const& name, VariableType const& variableType, double lowerBound, double upperBound, double objectiveFunctionCoefficient) {
            int error = 0;
            switch (variableType) {
                case LpSolver::BOUNDED:
                    error = GRBaddvar(model, 0, nullptr, nullptr, objectiveFunctionCoefficient, lowerBound, upperBound, GRB_INTEGER, name.c_str());
                    break;
                case LpSolver::UNBOUNDED:
                    error = GRBaddvar(model, 0, nullptr, nullptr, objectiveFunctionCoefficient, -GRB_INFINITY, GRB_INFINITY, GRB_INTEGER, name.c_str());
                    break;
                case LpSolver::UPPER_BOUND:
                    error = GRBaddvar(model, 0, nullptr, nullptr, objectiveFunctionCoefficient, -GRB_INFINITY, upperBound, GRB_INTEGER, name.c_str());
                    break;
                case LpSolver::LOWER_BOUND:
                    error = GRBaddvar(model, 0, nullptr, nullptr, objectiveFunctionCoefficient, lowerBound, GRB_INFINITY, GRB_INTEGER, name.c_str());
                    break;
            }

            if (error) {
				LOG4CPLUS_ERROR(logger, "Could not create binary Gurobi variable (" << GRBgeterrormsg(env) << ", error code " << error << ").");
				throw storm::exceptions::InvalidStateException() << "Could not create binary Gurobi variable (" << GRBgeterrormsg(env) << ", error code " << error << ").";
            }
            ++nextVariableIndex;
            return nextVariableIndex - 1;
        }
        
        uint_fast64_t GurobiLpSolver::createBinaryVariable(std::string const& name, double objectiveFunctionCoefficient) {
            int error = GRBaddvar(model, 0, nullptr, nullptr, objectiveFunctionCoefficient, 0.0, 1.0, GRB_BINARY, name.c_str());
            if (error) {
				LOG4CPLUS_ERROR(logger, "Could not create binary Gurobi variable (" << GRBgeterrormsg(env) << ", error code " << error << ").");
				throw storm::exceptions::InvalidStateException() << "Could not create binary Gurobi variable (" << GRBgeterrormsg(env) << ", error code " << error << ").";
            }
            ++nextVariableIndex;
            return nextVariableIndex - 1;
        }
        
        void GurobiLpSolver::addConstraint(std::string const& name, std::vector<uint_fast64_t> const& variables, std::vector<double> const& coefficients, BoundType const& boundType, double rightHandSideValue) {
            if (variables.size() != coefficients.size()) {
                LOG4CPLUS_ERROR(logger, "Sizes of variable indices vector and coefficients vector do not match.");
                throw storm::exceptions::InvalidStateException() << "Sizes of variable indices vector and coefficients vector do not match.";
            }
            
            // Convert the uint vector to ints for proper input to Gurobi.
            std::vector<int> variablesCopy(variables.begin(), variables.end());
            
            // Copy the coefficients, because Gurobi does not take the coefficients as const values. The alternative to this would be casting
            // away the constness, which gives undefined behaviour if Gurobi actually modifies something.
            std::vector<double> coefficientsCopy(coefficients);
            
            bool strictBound = boundType == LESS || boundType == GREATER;
            char sense = boundType == LESS || boundType == LESS_EQUAL ? GRB_LESS_EQUAL : boundType == EQUAL ? GRB_EQUAL : GRB_GREATER_EQUAL;
            
            // If the constraint enforces a strict bound, we need to do some tweaking of the right-hand side value, because Gurobi only supports
            // non-strict bounds.
            if (strictBound) {
                if (boundType == LESS) {
                    rightHandSideValue -= storm::settings::Settings::getInstance()->getOptionByLongName("precision").getArgument(0).getValueAsDouble();
                } else if (boundType == GREATER) {
                    rightHandSideValue += storm::settings::Settings::getInstance()->getOptionByLongName("precision").getArgument(0).getValueAsDouble();
                }
            }
            int error = GRBaddconstr(model, variablesCopy.size(), variablesCopy.data(), coefficientsCopy.data(), sense, rightHandSideValue, name == "" ? nullptr : name.c_str());
            
            if (error) {
                LOG4CPLUS_ERROR(logger, "Unable to assert Gurobi constraint (" << GRBgeterrormsg(env) << ", error code " << error << ").");
				throw storm::exceptions::InvalidStateException() << "Unable to assert Gurobi constraint (" << GRBgeterrormsg(env) << ", error code " << error << ").";
            }
        }
        
        void GurobiLpSolver::optimize() const {
            // First incorporate all recent changes.
            this->update();
         
            // Set the most recently set model sense.
            int error = GRBsetintattr(model, "ModelSense", this->getModelSense() == MINIMIZE ? 1 : -1);
            if (error) {
				LOG4CPLUS_ERROR(logger, "Unable to set Gurobi model sense (" << GRBgeterrormsg(env) << ", error code " << error << ").");
				throw storm::exceptions::InvalidStateException() << "Unable to set Gurobi model sense (" << GRBgeterrormsg(env) << ", error code " << error << ").";
            }
            
            // Then we actually optimize the model.
            error = GRBoptimize(model);
            if (error) {
				LOG4CPLUS_ERROR(logger, "Unable to optimize Gurobi model (" << GRBgeterrormsg(env) << ", error code " << error << ").");
				throw storm::exceptions::InvalidStateException() << "Unable to optimize Gurobi model (" << GRBgeterrormsg(env) << ", error code " << error << ").";
            }
            
            this->currentModelHasBeenOptimized = true;
        }
        
        bool GurobiLpSolver::isInfeasible() const {
            if (!this->currentModelHasBeenOptimized) {
                throw storm::exceptions::InvalidStateException() << "Illegal call to GurobiLpSolver::isInfeasible: model has not been optimized.";
            }

            int optimalityStatus = 0;
            
            int error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimalityStatus);
            if (error) {
				LOG4CPLUS_ERROR(logger, "Unable to retrieve optimization status of Gurobi model (" << GRBgeterrormsg(env) << ", error code " << error << ").");
				throw storm::exceptions::InvalidStateException() << "Unable to retrieve optimization status of Gurobi model (" << GRBgeterrormsg(env) << ", error code " << error << ").";
            }
            
            // By default, Gurobi may tell us only that the model is either infeasible or unbounded. To decide which one
            // it is, we need to perform an extra step.
            if (optimalityStatus == GRB_INF_OR_UNBD) {
                std::cout << "here" << std::endl;
                error = GRBsetintparam(GRBgetenv(model), GRB_INT_PAR_DUALREDUCTIONS, 0);
                if (error) {
					LOG4CPLUS_ERROR(logger, "Unable to set Gurobi parameter (" << GRBgeterrormsg(env) << ", error code " << error << ").");
					throw storm::exceptions::InvalidStateException() << "Unable to set Gurobi parameter (" << GRBgeterrormsg(env) << ", error code " << error << ").";
                }
                
                this->optimize();
                
                error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimalityStatus);
                if (error) {
					LOG4CPLUS_ERROR(logger, "Unable to retrieve optimization status of Gurobi model (" << GRBgeterrormsg(env) << ", error code " << error << ").");
					throw storm::exceptions::InvalidStateException() << "Unable to retrieve optimization status of Gurobi model (" << GRBgeterrormsg(env) << ", error code " << error << ").";
                }
                
                error = GRBsetintparam(GRBgetenv(model), GRB_INT_PAR_DUALREDUCTIONS, 1);
                if (error) {
					LOG4CPLUS_ERROR(logger, "Unable to set Gurobi parameter (" << GRBgeterrormsg(env) << ", error code " << error << ").");
					throw storm::exceptions::InvalidStateException() << "Unable to set Gurobi parameter (" << GRBgeterrormsg(env) << ", error code " << error << ").";
                }
            }
            
            return optimalityStatus == GRB_INFEASIBLE;
        }
        
        bool GurobiLpSolver::isUnbounded() const {
            if (!this->currentModelHasBeenOptimized) {
                throw storm::exceptions::InvalidStateException() << "Illegal call to GurobiLpSolver::isUnbounded: model has not been optimized.";
            }

            int optimalityStatus = 0;
            
            int error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimalityStatus);
            if (error) {
				LOG4CPLUS_ERROR(logger, "Unable to retrieve optimization status of Gurobi model (" << GRBgeterrormsg(env) << ", error code " << error << ").");
				throw storm::exceptions::InvalidStateException() << "Unable to retrieve optimization status of Gurobi model (" << GRBgeterrormsg(env) << ", error code " << error << ").";
            }
            
            // By default, Gurobi may tell us only that the model is either infeasible or unbounded. To decide which one
            // it is, we need to perform an extra step.
            if (optimalityStatus == GRB_INF_OR_UNBD) {
                error = GRBsetintparam(GRBgetenv(model), GRB_INT_PAR_DUALREDUCTIONS, 0);
                if (error) {
					LOG4CPLUS_ERROR(logger, "Unable to set Gurobi parameter (" << GRBgeterrormsg(env) << ", error code " << error << ").");
					throw storm::exceptions::InvalidStateException() << "Unable to set Gurobi parameter (" << GRBgeterrormsg(env) << ", error code " << error << ").";
                }
                
                this->optimize();

                error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimalityStatus);
                if (error) {
					LOG4CPLUS_ERROR(logger, "Unable to retrieve optimization status of Gurobi model (" << GRBgeterrormsg(env) << ", error code " << error << ").");
					throw storm::exceptions::InvalidStateException() << "Unable to retrieve optimization status of Gurobi model (" << GRBgeterrormsg(env) << ", error code " << error << ").";
                }

                error = GRBsetintparam(GRBgetenv(model), GRB_INT_PAR_DUALREDUCTIONS, 1);
                if (error) {
					LOG4CPLUS_ERROR(logger, "Unable to set Gurobi parameter (" << GRBgeterrormsg(env) << ", error code " << error << ").");
					throw storm::exceptions::InvalidStateException() << "Unable to set Gurobi parameter (" << GRBgeterrormsg(env) << ", error code " << error << ").";
                }
            }
            
            return optimalityStatus == GRB_UNBOUNDED;
        }
        
        bool GurobiLpSolver::isOptimal() const {
            if (!this->currentModelHasBeenOptimized) {
                return false;
            }
            int optimalityStatus = 0;
            
            int error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimalityStatus);
            if (error) {
				LOG4CPLUS_ERROR(logger, "Unable to retrieve optimization status of Gurobi model (" << GRBgeterrormsg(env) << ", error code " << error << ").");
				throw storm::exceptions::InvalidStateException() << "Unable to retrieve optimization status of Gurobi model (" << GRBgeterrormsg(env) << ", error code " << error << ").";
            }
            
            return optimalityStatus == GRB_OPTIMAL;
        }
        
        int_fast64_t GurobiLpSolver::getIntegerValue(uint_fast64_t variableIndex) const {
            if (!this->isOptimal()) {
                if (this->isInfeasible()) {
                    LOG4CPLUS_ERROR(logger, "Unable to get Gurobi solution from infeasible model (" << GRBgeterrormsg(env) << ").");
                    throw storm::exceptions::InvalidStateException() << "Unable to get Gurobi solution from infeasible model (" << GRBgeterrormsg(env) << ").";
                } else if (this->isUnbounded()) {
                    LOG4CPLUS_ERROR(logger, "Unable to get Gurobi solution from unbounded model (" << GRBgeterrormsg(env) << ").");
                    throw storm::exceptions::InvalidStateException() << "Unable to get Gurobi solution from unbounded model (" << GRBgeterrormsg(env) << ").";
                } else {
                    LOG4CPLUS_ERROR(logger, "Unable to get Gurobi solution from unoptimized model (" << GRBgeterrormsg(env) << ").");
                    throw storm::exceptions::InvalidStateException() << "Unable to get Gurobi solution from unoptimized model (" << GRBgeterrormsg(env) << ").";
                }
            }
            
            double value = 0;
            int error = GRBgetdblattrelement(model, GRB_DBL_ATTR_X, variableIndex, &value);
            if (error) {
				LOG4CPLUS_ERROR(logger, "Unable to get Gurobi solution (" << GRBgeterrormsg(env) << ", error code " << error << ").");
				throw storm::exceptions::InvalidStateException() << "Unable to get Gurobi solution (" << GRBgeterrormsg(env) << ", error code " << error << ").";
            }
            
            if (std::abs(value - static_cast<int>(value)) <= storm::settings::Settings::getInstance()->getOptionByLongName("gurobiinttol").getArgument(0).getValueAsDouble()) {
                // Nothing to do in this case.
            } else if (std::abs(value) > storm::settings::Settings::getInstance()->getOptionByLongName("gurobiinttol").getArgument(0).getValueAsDouble()) {
                LOG4CPLUS_ERROR(logger, "Illegal value for integer variable in Gurobi solution (" << value << ").");
                throw storm::exceptions::InvalidStateException() << "Illegal value for integer variable in Gurobi solution (" << value << ").";
            }
            
            return static_cast<int_fast64_t>(value);
        }
        
        bool GurobiLpSolver::getBinaryValue(uint_fast64_t variableIndex) const {
            if (!this->isOptimal()) {
                if (this->isInfeasible()) {
                    LOG4CPLUS_ERROR(logger, "Unable to get Gurobi solution from infeasible model (" << GRBgeterrormsg(env) << ").");
                    throw storm::exceptions::InvalidStateException() << "Unable to get Gurobi solution from infeasible model (" << GRBgeterrormsg(env) << ").";
                } else if (this->isUnbounded()) {
                    LOG4CPLUS_ERROR(logger, "Unable to get Gurobi solution from unbounded model (" << GRBgeterrormsg(env) << ").");
                    throw storm::exceptions::InvalidStateException() << "Unable to get Gurobi solution from unbounded model (" << GRBgeterrormsg(env) << ").";
                } else {
                    LOG4CPLUS_ERROR(logger, "Unable to get Gurobi solution from unoptimized model (" << GRBgeterrormsg(env) << ").");
                    throw storm::exceptions::InvalidStateException() << "Unable to get Gurobi solution from unoptimized model (" << GRBgeterrormsg(env) << ").";
                }
            }

            double value = 0;
            int error = GRBgetdblattrelement(model, GRB_DBL_ATTR_X, variableIndex, &value);
            if (error) {
				LOG4CPLUS_ERROR(logger, "Unable to get Gurobi solution (" << GRBgeterrormsg(env) << ", error code " << error << ").");
				throw storm::exceptions::InvalidStateException() << "Unable to get Gurobi solution (" << GRBgeterrormsg(env) << ", error code " << error << ").";
            }
            
            if (std::abs(value - 1) <= storm::settings::Settings::getInstance()->getOptionByLongName("gurobiinttol").getArgument(0).getValueAsDouble()) {
                // Nothing to do in this case.
            } else if (std::abs(value) > storm::settings::Settings::getInstance()->getOptionByLongName("gurobiinttol").getArgument(0).getValueAsDouble()) {
                LOG4CPLUS_ERROR(logger, "Illegal value for binary variable in Gurobi solution (" << value << ").");
                throw storm::exceptions::InvalidStateException() << "Illegal value for binary variable in Gurobi solution (" << value << ").";
            }
            
            return static_cast<bool>(value);
        }
        
        double GurobiLpSolver::getContinuousValue(uint_fast64_t variableIndex) const {
            if (!this->isOptimal()) {
                if (this->isInfeasible()) {
                    LOG4CPLUS_ERROR(logger, "Unable to get Gurobi solution from infeasible model (" << GRBgeterrormsg(env) << ").");
                    throw storm::exceptions::InvalidStateException() << "Unable to get Gurobi solution from infeasible model (" << GRBgeterrormsg(env) << ").";
                } else if (this->isUnbounded()) {
                    LOG4CPLUS_ERROR(logger, "Unable to get Gurobi solution from unbounded model (" << GRBgeterrormsg(env) << ").");
                    throw storm::exceptions::InvalidStateException() << "Unable to get Gurobi solution from unbounded model (" << GRBgeterrormsg(env) << ").";
                } else {
                    LOG4CPLUS_ERROR(logger, "Unable to get Gurobi solution from unoptimized model (" << GRBgeterrormsg(env) << ").");
                    throw storm::exceptions::InvalidStateException() << "Unable to get Gurobi solution from unoptimized model (" << GRBgeterrormsg(env) << ").";
                }
            }

            double value = 0;
            int error = GRBgetdblattrelement(model, GRB_DBL_ATTR_X, variableIndex, &value);
            if (error) {
				LOG4CPLUS_ERROR(logger, "Unable to get Gurobi solution (" << GRBgeterrormsg(env) << ", error code " << error << ").");
				throw storm::exceptions::InvalidStateException() << "Unable to get Gurobi solution (" << GRBgeterrormsg(env) << ", error code " << error << ").";
            }
            
            return value;
        }
        
        double GurobiLpSolver::getObjectiveValue() const {
            if (!this->isOptimal()) {
                if (this->isInfeasible()) {
                    LOG4CPLUS_ERROR(logger, "Unable to get Gurobi solution from infeasible model (" << GRBgeterrormsg(env) << ").");
                    throw storm::exceptions::InvalidStateException() << "Unable to get Gurobi solution from infeasible model (" << GRBgeterrormsg(env) << ").";
                } else if (this->isUnbounded()) {
                    LOG4CPLUS_ERROR(logger, "Unable to get Gurobi solution from unbounded model (" << GRBgeterrormsg(env) << ").");
                    throw storm::exceptions::InvalidStateException() << "Unable to get Gurobi solution from unbounded model (" << GRBgeterrormsg(env) << ").";
                } else {
                    LOG4CPLUS_ERROR(logger, "Unable to get Gurobi solution from unoptimized model (" << GRBgeterrormsg(env) << ").");
                    throw storm::exceptions::InvalidStateException() << "Unable to get Gurobi solution from unoptimized model (" << GRBgeterrormsg(env) << ").";
                }
            }
            
            double value = 0;
            int error = GRBgetdblattr(model, GRB_DBL_ATTR_OBJVAL, &value);
            if (error) {
				LOG4CPLUS_ERROR(logger, "Unable to get Gurobi solution (" << GRBgeterrormsg(env) << ", error code " << error << ").");
				throw storm::exceptions::InvalidStateException() << "Unable to get Gurobi solution (" << GRBgeterrormsg(env) << ", error code " << error << ").";
            }
            
            return value;
        }
        
        void GurobiLpSolver::writeModelToFile(std::string const& filename) const {
            int error = GRBwrite(model, filename.c_str());
            if (error) {
				LOG4CPLUS_ERROR(logger, "Unable to write Gurobi model (" << GRBgeterrormsg(env) << ", error code " << error << ") to file.");
				throw storm::exceptions::InvalidStateException() << "Unable to write Gurobi model (" << GRBgeterrormsg(env) << ", error code " << error << ") to file.";
            }
        }
    }
}

#endif