#include "storm/solver/Z3LpSolver.h"

#include <numeric>

#include "storm/storage/expressions/LinearCoefficientVisitor.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/DebugSettings.h"

#include "storm/utility/macros.h"
#include "storm/utility/constants.h"
#include "storm/storage/expressions/Expression.h"
#include "storm/storage/expressions/ExpressionManager.h"

#include "storm/exceptions/InvalidStateException.h"
#include "storm/exceptions/InvalidAccessException.h"
#include "storm/exceptions/InvalidArgumentException.h"
#include "storm/exceptions/NotImplementedException.h"
#include "storm/exceptions/ExpressionEvaluationException.h"


namespace storm {
    namespace solver {
        
#ifdef STORM_HAVE_Z3_OPTIMIZE
        Z3LpSolver::Z3LpSolver(std::string const& name, OptimizationDirection const& optDir) : LpSolver(optDir) {
            STORM_LOG_WARN_COND(name == "", "Z3 does not support names for solvers");
            z3::config config;
            config.set("model", true);
            context = std::unique_ptr<z3::context>(new z3::context(config));
            solver = std::unique_ptr<z3::optimize>(new z3::optimize(*context));
            expressionAdapter = std::unique_ptr<storm::adapters::Z3ExpressionAdapter>(new storm::adapters::Z3ExpressionAdapter(*this->manager, *context));
            optimizationFunction = this->getManager().rational(storm::utility::zero<storm::RationalNumber>());
        }
        
        Z3LpSolver::Z3LpSolver(std::string const& name) : Z3LpSolver(name, OptimizationDirection::Minimize) {
            // Intentionally left empty.
        }
        
        Z3LpSolver::Z3LpSolver(OptimizationDirection const& optDir) : Z3LpSolver("", optDir) {
            // Intentionally left empty.
        }
        
        Z3LpSolver::Z3LpSolver() : Z3LpSolver("", OptimizationDirection::Minimize) {
            // Intentionally left empty.
        }
        
        Z3LpSolver::~Z3LpSolver() {
            // Intentionally left empty.
        }
        
        void Z3LpSolver::update() const {
            // Since the model changed, we erase the optimality flag.
            lastCheckObjectiveValue.reset(nullptr);
            lastCheckModel.reset(nullptr);
            this->currentModelHasBeenOptimized = false;
        }
        
        storm::expressions::Variable Z3LpSolver::addBoundedContinuousVariable(std::string const& name, double lowerBound, double upperBound, double objectiveFunctionCoefficient) {
            storm::expressions::Variable newVariable = this->manager->declareVariable(name, manager->getRationalType());
            solver->add(expressionAdapter->translateExpression((newVariable.getExpression() >= this->manager->rational(lowerBound)) && (newVariable.getExpression() <= this->manager->rational(upperBound))));
            optimizationFunction = optimizationFunction + this->manager->rational(objectiveFunctionCoefficient) * newVariable;
            return newVariable;
        }
        
        storm::expressions::Variable Z3LpSolver::addLowerBoundedContinuousVariable(std::string const& name, double lowerBound, double objectiveFunctionCoefficient) {
            storm::expressions::Variable newVariable = this->manager->declareVariable(name, manager->getRationalType());
            solver->add(expressionAdapter->translateExpression(newVariable.getExpression() >= this->manager->rational(lowerBound)));
            optimizationFunction = optimizationFunction + this->manager->rational(objectiveFunctionCoefficient) * newVariable;
            return newVariable;
        }

        storm::expressions::Variable Z3LpSolver::addUpperBoundedContinuousVariable(std::string const& name, double upperBound, double objectiveFunctionCoefficient) {
            storm::expressions::Variable newVariable = this->manager->declareVariable(name, manager->getRationalType());
            solver->add(expressionAdapter->translateExpression(newVariable.getExpression() <= this->manager->rational(upperBound)));
            optimizationFunction = optimizationFunction + this->manager->rational(objectiveFunctionCoefficient) * newVariable;
            return newVariable;
        }

        storm::expressions::Variable Z3LpSolver::addUnboundedContinuousVariable(std::string const& name, double objectiveFunctionCoefficient) {
            storm::expressions::Variable newVariable = this->manager->declareVariable(name, manager->getRationalType());
            optimizationFunction = optimizationFunction + this->manager->rational(objectiveFunctionCoefficient) * newVariable;
            return newVariable;
        }
        
        storm::expressions::Variable Z3LpSolver::addBoundedIntegerVariable(std::string const& name, double lowerBound, double upperBound, double objectiveFunctionCoefficient) {
            storm::expressions::Variable newVariable = this->manager->declareVariable(name, manager->getIntegerType());
            solver->add(expressionAdapter->translateExpression((newVariable.getExpression() >= this->manager->rational(lowerBound)) && (newVariable.getExpression() <= this->manager->rational(upperBound))));
            optimizationFunction = optimizationFunction + this->manager->rational(objectiveFunctionCoefficient) * newVariable;
            return newVariable;
        }
        
        storm::expressions::Variable Z3LpSolver::addLowerBoundedIntegerVariable(std::string const& name, double lowerBound, double objectiveFunctionCoefficient) {
            storm::expressions::Variable newVariable = this->manager->declareVariable(name, manager->getIntegerType());
            solver->add(expressionAdapter->translateExpression(newVariable.getExpression() >= this->manager->rational(lowerBound)));
            optimizationFunction = optimizationFunction + this->manager->rational(objectiveFunctionCoefficient) * newVariable;
            return newVariable;
        }
        
        storm::expressions::Variable Z3LpSolver::addUpperBoundedIntegerVariable(std::string const& name, double upperBound, double objectiveFunctionCoefficient) {
            storm::expressions::Variable newVariable = this->manager->declareVariable(name, manager->getIntegerType());
            solver->add(expressionAdapter->translateExpression(newVariable.getExpression() <= this->manager->rational(upperBound)));
            optimizationFunction = optimizationFunction + this->manager->rational(objectiveFunctionCoefficient) * newVariable;
            return newVariable;
        }
        
        storm::expressions::Variable Z3LpSolver::addUnboundedIntegerVariable(std::string const& name, double objectiveFunctionCoefficient) {
            storm::expressions::Variable newVariable = this->manager->declareVariable(name, manager->getIntegerType());
            optimizationFunction = optimizationFunction + this->manager->rational(objectiveFunctionCoefficient) * newVariable;
            return newVariable;
        }
        
        storm::expressions::Variable Z3LpSolver::addBinaryVariable(std::string const& name, double objectiveFunctionCoefficient) {
            storm::expressions::Variable newVariable = this->manager->declareVariable(name, manager->getIntegerType());
            solver->add(expressionAdapter->translateExpression((newVariable.getExpression() >= this->manager->rational(storm::utility::one<storm::RationalNumber>())) && (newVariable.getExpression() <= this->manager->rational(storm::utility::one<storm::RationalNumber>()))));
            optimizationFunction = optimizationFunction + this->manager->rational(objectiveFunctionCoefficient) * newVariable;
            return newVariable;
        }

        void Z3LpSolver::addConstraint(std::string const& name, storm::expressions::Expression const& constraint) {            
            STORM_LOG_THROW(constraint.isRelationalExpression(), storm::exceptions::InvalidArgumentException, "Illegal constraint is not a relational expression.");
            STORM_LOG_THROW(constraint.getOperator() != storm::expressions::OperatorType::NotEqual, storm::exceptions::InvalidArgumentException, "Illegal constraint uses inequality operator.");
            STORM_LOG_WARN_COND(name == "", "Z3 does not support names for constraints");
            solver->add(expressionAdapter->translateExpression(constraint));
        }
        
        void Z3LpSolver::optimize() const {
            // First incorporate all recent changes.
            this->update();

            // Invoke push() as we want to be able to erase the current optimization function after checking
            solver->push();

            // Solve the optimization problem depending on the optimization direction
            z3::optimize::handle optFuncHandle = this->getOptimizationDirection() == OptimizationDirection::Minimize ? solver->minimize(expressionAdapter->translateExpression(optimizationFunction)) :  solver->maximize(expressionAdapter->translateExpression(optimizationFunction));
            z3::check_result chkRes = solver->check();
            STORM_LOG_THROW(chkRes != z3::unknown, storm::exceptions::InvalidStateException, "Unable to solve LP problem with Z3: Check result is unknown.");

            // We need to store the resulting information at this point. Otherwise, the information would be lost after calling pop() ...

            // Check feasibility
            lastCheckInfeasible = (chkRes == z3::unsat);
            if(lastCheckInfeasible) {
                lastCheckUnbounded = false;
            } else {
                // Get objective result
                lastCheckObjectiveValue = std::make_unique<z3::expr>(solver->upper(optFuncHandle));
                // Check boundedness
                STORM_LOG_ASSERT(lastCheckObjectiveValue->is_app(), "Failed to convert Z3 expression. Encountered unknown expression type.");
                lastCheckUnbounded = (lastCheckObjectiveValue->decl().decl_kind() != Z3_OP_ANUM);
                if(lastCheckUnbounded) {
                    lastCheckObjectiveValue.reset(nullptr);
                } else {
                    // Assert that the upper approximation equals the lower one
                    STORM_LOG_ASSERT(std::string(Z3_get_numeral_string(*context, *lastCheckObjectiveValue)) == std::string(Z3_get_numeral_string(*context, solver->lower(optFuncHandle))), "Lower and Upper Approximation of z3LPSolver result do not match.");
                    lastCheckModel = std::make_unique<z3::model>(solver->get_model());
                }
            }

            solver->pop(); // removes current optimization function
            this->currentModelHasBeenOptimized = true;
        }
        
        bool Z3LpSolver::isInfeasible() const {
            STORM_LOG_THROW(this->currentModelHasBeenOptimized, storm::exceptions::InvalidStateException, "Illegal call to Z3LpSolver::isInfeasible: model has not been optimized.");
            return lastCheckInfeasible;
        }
        
        bool Z3LpSolver::isUnbounded() const {
            STORM_LOG_THROW(this->currentModelHasBeenOptimized, storm::exceptions::InvalidStateException, "Illegal call to Z3LpSolver::isUnbounded: model has not been optimized.");
            return lastCheckUnbounded;
        }
        
        bool Z3LpSolver::isOptimal() const {
            STORM_LOG_THROW(this->currentModelHasBeenOptimized, storm::exceptions::InvalidStateException, "Illegal call to Z3LpSolver::isOptimal: model has not been optimized.");
            return !lastCheckInfeasible && !lastCheckUnbounded;
        }

        storm::expressions::Expression Z3LpSolver::getValue(storm::expressions::Variable const& variable) const {
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
        
        double Z3LpSolver::getContinuousValue(storm::expressions::Variable const& variable) const {
            storm::expressions::Expression value = getValue(variable);
            if(value.getBaseExpression().isIntegerLiteralExpression()) {
                return value.getBaseExpression().asIntegerLiteralExpression().getValue();
            }
            STORM_LOG_THROW(value.getBaseExpression().isRationalLiteralExpression(), storm::exceptions::ExpressionEvaluationException, "Expected a rational literal while obtaining the value of a continuous variable. Got " << value << "instead.");
            return value.getBaseExpression().asRationalLiteralExpression().getValueAsDouble();
        }
        
        int_fast64_t Z3LpSolver::getIntegerValue(storm::expressions::Variable const& variable) const {
            storm::expressions::Expression value = getValue(variable);
            STORM_LOG_THROW(value.getBaseExpression().isIntegerLiteralExpression(), storm::exceptions::ExpressionEvaluationException, "Expected an integer literal while obtaining the value of an integer variable. Got " << value << "instead.");
            return value.getBaseExpression().asIntegerLiteralExpression().getValue();
        }
        
        bool Z3LpSolver::getBinaryValue(storm::expressions::Variable const& variable) const {
            storm::expressions::Expression value = getValue(variable);
            // Binary variables are in fact represented as integer variables!
            STORM_LOG_THROW(value.getBaseExpression().isIntegerLiteralExpression(), storm::exceptions::ExpressionEvaluationException, "Expected an integer literal while obtaining the value of a binary variable. Got " << value << "instead.");
            int_fast64_t val = value.getBaseExpression().asIntegerLiteralExpression().getValue();
            STORM_LOG_THROW((val==0 || val==1), storm::exceptions::ExpressionEvaluationException, "Tried to get a binary value for a variable that is neither 0 nor 1.");
            return val==1;
        }
        
        double Z3LpSolver::getObjectiveValue() const {
            if (!this->isOptimal()) {
                STORM_LOG_THROW(!this->isInfeasible(), storm::exceptions::InvalidAccessException, "Unable to get Z3 solution from infeasible model.");
                STORM_LOG_THROW(!this->isUnbounded(), storm::exceptions::InvalidAccessException, "Unable to get Z3 solution from unbounded model.");
                STORM_LOG_THROW(false, storm::exceptions::InvalidAccessException, "Unable to get Z3 solution from unoptimized model.");
            }
            STORM_LOG_ASSERT(lastCheckObjectiveValue, "Objective value has not been stored.");

            storm::expressions::Expression result = this->expressionAdapter->translateExpression(*lastCheckObjectiveValue);
            if(result.getBaseExpression().isIntegerLiteralExpression()) {
                return result.getBaseExpression().asIntegerLiteralExpression().getValue();
            }
            STORM_LOG_THROW(result.getBaseExpression().isRationalLiteralExpression(), storm::exceptions::ExpressionEvaluationException, "Expected a rational literal while obtaining the objective result. Got " << result << "instead.");
            return result.getBaseExpression().asRationalLiteralExpression().getValueAsDouble();
        }
        
        void Z3LpSolver::writeModelToFile(std::string const& filename) const {
                STORM_LOG_THROW(!this->isUnbounded(), storm::exceptions::NotImplementedException, "Exporting LP Problems to a file is not implemented for z3.");
        }

        storm::expressions::Variable Z3LpSolver::addBoundedContinuousVariable(std::string const& name, storm::RationalNumber const& lowerBound, storm::RationalNumber const& upperBound, storm::RationalNumber const& objectiveFunctionCoefficient) {
            storm::expressions::Variable newVariable = this->manager->declareVariable(name, manager->getRationalType());
            solver->add(expressionAdapter->translateExpression((newVariable.getExpression() >= this->manager->rational(lowerBound)) && (newVariable.getExpression() <= this->manager->rational(upperBound))));
            optimizationFunction = optimizationFunction + this->manager->rational(objectiveFunctionCoefficient) * newVariable;
            return newVariable;
        }

        storm::expressions::Variable Z3LpSolver::addLowerBoundedContinuousVariable(std::string const& name, storm::RationalNumber const& lowerBound, storm::RationalNumber const& objectiveFunctionCoefficient) {
            storm::expressions::Variable newVariable = this->manager->declareVariable(name, manager->getRationalType());
            solver->add(expressionAdapter->translateExpression(newVariable.getExpression() >= this->manager->rational(lowerBound)));
            optimizationFunction = optimizationFunction + this->manager->rational(objectiveFunctionCoefficient) * newVariable;
            return newVariable;
        }

        storm::expressions::Variable Z3LpSolver::addUpperBoundedContinuousVariable(std::string const& name, storm::RationalNumber const& upperBound, storm::RationalNumber const& objectiveFunctionCoefficient) {
            storm::expressions::Variable newVariable = this->manager->declareVariable(name, manager->getRationalType());
            solver->add(expressionAdapter->translateExpression(newVariable.getExpression() <= this->manager->rational(upperBound)));
            optimizationFunction = optimizationFunction + this->manager->rational(objectiveFunctionCoefficient) * newVariable;
            return newVariable;
        }

        storm::expressions::Variable Z3LpSolver::addUnboundedContinuousVariable(std::string const& name, storm::RationalNumber const& objectiveFunctionCoefficient) {
            storm::expressions::Variable newVariable = this->manager->declareVariable(name, manager->getRationalType());
            optimizationFunction = optimizationFunction + this->manager->rational(objectiveFunctionCoefficient) * newVariable;
            return newVariable;
        }

        storm::expressions::Variable Z3LpSolver::addBoundedIntegerVariable(std::string const& name, storm::RationalNumber const& lowerBound, storm::RationalNumber const& upperBound, storm::RationalNumber const& objectiveFunctionCoefficient) {
            storm::expressions::Variable newVariable = this->manager->declareVariable(name, manager->getIntegerType());
            solver->add(expressionAdapter->translateExpression((newVariable.getExpression() >= this->manager->rational(lowerBound)) && (newVariable.getExpression() <= this->manager->rational(upperBound))));
            optimizationFunction = optimizationFunction + this->manager->rational(objectiveFunctionCoefficient) * newVariable;
            return newVariable;
        }

        storm::expressions::Variable Z3LpSolver::addLowerBoundedIntegerVariable(std::string const& name, storm::RationalNumber const& lowerBound, storm::RationalNumber const& objectiveFunctionCoefficient) {
            storm::expressions::Variable newVariable = this->manager->declareVariable(name, manager->getIntegerType());
            solver->add(expressionAdapter->translateExpression(newVariable.getExpression() >= this->manager->rational(lowerBound)));
            optimizationFunction = optimizationFunction + this->manager->rational(objectiveFunctionCoefficient) * newVariable;
            return newVariable;
        }

        storm::expressions::Variable Z3LpSolver::addUpperBoundedIntegerVariable(std::string const& name, storm::RationalNumber const& upperBound, storm::RationalNumber const& objectiveFunctionCoefficient) {
            storm::expressions::Variable newVariable = this->manager->declareVariable(name, manager->getIntegerType());
            solver->add(expressionAdapter->translateExpression(newVariable.getExpression() <= this->manager->rational(upperBound)));
            optimizationFunction = optimizationFunction + this->manager->rational(objectiveFunctionCoefficient) * newVariable;
            return newVariable;
        }

        storm::expressions::Variable Z3LpSolver::addUnboundedIntegerVariable(std::string const& name, storm::RationalNumber const& objectiveFunctionCoefficient) {
            storm::expressions::Variable newVariable = this->manager->declareVariable(name, manager->getIntegerType());
            optimizationFunction = optimizationFunction + this->manager->rational(objectiveFunctionCoefficient) * newVariable;
            return newVariable;
        }

        storm::expressions::Variable Z3LpSolver::addBinaryVariable(std::string const& name, storm::RationalNumber const& objectiveFunctionCoefficient) {
            storm::expressions::Variable newVariable = this->manager->declareVariable(name, manager->getIntegerType());
            solver->add(expressionAdapter->translateExpression((newVariable.getExpression() >= this->manager->rational(storm::utility::one<storm::RationalNumber>())) && (newVariable.getExpression() <= this->manager->rational(storm::utility::one<storm::RationalNumber>()))));
            optimizationFunction = optimizationFunction + this->manager->rational(objectiveFunctionCoefficient) * newVariable;
            return newVariable;
        }

        storm::RationalNumber Z3LpSolver::getExactContinuousValue(storm::expressions::Variable const& variable) const {
            storm::expressions::Expression value = getValue(variable);
            if(value.getBaseExpression().isIntegerLiteralExpression()) {
                return storm::utility::convertNumber<storm::RationalNumber>(value.getBaseExpression().asIntegerLiteralExpression().getValue());
            }
            STORM_LOG_ASSERT(value.getBaseExpression().isRationalLiteralExpression(), "Expected a rational literal while obtaining the value of a continuous variable. Got " << value << "instead.");
            return value.getBaseExpression().asRationalLiteralExpression().getValue();
        }

        storm::RationalNumber Z3LpSolver::getExactObjectiveValue() const {
            if (!this->isOptimal()) {
                STORM_LOG_THROW(!this->isInfeasible(), storm::exceptions::InvalidAccessException, "Unable to get Z3 solution from infeasible model.");
                STORM_LOG_THROW(!this->isUnbounded(), storm::exceptions::InvalidAccessException, "Unable to get Z3 solution from unbounded model.");
                STORM_LOG_THROW(false, storm::exceptions::InvalidAccessException, "Unable to get Z3 solution from unoptimized model.");
            }
            STORM_LOG_ASSERT(lastCheckObjectiveValue, "Objective value has not been stored.");

            storm::expressions::Expression result = this->expressionAdapter->translateExpression(*lastCheckObjectiveValue);
            if(result.getBaseExpression().isIntegerLiteralExpression()) {
                return storm::utility::convertNumber<storm::RationalNumber>(result.getBaseExpression().asIntegerLiteralExpression().getValue());
            }
            STORM_LOG_THROW(result.getBaseExpression().isRationalLiteralExpression(), storm::exceptions::ExpressionEvaluationException, "Expected a rational literal while obtaining the objective result. Got " << result << "instead.");
            return result.getBaseExpression().asRationalLiteralExpression().getValue();
        }

#else 
            Z3LpSolver::Z3LpSolver(std::string const&, OptimizationDirection const&) {
                throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without Z3 or the version of Z3 does not support optimization. Yet, a method was called that requires this support.";
            }

            Z3LpSolver::Z3LpSolver(std::string const&) {
                throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without Z3 or the version of Z3 does not support optimization. Yet, a method was called that requires this support.";
            }

            Z3LpSolver::Z3LpSolver(OptimizationDirection const&) {
                throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without Z3 or the version of Z3 does not support optimization. Yet, a method was called that requires this support.";
            }

            Z3LpSolver::Z3LpSolver() {
                throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without Z3 or the version of Z3 does not support optimization. Yet, a method was called that requires this support.";
            }

            Z3LpSolver::~Z3LpSolver() {

            }

            storm::expressions::Variable Z3LpSolver::addBoundedContinuousVariable(std::string const&, double, double, double) {
                throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without Z3 or the version of Z3 does not support optimization. Yet, a method was called that requires this support.";
            }

            storm::expressions::Variable Z3LpSolver::addLowerBoundedContinuousVariable(std::string const&, double, double ) {
                throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without Z3 or the version of Z3 does not support optimization. Yet, a method was called that requires this support.";            }

            storm::expressions::Variable Z3LpSolver::addUpperBoundedContinuousVariable(std::string const&, double, double ) {
                throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without Z3 or the version of Z3 does not support optimization. Yet, a method was called that requires this support.";
            }

            storm::expressions::Variable Z3LpSolver::addUnboundedContinuousVariable(std::string const&, double ) {
                throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without Z3 or the version of Z3 does not support optimization. Yet, a method was called that requires this support.";
            }

            storm::expressions::Variable Z3LpSolver::addBoundedIntegerVariable(std::string const&, double, double, double) {
                throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without Z3 or the version of Z3 does not support optimization. Yet, a method was called that requires this support.";
            }

            storm::expressions::Variable Z3LpSolver::addLowerBoundedIntegerVariable(std::string const&, double, double) {
                throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without Z3 or the version of Z3 does not support optimization. Yet, a method was called that requires this support.";
            }

            storm::expressions::Variable Z3LpSolver::addUpperBoundedIntegerVariable(std::string const&, double, double) {
                throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without Z3 or the version of Z3 does not support optimization. Yet, a method was called that requires this support.";
            }

            storm::expressions::Variable Z3LpSolver::addUnboundedIntegerVariable(std::string const&, double) {
                throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without Z3 or the version of Z3 does not support optimization. Yet, a method was called that requires this support.";
            }

            storm::expressions::Variable Z3LpSolver::addBinaryVariable(std::string const&, double) {
                throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without Z3 or the version of Z3 does not support optimization. Yet, a method was called that requires this support.";
            }

            void Z3LpSolver::update() const {
                throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without Z3 or the version of Z3 does not support optimization. Yet, a method was called that requires this support.";
            }

            void Z3LpSolver::addConstraint(std::string const&, storm::expressions::Expression const&) {
                throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without Z3 or the version of Z3 does not support optimization. Yet, a method was called that requires this support.";
            }

            void Z3LpSolver::optimize() const {
                throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without Z3 or the version of Z3 does not support optimization. Yet, a method was called that requires this support.";
            }

            bool Z3LpSolver::isInfeasible() const {
                throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without Z3 or the version of Z3 does not support optimization. Yet, a method was called that requires this support.";
            }

            bool Z3LpSolver::isUnbounded() const {
                throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without Z3 or the version of Z3 does not support optimization. Yet, a method was called that requires this support.";
            }

            bool Z3LpSolver::isOptimal() const {
                throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without Z3 or the version of Z3 does not support optimization. Yet, a method was called that requires this support.";
            }
            
            storm::expressions::Expression Z3LpSolver::getValue(storm::expressions::Variable const& variable) const {
                throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without Z3 or the version of Z3 does not support optimization. Yet, a method was called that requires this support.";
            }

            double Z3LpSolver::getContinuousValue(storm::expressions::Variable const&) const {
                throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without Z3 or the version of Z3 does not support optimization. Yet, a method was called that requires this support.";
            }

            int_fast64_t Z3LpSolver::getIntegerValue(storm::expressions::Variable const&) const {
                throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without Z3 or the version of Z3 does not support optimization. Yet, a method was called that requires this support.";
            }

            bool Z3LpSolver::getBinaryValue(storm::expressions::Variable const&) const {
                throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without Z3 or the version of Z3 does not support optimization. Yet, a method was called that requires this support.";
            }

            double Z3LpSolver::getObjectiveValue() const {
                throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without Z3 or the version of Z3 does not support optimization. Yet, a method was called that requires this support.";
            }

            void Z3LpSolver::writeModelToFile(std::string const&) const {
                throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without Z3 or the version of Z3 does not support optimization. Yet, a method was called that requires this support.";
            }

            storm::expressions::Variable Z3LpSolver::addBoundedContinuousVariable(std::string const& name, storm::RationalNumber const& lowerBound, storm::RationalNumber const& upperBound, storm::RationalNumber const& objectiveFunctionCoefficient) {
                throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without Z3 or the version of Z3 does not support optimization. Yet, a method was called that requires this support.";
            }

            storm::expressions::Variable Z3LpSolver::addLowerBoundedContinuousVariable(std::string const& name, storm::RationalNumber const& lowerBound, storm::RationalNumber const& objectiveFunctionCoefficient) {
                throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without Z3 or the version of Z3 does not support optimization. Yet, a method was called that requires this support.";

            }

            storm::expressions::Variable Z3LpSolver::addUpperBoundedContinuousVariable(std::string const& name, storm::RationalNumber const& upperBound, storm::RationalNumber const& objectiveFunctionCoefficient) {
                throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without Z3 or the version of Z3 does not support optimization. Yet, a method was called that requires this support.";
            }

            storm::expressions::Variable Z3LpSolver::addUnboundedContinuousVariable(std::string const& name, storm::RationalNumber const& objectiveFunctionCoefficient) {
                throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without Z3 or the version of Z3 does not support optimization. Yet, a method was called that requires this support.";
            }

            storm::expressions::Variable Z3LpSolver::addBoundedIntegerVariable(std::string const& name, storm::RationalNumber const& lowerBound, storm::RationalNumber const& upperBound, storm::RationalNumber const& objectiveFunctionCoefficient) {
                throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without Z3 or the version of Z3 does not support optimization. Yet, a method was called that requires this support.";
            }

            storm::expressions::Variable Z3LpSolver::addLowerBoundedIntegerVariable(std::string const& name, storm::RationalNumber const& lowerBound, storm::RationalNumber const& objectiveFunctionCoefficient) {
                throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without Z3 or the version of Z3 does not support optimization. Yet, a method was called that requires this support.";
            }

            storm::expressions::Variable Z3LpSolver::addUpperBoundedIntegerVariable(std::string const& name, storm::RationalNumber const& upperBound, storm::RationalNumber const& objectiveFunctionCoefficient) {
                throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without Z3 or the version of Z3 does not support optimization. Yet, a method was called that requires this support.";
            }

            storm::expressions::Variable Z3LpSolver::addUnboundedIntegerVariable(std::string const& name, storm::RationalNumber const& objectiveFunctionCoefficient) {
                throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without Z3 or the version of Z3 does not support optimization. Yet, a method was called that requires this support.";
            }

            storm::expressions::Variable Z3LpSolver::addBinaryVariable(std::string const& name, storm::RationalNumber const& objectiveFunctionCoefficient) {
                throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without Z3 or the version of Z3 does not support optimization. Yet, a method was called that requires this support.";
            }

            storm::RationalNumber Z3LpSolver::getExactContinuousValue(storm::expressions::Variable const& variable) const {
                throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without Z3 or the version of Z3 does not support optimization. Yet, a method was called that requires this support.";
            }

            storm::RationalNumber Z3LpSolver::getExactObjectiveValue() const {
                throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without Z3 or the version of Z3 does not support optimization. Yet, a method was called that requires this support.";
            }


#endif        
    }
}
