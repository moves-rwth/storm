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
            
            virtual uint_fast64_t createContinuousVariable(std::string const& name, VariableType const& variableType, double lowerBound, double upperBound, double objectiveFunctionCoefficient) override;
            virtual uint_fast64_t createIntegerVariable(std::string const& name, VariableType const& variableType, double lowerBound, double upperBound, double objectiveFunctionCoefficient) override;
            virtual uint_fast64_t createBinaryVariable(std::string const& name, double objectiveFunctionCoefficient) override;
            
            virtual void addConstraint(std::string const& name, std::vector<uint_fast64_t> const& variables, std::vector<double> const& coefficients, BoundType const& boundType, double rightHandSideValue) override;
            
            virtual void optimize() const override;
            virtual bool isInfeasible() const override;
            virtual bool isUnbounded() const override;
            virtual bool isOptimal() const override;

            virtual int_fast64_t getIntegerValue(uint_fast64_t variableIndex) const override;
            virtual bool getBinaryValue(uint_fast64_t variableIndex) const override;
            virtual double getContinuousValue(uint_fast64_t variableIndex) const override;
            virtual double getObjectiveValue() const override;

            virtual void writeModelToFile(std::string const& filename) const override;
            
        private:
            /*!
             * Sets some properties of the Gurobi environment according to parameters given by the options.
             */
            void setGurobiEnvironmentProperties() const;
            
            /*!
             * Calls Gurobi to incorporate the latest changes to the model.
             */
            void updateModel() const;
            
            // The Gurobi environment.
            GRBenv* env;
            
            // The Gurobi model.
            GRBmodel* model;
            
            // A counter that keeps track of the next free variable index.
            uint_fast64_t nextVariableIndex;
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
            
            virtual uint_fast64_t createContinuousVariable(std::string const& name, VariableType const& variableType, double lowerBound, double upperBound, double objectiveFunctionCoefficient) override {
                throw storm::exceptions::NotImplementedException() << "This version of StoRM was compiled without support for Gurobi. Yet, a method was called that requires this support. Please choose a version of support with Gurobi support.";
            }
            
            virtual uint_fast64_t createIntegerVariable(std::string const& name, VariableType const& variableType, double lowerBound, double upperBound, double objectiveFunctionCoefficient) override {
                throw storm::exceptions::NotImplementedException() << "This version of StoRM was compiled without support for Gurobi. Yet, a method was called that requires this support. Please choose a version of support with Gurobi support.";
            }
            
            virtual uint_fast64_t createBinaryVariable(std::string const& name, double objectiveFunctionCoefficient) override {
                throw storm::exceptions::NotImplementedException() << "This version of StoRM was compiled without support for Gurobi. Yet, a method was called that requires this support. Please choose a version of support with Gurobi support.";
            }
            
            virtual void addConstraint(std::string const& name, std::vector<uint_fast64_t> const& variables, std::vector<double> const& coefficients, BoundType const& boundType, double rightHandSideValue) override {
                throw storm::exceptions::NotImplementedException() << "This version of StoRM was compiled without support for Gurobi. Yet, a method was called that requires this support. Please choose a version of support with Gurobi support.";
            }
            
            virtual void setModelSense(ModelSense const& modelSense) {
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
            
            virtual int_fast64_t getIntegerValue(uint_fast64_t variableIndex) const override {
                throw storm::exceptions::NotImplementedException() << "This version of StoRM was compiled without support for Gurobi. Yet, a method was called that requires this support. Please choose a version of support with Gurobi support.";
            }
            
            virtual bool getBinaryValue(uint_fast64_t variableIndex) const override {
                throw storm::exceptions::NotImplementedException() << "This version of StoRM was compiled without support for Gurobi. Yet, a method was called that requires this support. Please choose a version of support with Gurobi support.";
            }
            
            virtual double getContinuousValue(uint_fast64_t variableIndex) const override {
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
