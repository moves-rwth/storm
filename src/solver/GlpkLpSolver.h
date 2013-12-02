#ifndef STORM_SOLVER_GLPKLPSOLVER_H_
#define STORM_SOLVER_GLPKLPSOLVER_H_

#include "src/solver/LpSolver.h"
#include "src/exceptions/NotImplementedException.h"

// To detect whether the usage of glpk is possible, this include is neccessary.
#include "storm-config.h"

#ifdef STORM_HAVE_GLPK
#include <glpk.h>
#endif

namespace storm {
    namespace solver {
#ifdef STORM_HAVE_GLPK
        class GlpkLpSolver : public LpSolver {
        public:
            GlpkLpSolver(std::string const& name, ModelSense const& modelSense);
            GlpkLpSolver(std::string const& name);
            virtual ~GlpkLpSolver();
            
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
            // The glpk LP problem.
            glp_prob* lp;
            
            // A counter that keeps track of the next free variable index.
            uint_fast64_t nextVariableIndex;
            
            // A counter that keeps track of the next free constraint index.
            uint_fast64_t nextConstraintIndex;
            
            // A flag that stores whether the model was optimized properly.
            mutable bool isOptimal;
            
            // The arrays that store the coefficient matrix of the problem.
            std::vector<int> rowIndices;
            std::vector<int> columnIndices;
            std::vector<double> coefficientValues;
        };
#else
        // If glpk is not available, we provide a stub implementation that emits an error if any of its methods is called.
        class GlpkLpSolver : public LpSolver {
        public:
            
            GlpkLpSolver(std::string const& name) : LpSolver(MINIMIZE) {
                throw storm::exceptions::NotImplementedException() << "This version of StoRM was compiled without support for glpk. Yet, a method was called that requires this support. Please choose a version of support with glpk support.";
            }
            
            virtual GlpkLpSolver() {
                throw storm::exceptions::NotImplementedException() << "This version of StoRM was compiled without support for glpk. Yet, a method was called that requires this support. Please choose a version of support with glpk support.";
            }
            
            virtual uint_fast64_t createContinuousVariable(std::string const& name, VariableType const& variableType, double lowerBound, double upperBound, double objectiveFunctionCoefficient) override {
                throw storm::exceptions::NotImplementedException() << "This version of StoRM was compiled without support for glpk. Yet, a method was called that requires this support. Please choose a version of support with glpk support.";
            }
            
            virtual uint_fast64_t createIntegerVariable(std::string const& name, VariableType const& variableType, double lowerBound, double upperBound, double objectiveFunctionCoefficient) override {
                throw storm::exceptions::NotImplementedException() << "This version of StoRM was compiled without support for glpk. Yet, a method was called that requires this support. Please choose a version of support with glpk support.";
            }
            
            virtual uint_fast64_t createBinaryVariable(std::string const& name, double objectiveFunctionCoefficient) override {
                throw storm::exceptions::NotImplementedException() << "This version of StoRM was compiled without support for glpk. Yet, a method was called that requires this support. Please choose a version of support with glpk support.";
            }
            
            virtual void addConstraint(std::string const& name, std::vector<uint_fast64_t> const& variables, std::vector<double> const& coefficients, BoundType const& boundType, double rightHandSideValue) override {
                throw storm::exceptions::NotImplementedException() << "This version of StoRM was compiled without support for glpk. Yet, a method was called that requires this support. Please choose a version of support with glpk support.";
            }
            
            virtual void setModelSense(ModelSense const& newModelSense) {
                throw storm::exceptions::NotImplementedException() << "This version of StoRM was compiled without support for glpk. Yet, a method was called that requires this support. Please choose a version of support with glpk support.";
            }
            
            virtual void optimize() const override {
                throw storm::exceptions::NotImplementedException() << "This version of StoRM was compiled without support for glpk. Yet, a method was called that requires this support. Please choose a version of support with glpk support.";
            }
            
            virtual int_fast64_t getIntegerValue(uint_fast64_t variableIndex) const override {
                throw storm::exceptions::NotImplementedException() << "This version of StoRM was compiled without support for glpk. Yet, a method was called that requires this support. Please choose a version of support with glpk support.";
            }
            
            virtual bool getBinaryValue(uint_fast64_t variableIndex) const override {
                throw storm::exceptions::NotImplementedException() << "This version of StoRM was compiled without support for glpk. Yet, a method was called that requires this support. Please choose a version of support with glpk support.";
            }
            
            virtual double getContinuousValue(uint_fast64_t variableIndex) const override {
                throw storm::exceptions::NotImplementedException() << "This version of StoRM was compiled without support for glpk. Yet, a method was called that requires this support. Please choose a version of support with glpk support.";
            }
            
            virtual void writeModelToFile(std::string const& filename) const override {
                throw storm::exceptions::NotImplementedException() << "This version of StoRM was compiled without support for glpk. Yet, a method was called that requires this support. Please choose a version of support with glpk support.";
            }
        };
#endif
    }
}

#endif /* STORM_SOLVER_GLPKLPSOLVER_H_ */
