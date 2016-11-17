#ifndef STORM_MODELS_SPARSE_STATEANNOTATION_H_
#define STORM_MODELS_SPARSE_STATEANNOTATION_H_

#include "src/storage/sparse/StateType.h"

namespace storm {
    namespace models {
        namespace sparse {
            
            class StateAnnotation {
            public:
                virtual std::string stateInfo(storm::storage::sparse::state_type const& state) const = 0;
            };
            
        }
    }
}

#endif /* STORM_MODELS_SPARSE_STATEANNOTATION_H_ */