#ifndef STORM_SOLVER_LPSOLVER
#define STORM_SOLVER_LPSOLVER

#include <cstdint>
#include <string>
#include <vector>

namespace storm {
    namespace solver {
        
        class LpSolver {
        public:
            enum VariableType {
                UNBOUNDED,
                LOWER_BOUND,
                UPPER_BOUND,
                BOUNDED,
            };
            
            enum BoundType {
                LESS,
                LESS_EQUAL,
                GREATER,
                GREATER_EQUAL,
                EQUAL
            };
            
            enum ModelSense {
                MINIMIZE,
                MAXIMIZE
            };
            
            LpSolver(ModelSense const& modelSense) : modelSense(modelSense) {
                // Intentionally left empty.
            }
            
            virtual uint_fast64_t createContinuousVariable(std::string const& name, VariableType const& variableType, double lowerBound, double upperBound, double objectiveFunctionCoefficient) = 0;
            virtual uint_fast64_t createIntegerVariable(std::string const& name, VariableType const& variableType, double lowerBound, double upperBound, double objectiveFunctionCoefficient) = 0;
            virtual uint_fast64_t createBinaryVariable(std::string const& name, double objectiveFunctionCoefficient) = 0;
            
            virtual void addConstraint(std::string const& name, std::vector<uint_fast64_t> const& variables, std::vector<double> const& coefficients, BoundType const& boundType, double rightHandSideValue) = 0;
            
            virtual void optimize() const = 0;
            
            virtual int_fast64_t getIntegerValue(uint_fast64_t variableIndex) const = 0;
            virtual bool getBinaryValue(uint_fast64_t variableIndex) const = 0;
            virtual double getContinuousValue(uint_fast64_t variableIndex) const = 0;

            virtual void setModelSense(ModelSense const& newModelSense) {
                this->modelSense = newModelSense;
            }
            
            ModelSense getModelSense() const {
                return modelSense;
            }
            
        private:
            ModelSense modelSense;
        };
    }
}

#endif /* STORM_SOLVER_LPSOLVER */
