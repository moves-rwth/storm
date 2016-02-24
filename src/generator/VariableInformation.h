#ifndef STORM_GENERATOR_PRISM_VARIABLEINFORMATION_H_
#define STORM_GENERATOR_PRISM_VARIABLEINFORMATION_H_

#include <vector>
#include <boost/container/flat_map.hpp>

#include "src/storage/expressions/Variable.h"
#include "src/storage/prism/Program.h"

namespace storm {
    namespace generator {
        
        // A structure storing information about the boolean variables of the program.
        struct BooleanVariableInformation {
            BooleanVariableInformation(storm::expressions::Variable const& variable, bool initialValue, uint_fast64_t bitOffset);
            
            // The boolean variable.
            storm::expressions::Variable variable;
            
            // Its initial value.
            bool initialValue;
            
            // Its bit offset in the compressed state.
            uint_fast64_t bitOffset;
        };
        
        // A structure storing information about the integer variables of the program.
        struct IntegerVariableInformation {
            IntegerVariableInformation(storm::expressions::Variable const& variable, int_fast64_t initialValue, int_fast64_t lowerBound, int_fast64_t upperBound, uint_fast64_t bitOffset, uint_fast64_t bitWidth);
            
            // The integer variable.
            storm::expressions::Variable variable;
            
            // Its initial value.
            int_fast64_t initialValue;
            
            // The lower bound of its range.
            int_fast64_t lowerBound;
            
            // The upper bound of its range.
            int_fast64_t upperBound;
            
            // Its bit offset in the compressed state.
            uint_fast64_t bitOffset;
            
            // Its bit width in the compressed state.
            uint_fast64_t bitWidth;
        };
        
        // A structure storing information about the used variables of the program.
        struct VariableInformation {
            VariableInformation(storm::prism::Program const& program);
            uint_fast64_t getTotalBitOffset() const;
            
            // Provide methods to access the bit offset and width of variables in the compressed state.
            uint_fast64_t getBitOffset(storm::expressions::Variable const& variable) const;
            uint_fast64_t getBitWidth(storm::expressions::Variable const& variable) const;
            
            // The total bit offset over all variables.
            uint_fast64_t totalBitOffset;
            
            // The known boolean variables.
            boost::container::flat_map<storm::expressions::Variable, uint_fast64_t> booleanVariableToIndexMap;
            std::vector<BooleanVariableInformation> booleanVariables;
            
            // The known integer variables.
            boost::container::flat_map<storm::expressions::Variable, uint_fast64_t> integerVariableToIndexMap;
            std::vector<IntegerVariableInformation> integerVariables;
        };
        
    }
}

#endif /* STORM_GENERATOR_PRISM_VARIABLEINFORMATION_H_ */