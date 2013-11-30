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
        class GurobiLpSolver : public LpSolver {
        public:
            GurobiLpSolver(std::string const& name, ModelSense const& modelSense);
            GurobiLpSolver(std::string const& name);
            virtual ~GurobiLpSolver();
            
            virtual uint_fast64_t createContinuousVariable(std::string const& name, VariableType const& variableType, double lowerBound, double upperBound, double objectiveFunctionCoefficient) override;
            virtual uint_fast64_t createIntegerVariable(std::string const& name, VariableType const& variableType, double lowerBound, double upperBound, double objectiveFunctionCoefficient) override;
            virtual uint_fast64_t createBinaryVariable(std::string const& name, double objectiveFunctionCoefficient) override;
            
            virtual void addConstraint(std::string const& name, std::vector<uint_fast64_t> const& variables, std::vector<double> const& coefficients, BoundType const& boundType, double rightHandSideValue) override;
            
            virtual void setModelSense(ModelSense const& newModelSense) override;
            virtual void optimize() const override;
            
            virtual int_fast64_t getIntegerValue(uint_fast64_t variableIndex) const override;
            virtual bool getBinaryValue(uint_fast64_t variableIndex) const override;
            virtual double getContinuousValue(uint_fast64_t variableIndex) const override;
            
            virtual void writeModelToFile(std::string const& filename) const override;
            
        private:
            void setGurobiEnvironmentProperties() const;
            void updateModel() const;
            
            // The Gurobi environment.
            GRBenv* env;
            
            // The Gurobi model.
            GRBmodel* model;
            
            // A counter that keeps track of the next free variable index.
            uint_fast64_t nextVariableIndex;
            
            // A flag that stores whether the model was optimized properly.
            mutable bool isOptimal;
        };
#else
        // If Gurobi is not available, we provide a stub implementation that emits an error if any of its methods is called.
        class GurobiLpSolver : public LpSolver {
        public:

        	GurobiLpSolver(std::string const& name) : LpSolver(MINIMIZE) {
        		throw storm::exceptions::NotImplementedException() << "This version of StoRM was compiled without support for Gurobi. Yet, a method was called that requires this support. Please choose a version of support with Gurobi support.";
        	}

        	virtual ~GurobiLpSolver() {
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
            
            virtual void setModelSense(ModelSense const& newModelSense) {
                throw storm::exceptions::NotImplementedException() << "This version of StoRM was compiled without support for Gurobi. Yet, a method was called that requires this support. Please choose a version of support with Gurobi support.";
            }
            
            virtual void optimize() const override {
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
            
            virtual void writeModelToFile(std::string const& filename) const override {
                throw storm::exceptions::NotImplementedException() << "This version of StoRM was compiled without support for Gurobi. Yet, a method was called that requires this support. Please choose a version of support with Gurobi support.";
            }
        };
#endif
    
    }
}

#endif /* STORM_SOLVER_GUROBILPSOLVER */
