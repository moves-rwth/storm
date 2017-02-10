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
        
#ifdef STORM_HAVE_Z3
        Z3LpSolver::Z3LpSolver(std::string const& name, OptimizationDirection const& optDir) : LpSolver(optDir), z3CheckResult(z3::unknown), z3Handle(0) {
            STORM_LOG_WARN_COND(name != "", "Z3 does not support names for solvers");
            z3::config config;
            config.set("model", true);
            context = std::unique_ptr<z3::context>(new z3::context(config));
            solver = std::unique_ptr<z3::optimize>(new z3::optimize(*context));
            expressionAdapter = std::unique_ptr<storm::adapters::Z3ExpressionAdapter>(new storm::adapters::Z3ExpressionAdapter(*this->manager, *context));
            optimizationFunction = this->getManager().rational(storm::utility::zero<double>());
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
            // Since the model changed, we erase the optimality flag and reset the current model.
            this->z3Model.reset(nullptr);
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
            storm::expressions::Variable newVariable = this->manager->declareVariable(name, manager->getBooleanType());
            optimizationFunction = optimizationFunction + this->manager->rational(objectiveFunctionCoefficient) * newVariable;
            return newVariable;
        }

        void Z3LpSolver::addConstraint(std::string const& name, storm::expressions::Expression const& constraint) {            
            STORM_LOG_THROW(constraint.isRelationalExpression(), storm::exceptions::InvalidArgumentException, "Illegal constraint is not a relational expression.");
            STORM_LOG_THROW(constraint.getOperator() != storm::expressions::OperatorType::NotEqual, storm::exceptions::InvalidArgumentException, "Illegal constraint uses inequality operator.");
            STORM_LOG_WARN_COND(name != "", "Z3 does not support names for constraints");
            solver->add(expressionAdapter->translateExpression(constraint));
        }
        
        void Z3LpSolver::optimize() const {
            // First incorporate all recent changes.
            this->update();

            // Invoke push() as we want to be able to erase the current optimization function after checking
            solver->push();

            // Solve the optimization problem depending on the optimization direction
            z3Handle = this->getOptimizationDirection() == OptimizationDirection::Minimize ? solver->minimize(expressionAdapter->translateExpression(optimizationFunction)) :  solver->maximize(expressionAdapter->translateExpression(optimizationFunction));
            z3CheckResult = solver->check();
            STORM_LOG_THROW(z3CheckResult != z3::unknown, storm::exceptions::InvalidStateException, "Unable to solve LP problem with Z3: Check result is unknown.");

            this->currentModelHasBeenOptimized = true;
            solver->pop(); // removes current optimization function

        }
        
        bool Z3LpSolver::isInfeasible() const {
            STORM_LOG_THROW(this->currentModelHasBeenOptimized, storm::exceptions::InvalidStateException, "Illegal call to Z3LpSolver::isInfeasible: model has not been optimized.");
            return z3CheckResult == z3::unsat;
        }
        
        bool Z3LpSolver::isUnbounded() const {
            STORM_LOG_THROW(this->currentModelHasBeenOptimized, storm::exceptions::InvalidStateException, "Illegal call to Z3LpSolver::isInfeasible: model has not been optimized.");
            z3::expr expr = solver->upper(z3Handle);
            STORM_LOG_THROW(expr.is_app(), storm::exceptions::ExpressionEvaluationException, "Failed to convert Z3 expression. Encountered unknown expression type.");
            return expr.decl().decl_kind() != Z3_OP_ANUM;
        }
        
        bool Z3LpSolver::isOptimal() const {
            return !this->isInfeasible() && !this->isUnbounded();
        }


        storm::expressions::Expression Z3LpSolver::getValue(storm::expressions::Variable const& variable) const {
            STORM_LOG_ASSERT(variable.getManager() == this->getManager(), "Requested variable is managed by a different manager.");
            if (!this->isOptimal()) {
                STORM_LOG_THROW(!this->isInfeasible(), storm::exceptions::InvalidAccessException, "Unable to get Z3 solution from infeasible model.");
                STORM_LOG_THROW(!this->isUnbounded(), storm::exceptions::InvalidAccessException, "Unable to get Z3 solution from unbounded model.");
                STORM_LOG_THROW(false, storm::exceptions::InvalidAccessException, "Unable to get Z3 solution from unoptimized model.");
            }
            if(!this->z3Model) {
                z3Model = std::unique_ptr<z3::model>(new z3::model(solver->get_model()));
            }
            z3::expr z3Var = this->expressionAdapter->translateExpression(variable);
			return this->expressionAdapter->translateExpression(z3Model->eval(z3Var, true));
        }

        double Z3LpSolver::getContinuousValue(storm::expressions::Variable const& variable) const {
            return getValue(variable).evaluateAsDouble();
        }
        
        int_fast64_t Z3LpSolver::getIntegerValue(storm::expressions::Variable const& variable) const {
            return getValue(variable).evaluateAsInt();
        }
        
        bool Z3LpSolver::getBinaryValue(storm::expressions::Variable const& variable) const {
           return getValue(variable).evaluateAsBool();
        }
        
        double Z3LpSolver::getObjectiveValue() const {
            if (!this->isOptimal()) {
                STORM_LOG_THROW(!this->isInfeasible(), storm::exceptions::InvalidAccessException, "Unable to get Z3 solution from infeasible model.");
                STORM_LOG_THROW(!this->isUnbounded(), storm::exceptions::InvalidAccessException, "Unable to get Z3 solution from unbounded model.");
                STORM_LOG_THROW(false, storm::exceptions::InvalidAccessException, "Unable to get Z3 solution from unoptimized model.");
            }
            z3::expr expr = solver->upper(z3Handle);
            STORM_LOG_THROW(expr == solver->lower(z3Handle), storm::exceptions::InvalidAccessException, "Lower and Upper Approximation of z3LPSolver result do not match."); // TODO: make this an assertion
            return this->expressionAdapter->translateExpression(expr).evaluateAsDouble();
        }
        
        void Z3LpSolver::writeModelToFile(std::string const& filename) const {
                STORM_LOG_THROW(!this->isUnbounded(), storm::exceptions::NotImplementedException, "Exporting LP Problems to a file is not implemented for z3.");
        }

#else 
            Z3LpSolver::Z3LpSolver(std::string const&, OptimizationDirection const&) {
                throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for Z3. Yet, a method was called that requires this support. Please choose a version of support with Z3 support.";
            }
            
            Z3LpSolver::Z3LpSolver(std::string const&) {
                throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for Z3. Yet, a method was called that requires this support. Please choose a version of support with Z3 support.";
            }
            
            Z3LpSolver::Z3LpSolver(OptimizationDirection const&) {
                throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for Z3. Yet, a method was called that requires this support. Please choose a version of support with Z3 support.";
            }
            
            Z3LpSolver::Z3LpSolver() {
                throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for Z3. Yet, a method was called that requires this support. Please choose a version of support with Z3 support.";
            }
            
            Z3LpSolver::~Z3LpSolver() { 
            
            }
            
            storm::expressions::Variable Z3LpSolver::addBoundedContinuousVariable(std::string const&, double, double, double) {
                throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for Z3. Yet, a method was called that requires this support. Please choose a version of support with Z3 support.";
            }
            
            storm::expressions::Variable Z3LpSolver::addLowerBoundedContinuousVariable(std::string const&, double, double ) {
                throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for Z3. Yet, a method was called that requires this support. Please choose a version of support with Z3 support.";            }
            
            storm::expressions::Variable Z3LpSolver::addUpperBoundedContinuousVariable(std::string const&, double, double ) {
                throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for Z3. Yet, a method was called that requires this support. Please choose a version of support with Z3 support.";
            }
            
            storm::expressions::Variable Z3LpSolver::addUnboundedContinuousVariable(std::string const&, double ) {
                throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for Z3. Yet, a method was called that requires this support. Please choose a version of support with Z3 support.";
            }
            
            storm::expressions::Variable Z3LpSolver::addBoundedIntegerVariable(std::string const&, double, double, double) {
                throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for Z3. Yet, a method was called that requires this support. Please choose a version of support with Z3 support.";
            }
            
            storm::expressions::Variable Z3LpSolver::addLowerBoundedIntegerVariable(std::string const&, double, double) {
                throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for Z3. Yet, a method was called that requires this support. Please choose a version of support with Z3 support.";
            }
            
            storm::expressions::Variable Z3LpSolver::addUpperBoundedIntegerVariable(std::string const&, double, double) {
                throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for Z3. Yet, a method was called that requires this support. Please choose a version of support with Z3 support.";
            }
            
            storm::expressions::Variable Z3LpSolver::addUnboundedIntegerVariable(std::string const&, double) {
                throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for Z3. Yet, a method was called that requires this support. Please choose a version of support with Z3 support.";
            }
            
            storm::expressions::Variable Z3LpSolver::addBinaryVariable(std::string const&, double) {
                throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for Z3. Yet, a method was called that requires this support. Please choose a version of support with Z3 support.";
            }
            
            void Z3LpSolver::update() const {
                throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for Z3. Yet, a method was called that requires this support. Please choose a version of support with Z3 support.";
            }
            
            void Z3LpSolver::addConstraint(std::string const&, storm::expressions::Expression const&) {
                throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for Z3. Yet, a method was called that requires this support. Please choose a version of support with Z3 support.";
            }
            
            void Z3LpSolver::optimize() const {
                throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for Z3. Yet, a method was called that requires this support. Please choose a version of support with Z3 support.";
            }
            
            bool Z3LpSolver::isInfeasible() const {
                throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for Z3. Yet, a method was called that requires this support. Please choose a version of support with Z3 support.";
            }
            
            bool Z3LpSolver::isUnbounded() const {
                throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for Z3. Yet, a method was called that requires this support. Please choose a version of support with Z3 support.";
            }
            
            bool Z3LpSolver::isOptimal() const {
                throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for Z3. Yet, a method was called that requires this support. Please choose a version of support with Z3 support.";
            }
            
            double Z3LpSolver::getContinuousValue(storm::expressions::Variable const&) const {
                throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for Z3. Yet, a method was called that requires this support. Please choose a version of support with Z3 support.";
            }
            
            int_fast64_t Z3LpSolver::getIntegerValue(storm::expressions::Variable const&) const {
                throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for Z3. Yet, a method was called that requires this support. Please choose a version of support with Z3 support.";
            }
            
            bool Z3LpSolver::getBinaryValue(storm::expressions::Variable const&) const {
                throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for Z3. Yet, a method was called that requires this support. Please choose a version of support with Z3 support.";
            }
            
            double Z3LpSolver::getObjectiveValue() const {
                throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for Z3. Yet, a method was called that requires this support. Please choose a version of support with Z3 support.";
            }
            
            void Z3LpSolver::writeModelToFile(std::string const&) const {
                throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for Z3. Yet, a method was called that requires this support. Please choose a version of support with Z3 support.";
            }

            void Z3LpSolver::toggleOutput(bool) const {
                throw storm::exceptions::NotImplementedException() << "This version of storm was compiled without support for Z3. Yet, a method was called that requires this support. Please choose a version of support with Z3 support.";
            }

#endif        
    }
}
