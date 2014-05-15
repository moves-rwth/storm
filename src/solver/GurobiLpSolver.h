#ifndef STORM_SOLVER_GUROBILPSOLVER
#define STORM_SOLVER_GUROBILPSOLVER

#include "src/solver/LpSolver.h"
#include "src/exceptions/NotImplementedException.h"

// To detect whether the usage of Gurobi is possible, this include is neccessary.
#include "storm-config.h"

#ifdef STORM_HAVE_GUROBI
extern "C" {
#include "gurobi_c.h"
    
    int __stdcall GRBislp(GRBenv **, const char *, const char *, const char *, const char *);
}
#endif

namespace storm {
    namespace solver {

#ifdef STORM_HAVE_GUROBI
        /*!
         * A class that implements the LpSolver interface using Gurobi.
         */
        class GurobiLpSolver : public LpSolver {
        public:
            /*!
             * Constructs a solver with the given name and model sense.
             *
             * @param name The name of the LP problem.
             * @param modelSense A value indicating whether the value of the objective function is to be minimized or
             * maximized.
             */
            GurobiLpSolver(std::string const& name, ModelSense const& modelSense);
            
            /*!
             * Constructs a solver with the given name. By default the objective function is assumed to be minimized,
             * but this may be altered later using a call to setModelSense.
             *
             * @param name The name of the LP problem.
             */
            GurobiLpSolver(std::string const& name);
            
            /*!
             * Constructs a solver without a name and the given model sense.
             *
             * @param modelSense A value indicating whether the value of the objective function is to be minimized or
             * maximized.
             */
            GurobiLpSolver(ModelSense const& modelSense);
            
            /*!
             * Constructs a solver without a name. By default the objective function is assumed to be minimized,
             * but this may be altered later using a call to setModelSense.
             */
            GurobiLpSolver();
            
            /*!
             * Destructs a solver by freeing the pointers to Gurobi's structures.
             */
            virtual ~GurobiLpSolver();
            
            // Methods to add continuous variables.
            virtual void addBoundedContinuousVariable(std::string const& name, double lowerBound, double upperBound, double objectiveFunctionCoefficient = 0) override;
            virtual void addLowerBoundedContinuousVariable(std::string const& name, double lowerBound, double objectiveFunctionCoefficient = 0) override;
            virtual void addUpperBoundedContinuousVariable(std::string const& name, double upperBound, double objectiveFunctionCoefficient = 0) override;
            virtual void addUnboundedContinuousVariable(std::string const& name, double objectiveFunctionCoefficient = 0) override;
            
            // Methods to add integer variables.
            virtual void addBoundedIntegerVariable(std::string const& name, double lowerBound, double upperBound, double objectiveFunctionCoefficient = 0) override;
            virtual void addLowerBoundedIntegerVariable(std::string const& name, double lowerBound, double objectiveFunctionCoefficient = 0) override;
            virtual void addUpperBoundedIntegerVariable(std::string const& name, double upperBound, double objectiveFunctionCoefficient = 0) override;
            virtual void addUnboundedIntegerVariable(std::string const& name, double objectiveFunctionCoefficient = 0) override;
            
            // Methods to add binary variables.
            virtual void addBinaryVariable(std::string const& name, double objectiveFunctionCoefficient = 0) override;
            
            // Methods to incorporate recent changes.
            virtual void update() const override;
            
            // Methods to add constraints
            virtual void addConstraint(std::string const& name, storm::expressions::Expression const& constraint) override;
            
            // Methods to optimize and retrieve optimality status.
            virtual void optimize() const override;
            virtual bool isInfeasible() const override;
            virtual bool isUnbounded() const override;
            virtual bool isOptimal() const override;
            
            // Methods to retrieve values of variables and the objective function in the optimal solutions.
            virtual double getContinuousValue(std::string const& name) const override;
            virtual int_fast64_t getIntegerValue(std::string const& name) const override;
            virtual bool getBinaryValue(std::string const& name) const override;
            virtual double getObjectiveValue() const override;
            
            // Methods to print the LP problem to a file.
            virtual void writeModelToFile(std::string const& filename) const override;
            
        private:
            /*!
             * Sets some properties of the Gurobi environment according to parameters given by the options.
             */
            void setGurobiEnvironmentProperties() const;
            
            /*!
             * Adds a variable with the given name, type, lower and upper bound and objective function coefficient.
             *
             * @param name The name of the variable.
             * @param variableType The type of the variable in terms of Gurobi's constants.
             * @param lowerBound The lower bound of the range of the variable.
             * @param upperBound The upper bound of the range of the variable.
             * @param objectiveFunctionCoefficient The coefficient of the variable in the objective function.
             */
            void addVariable(std::string const& name, char variableType, double lowerBound, double upperBound, double objectiveFunctionCoefficient);
            
            // The Gurobi environment.
            GRBenv* env;
            
            // The Gurobi model.
            GRBmodel* model;
            
            // The index of the next variable.
            int nextVariableIndex;
            
            // A mapping from variable names to their indices.
            std::map<std::string, int> variableNameToIndexMap;
        };
#else
        // If Gurobi is not available, we provide a stub implementation that emits an error if any of its methods is called.
        class GurobiLpSolver : public LpSolver {
        public:
            GurobiLpSolver(std::string const& name, ModelSense const& modelSense) {
                throw storm::exceptions::NotImplementedException() << "This version of StoRM was compiled without support for Gurobi. Yet, a method was called that requires this support. Please choose a version of support with Gurobi support.";
            }
            
            GurobiLpSolver(std::string const& name) {
                throw storm::exceptions::NotImplementedException() << "This version of StoRM was compiled without support for Gurobi. Yet, a method was called that requires this support. Please choose a version of support with Gurobi support.";
            }
            
            GurobiLpSolver(ModelSense const& modelSense) {
                throw storm::exceptions::NotImplementedException() << "This version of StoRM was compiled without support for Gurobi. Yet, a method was called that requires this support. Please choose a version of support with Gurobi support.";
            }
            
            GurobiLpSolver() {
                throw storm::exceptions::NotImplementedException() << "This version of StoRM was compiled without support for Gurobi. Yet, a method was called that requires this support. Please choose a version of support with Gurobi support.";
            }
            
            virtual void addBoundedContinuousVariable(std::string const& name, double lowerBound, double upperBound, double objectiveFunctionCoefficient = 0) override {
                throw storm::exceptions::NotImplementedException() << "This version of StoRM was compiled without support for Gurobi. Yet, a method was called that requires this support. Please choose a version of support with Gurobi support.";
            }
            
            virtual void addLowerBoundedContinuousVariable(std::string const& name, double lowerBound, double objectiveFunctionCoefficient = 0) override {
                throw storm::exceptions::NotImplementedException() << "This version of StoRM was compiled without support for Gurobi. Yet, a method was called that requires this support. Please choose a version of support with Gurobi support.";            }
            
            virtual void addUpperBoundedContinuousVariable(std::string const& name, double upperBound, double objectiveFunctionCoefficient = 0) override {
                throw storm::exceptions::NotImplementedException() << "This version of StoRM was compiled without support for Gurobi. Yet, a method was called that requires this support. Please choose a version of support with Gurobi support.";
            }
            
            virtual void addUnboundedContinuousVariable(std::string const& name, double objectiveFunctionCoefficient = 0) override {
                throw storm::exceptions::NotImplementedException() << "This version of StoRM was compiled without support for Gurobi. Yet, a method was called that requires this support. Please choose a version of support with Gurobi support.";
            }
            
            virtual void addBoundedIntegerVariable(std::string const& name, double lowerBound, double upperBound, double objectiveFunctionCoefficient = 0) override {
                throw storm::exceptions::NotImplementedException() << "This version of StoRM was compiled without support for Gurobi. Yet, a method was called that requires this support. Please choose a version of support with Gurobi support.";
            }
            
            virtual void addLowerBoundedIntegerVariable(std::string const& name, double lowerBound, double objectiveFunctionCoefficient = 0) override {
                throw storm::exceptions::NotImplementedException() << "This version of StoRM was compiled without support for Gurobi. Yet, a method was called that requires this support. Please choose a version of support with Gurobi support.";
            }
            
            virtual void addUpperBoundedIntegerVariable(std::string const& name, double upperBound, double objectiveFunctionCoefficient = 0) override {
                throw storm::exceptions::NotImplementedException() << "This version of StoRM was compiled without support for Gurobi. Yet, a method was called that requires this support. Please choose a version of support with Gurobi support.";
            }
            
            virtual void addUnboundedIntegerVariable(std::string const& name, double objectiveFunctionCoefficient = 0) override {
                throw storm::exceptions::NotImplementedException() << "This version of StoRM was compiled without support for Gurobi. Yet, a method was called that requires this support. Please choose a version of support with Gurobi support.";
            }
            
            virtual uint_fast64_t addBinaryVariable(std::string const& name, double objectiveFunctionCoefficient = 0) override {
                throw storm::exceptions::NotImplementedException() << "This version of StoRM was compiled without support for Gurobi. Yet, a method was called that requires this support. Please choose a version of support with Gurobi support.";
            }
            
            virtual void update() const override {
                throw storm::exceptions::NotImplementedException() << "This version of StoRM was compiled without support for Gurobi. Yet, a method was called that requires this support. Please choose a version of support with Gurobi support.";
            }
            
            virtual void addConstraint(std::string const& name, storm::expressions::Expression const& constraint) override {
                throw storm::exceptions::NotImplementedException() << "This version of StoRM was compiled without support for Gurobi. Yet, a method was called that requires this support. Please choose a version of support with Gurobi support.";
            }
            
            virtual void optimize() const override {
                throw storm::exceptions::NotImplementedException() << "This version of StoRM was compiled without support for Gurobi. Yet, a method was called that requires this support. Please choose a version of support with Gurobi support.";
            }
            
            virtual bool isInfeasible() const override {
                throw storm::exceptions::NotImplementedException() << "This version of StoRM was compiled without support for Gurobi. Yet, a method was called that requires this support. Please choose a version of support with Gurobi support.";
            }
            
            virtual bool isUnbounded() const override {
                throw storm::exceptions::NotImplementedException() << "This version of StoRM was compiled without support for Gurobi. Yet, a method was called that requires this support. Please choose a version of support with Gurobi support.";
            }
            
            virtual bool isOptimal() const override {
                throw storm::exceptions::NotImplementedException() << "This version of StoRM was compiled without support for Gurobi. Yet, a method was called that requires this support. Please choose a version of support with Gurobi support.";
            }
            
            virtual double getContinuousValue(std::string const& name) const override {
                throw storm::exceptions::NotImplementedException() << "This version of StoRM was compiled without support for Gurobi. Yet, a method was called that requires this support. Please choose a version of support with Gurobi support.";
            }
            
            virtual int_fast64_t getIntegerValue(std::string const& name) const override {
                throw storm::exceptions::NotImplementedException() << "This version of StoRM was compiled without support for Gurobi. Yet, a method was called that requires this support. Please choose a version of support with Gurobi support.";
            }
            
            virtual bool getBinaryValue(std::string const& name) const override {
                throw storm::exceptions::NotImplementedException() << "This version of StoRM was compiled without support for Gurobi. Yet, a method was called that requires this support. Please choose a version of support with Gurobi support.";
            }
            
            virtual double getObjectiveValue() const override {
                throw storm::exceptions::NotImplementedException() << "This version of StoRM was compiled without support for Gurobi. Yet, a method was called that requires this support. Please choose a version of support with Gurobi support.";
            }
            
            virtual void writeModelToFile(std::string const& filename) const override {
                throw storm::exceptions::NotImplementedException() << "This version of StoRM was compiled without support for Gurobi. Yet, a method was called that requires this support. Please choose a version of support with Gurobi support.";
            }
        };
#endif

    }
}

#endif /* STORM_SOLVER_GUROBILPSOLVER */
