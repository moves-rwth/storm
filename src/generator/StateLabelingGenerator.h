#ifndef STORM_GENERATOR_STATELABELINGGENERATOR_H_
#define STORM_GENERATOR_STATELABELINGGENERATOR_H_

#include "src/models/sparse/StateLabeling.h"

#include "src/storage/BitVectorHashMap.h"

namespace storm {
    namespace generator {
        
        template<typename StateType = uint32_t>
        class StateLabelingGenerator {
        public:
            virtual storm::models::sparse::StateLabeling generate(storm::storage::BitVectorHashMap<StateType> const& states, std::vector<StateType> const& initialStateIndices = {}) = 0;
        };
        
    }
}

#endif /* STORM_GENERATOR_STATELABELINGGENERATOR_H_ */