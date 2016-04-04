#ifndef STORM_GENERATOR_PRISMSTATELABELINGGENERATOR_H_
#define STORM_GENERATOR_PRISMSTATELABELINGGENERATOR_H_

#include "src/generator/StateLabelingGenerator.h"

#include "src/generator/VariableInformation.h"

#include "src/storage/prism/Program.h"

namespace storm {
    namespace generator {
        
        template<typename ValueType, typename StateType = uint32_t>
        class PrismStateLabelingGenerator : public StateLabelingGenerator<StateType> {
        public:
            PrismStateLabelingGenerator(storm::prism::Program const& program, VariableInformation const& variableInformation);
            
            virtual storm::models::sparse::StateLabeling generate(storm::storage::BitVectorHashMap<StateType> const& states, std::vector<StateType> const& initialStateIndices = {}) override;
            
        private:
            // The program for which to generate the labels.
            storm::prism::Program const& program;
            
            // Information about how the variables are packed.
            VariableInformation const& variableInformation;
        };
        
    }
}

#endif /* STORM_GENERATOR_PRISMSTATELABELINGGENERATOR_H_ */